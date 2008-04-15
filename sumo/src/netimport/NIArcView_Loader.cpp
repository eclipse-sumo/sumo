/****************************************************************************/
/// @file    NIArcView_Loader.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id:NIArcView_Loader.cpp 4701 2007-11-09 14:29:29Z dkrajzew $
///
// Importer for networks stored in ArcView-shape format
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/common/FileErrorReporter.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/common/TplConvert.h>
#include <utils/importio/LineHandler.h>
#include <utils/importio/LineReader.h>
#include <utils/common/StringUtils.h>
#include <utils/options/OptionsCont.h>
#include <utils/geom/GeomHelper.h>
#include <netbuild/NBHelpers.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBTypeCont.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include "NIArcView_Loader.h"
#include <netimport/NINavTeqHelper.h>
#include <utils/geom/GeoConvHelper.h>

#ifdef HAVE_GDAL
#include <ogrsf_frmts.h>
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
NIArcView_Loader::NIArcView_Loader(OptionsCont &oc,
                                   NBNodeCont &nc,
                                   NBEdgeCont &ec,
                                   NBTypeCont &tc,
                                   const std::string &dbf_name,
                                   const std::string &shp_name,
                                   bool speedInKMH,
                                   bool useNewLaneNumberInfoPlain)
        : FileErrorReporter("Navtech Edge description", dbf_name),
        myOptions(oc), mySHPName(shp_name),
        myNameAddition(0),
        myNodeCont(nc), myEdgeCont(ec), myTypeCont(tc),
        mySpeedInKMH(speedInKMH),
        myUseNewLaneNumberInfoPlain(useNewLaneNumberInfoPlain),
        myRunningNodeID(0)
{}


NIArcView_Loader::~NIArcView_Loader()
{}


bool
NIArcView_Loader::load(OptionsCont &)
{
#ifdef HAVE_GDAL
    MsgHandler::getMessageInstance()->beginProcessMsg("Loading data from '" + mySHPName + "'...");
    OGRRegisterAll();
    OGRDataSource       *poDS;

    poDS = OGRSFDriverRegistrar::Open(mySHPName.c_str(), FALSE);
    if (poDS == NULL) {
        MsgHandler::getErrorInstance()->inform("Could not open shape description '" + mySHPName + "'.");
        return false;
    }

    // begin file parsing
    OGRLayer  *poLayer = poDS->GetLayer(0);
    poLayer->ResetReading();

    // build coordinate transformation
    OGRSpatialReference *origTransf = poLayer->GetSpatialRef();
    OGRSpatialReference destTransf;
    // use wgs84 as destination
    destTransf.SetWellKnownGeogCS("WGS84");
    OGRCoordinateTransformation *poCT =
        OGRCreateCoordinateTransformation(origTransf, &destTransf);
    if (poCT == NULL) {
        if (myOptions.isSet("arcview.guess-projection")) {
            OGRSpatialReference origTransf2;
            origTransf2.SetWellKnownGeogCS("WGS84");
            poCT = OGRCreateCoordinateTransformation(&origTransf2, &destTransf);
        }
        if (poCT==0) {
            WRITE_WARNING("Could not create geocoordinates converter; check whether proj.4 is installed.");
        }
    }

    OGRFeature *poFeature;
    poLayer->ResetReading();
    while ((poFeature = poLayer->GetNextFeature()) != NULL) {
        // read in edge attributes
        string id =
            myOptions.isSet("arcview.street-id")
            ? poFeature->GetFieldAsString((char*)(myOptions.getString("arcview.street-id").c_str()))
            : poFeature->GetFieldAsString("LINK_ID");
        id = StringUtils::prune(id);
        string name =
            myOptions.isSet("arcview.street-id")
            ? poFeature->GetFieldAsString((char*) myOptions.getString("arcview.street-id").c_str())
            : poFeature->GetFieldAsString("ST_NAME");
        name = StringUtils::prune(StringUtils::replace(name, "&", "&amp;"));

        string from_node =
            myOptions.isSet("arcview.from-id")
            ? poFeature->GetFieldAsString((char*)(myOptions.getString("arcview.from-id").c_str()))
            : poFeature->GetFieldAsString("REF_IN_ID");
        from_node = StringUtils::prune(from_node);
        string to_node =
            myOptions.isSet("arcview.to-id")
            ? poFeature->GetFieldAsString((char*) myOptions.getString("arcview.to-id").c_str())
            : poFeature->GetFieldAsString("NREF_IN_ID");
        to_node = StringUtils::prune(to_node);
        if (from_node==""||to_node=="") {
            from_node = toString(myRunningNodeID++);
            to_node = toString(myRunningNodeID++);
        }
        string type = poFeature->GetFieldAsString("ST_TYP_AFT");
        SUMOReal speed = getSpeed(*poFeature, id);
        size_t nolanes = getLaneNo(*poFeature, id, speed, myUseNewLaneNumberInfoPlain);
        int priority = getPriority(*poFeature, id);
        if (nolanes==0||speed==0) {
            if (myOptions.getBool("arcview.use-defaults-on-failure")) {
                nolanes = myTypeCont.getDefaultNoLanes();
                speed = myTypeCont.getDefaultSpeed();
            } else {
                OGRFeature::DestroyFeature(poFeature);
                MsgHandler::getErrorInstance()->inform("The description seems to be invalid;Please recheck usage of types.");
                return false;
            }
        }
        if (mySpeedInKMH) {
            speed = speed / (SUMOReal) 3.6;
        }

        NBEdge::EdgeBasicFunction function = NBEdge::EDGEFUNCTION_NORMAL;
        NBNode *from = 0;
        NBNode *to = 0;

        // read in the geometry
        OGRGeometry *poGeometry = poFeature->GetGeometryRef();
        OGRwkbGeometryType gtype = poGeometry->getGeometryType();
        assert(gtype==wkbLineString);
        OGRLineString *cgeom = (OGRLineString*) poGeometry;
        if (poCT!=0) {
            // try transform to wgs84
            cgeom->transform(poCT);
        }

        Position2DVector shape;
        for (int j=0; j<cgeom->getNumPoints(); j++) {
            Position2D pos((SUMOReal) cgeom->getX(j), (SUMOReal) cgeom->getY(j));
            GeoConvHelper::x2cartesian(pos);
            shape.push_back_noDoublePos(pos);
        }

        // build from-node
        Position2D from_pos = shape[0];
        if (!myNodeCont.insert(from_node, from_pos)) {
            from = new NBNode(from_node + "___" + toString<int>(myNameAddition++),
                              from_pos);
            myNodeCont.insert(from);
        } else {
            from = myNodeCont.retrieve(from_pos);
        }
        // build to-node
        Position2D to_pos = shape[-1];
        if (!myNodeCont.insert(to_node, to_pos)) {
            to = new NBNode(to_node + "___" + toString<int>(myNameAddition++),
                            to_pos);
            myNodeCont.insert(to);
        } else {
            to = myNodeCont.retrieve(to_pos);
        }

        if (from==0||to==0) {
            WRITE_WARNING("Could not insert one of the nodes edge '" + id + "' shall connect; skipping.");
            continue;
        }

        if (from==to) {
            WRITE_WARNING("Edge '" + id + "' connects same nodes; skipping.");
            continue;
        }
        if (from->getPosition().almostSame(to->getPosition())) {
            WRITE_WARNING("Edge '" + id + "' is too short; skipping.");
            continue;
        }

        // retrieve the information whether the street is bi-directional
        string dir;
        int index = poFeature->GetDefnRef()->GetFieldIndex("DIR_TRAVEL");
        if (index>=0&&poFeature->IsFieldSet(index)) {
            dir = poFeature->GetFieldAsString(index);
        }
        // add positive direction if wanted
        if (dir=="B"||dir=="F"||dir==""||myOptions.getBool("arcview.all-bidi")) {
            if (myEdgeCont.retrieve(id)==0) {
                NBEdge::LaneSpreadFunction spread =
                    dir=="B"||dir=="FALSE"
                    ? NBEdge::LANESPREAD_RIGHT
                    : NBEdge::LANESPREAD_CENTER;
                NBEdge *edge = new NBEdge(id, from, to, type, speed, nolanes,
                                          priority, shape, spread, function);
                myEdgeCont.insert(edge);
            }
        }
        // add negative direction if wanted
        if (dir=="B"||dir=="T"||myOptions.getBool("arcview.all-bidi")) {
            id = "-" + id;
            if (myEdgeCont.retrieve(id)==0) {
                NBEdge::LaneSpreadFunction spread =
                    dir=="B"||dir=="FALSE"
                    ? NBEdge::LANESPREAD_RIGHT
                    : NBEdge::LANESPREAD_CENTER;
                NBEdge *edge = new NBEdge(id, to, from, type, speed, nolanes,
                                          priority, shape.reverse(), spread, function);
                myEdgeCont.insert(edge);
            }
        }
        OGRFeature::DestroyFeature(poFeature);
    }
    MsgHandler::getMessageInstance()->endProcessMsg("done.");
    return !MsgHandler::getErrorInstance()->wasInformed();
#else
    MsgHandler::getErrorInstance()->inform("SUMO was compiled without GDAL support.");
    return false;
#endif
}

#ifdef HAVE_GDAL
SUMOReal
NIArcView_Loader::getSpeed(OGRFeature &poFeature, const std::string &edgeid)
{
    if (myOptions.isSet("arcview.type-id")) {
        return myTypeCont.getSpeed(poFeature.GetFieldAsString((char*)(myOptions.getString("arcview.type-id").c_str())));
    }
    // try to get definitions as to be found in SUMO-XML-definitions
    //  idea by John Michael Calandrino
    int index = poFeature.GetDefnRef()->GetFieldIndex("speed");
    if (index>=0&&poFeature.IsFieldSet(index)) {
        return (SUMOReal) poFeature.GetFieldAsDouble(index);
    }
    index = poFeature.GetDefnRef()->GetFieldIndex("SPEED");
    if (index>=0&&poFeature.IsFieldSet(index)) {
        return (SUMOReal) poFeature.GetFieldAsDouble(index);
    }
    // try to get the NavTech-information
    index = poFeature.GetDefnRef()->GetFieldIndex("SPEED_CAT");
    if (index>=0&&poFeature.IsFieldSet(index)) {
        string def = poFeature.GetFieldAsString(index);
        return NINavTeqHelper::getSpeed(edgeid, def);
    }
    return -1;
}


size_t
NIArcView_Loader::getLaneNo(OGRFeature &poFeature, const std::string &edgeid,
                            SUMOReal speed, bool /*useNewLaneNumberInfoPlain*/)
{
    if (myOptions.isSet("arcview.type-id")) {
        return myTypeCont.getNoLanes(poFeature.GetFieldAsString((char*)(myOptions.getString("arcview.type-id").c_str())));
    }
    // try to get definitions as to be found in SUMO-XML-definitions
    //  idea by John Michael Calandrino
    int index = poFeature.GetDefnRef()->GetFieldIndex("nolanes");
    if (index>=0&&poFeature.IsFieldSet(index)) {
        return poFeature.GetFieldAsInteger(index);
    }
    index = poFeature.GetDefnRef()->GetFieldIndex("NOLANES");
    if (index>=0&&poFeature.IsFieldSet(index)) {
        return poFeature.GetFieldAsInteger(index);
    }
    index = poFeature.GetDefnRef()->GetFieldIndex("rnol");
    if (index>=0&&poFeature.IsFieldSet(index)) {
        return poFeature.GetFieldAsInteger(index);
    }
    index = poFeature.GetDefnRef()->GetFieldIndex("LANE_CAT");
    if (index>=0&&poFeature.IsFieldSet(index)) {
        string def = poFeature.GetFieldAsString(index);
        return NINavTeqHelper::getLaneNumber(edgeid, def, speed, myUseNewLaneNumberInfoPlain);
    }
    return 0;
}


int
NIArcView_Loader::getPriority(OGRFeature &poFeature, const std::string &/*edgeid*/)
{
    if (myOptions.isSet("arcview.type-id")) {
        return myTypeCont.getPriority(poFeature.GetFieldAsString((char*)(myOptions.getString("arcview.type-id").c_str())));
    }
    // try to get definitions as to be found in SUMO-XML-definitions
    //  idea by John Michael Calandrino
    int index = poFeature.GetDefnRef()->GetFieldIndex("priority");
    if (index>=0&&poFeature.IsFieldSet(index)) {
        return poFeature.GetFieldAsInteger(index);
    }
    index = poFeature.GetDefnRef()->GetFieldIndex("PRIORITY");
    if (index>=0&&poFeature.IsFieldSet(index)) {
        return poFeature.GetFieldAsInteger(index);
    }
    // try to determine priority from NavTechs FUNC_CLASS attribute
    index = poFeature.GetDefnRef()->GetFieldIndex("FUNC_CLASS");
    if (index>=0&&poFeature.IsFieldSet(index)) {
        return poFeature.GetFieldAsInteger(index);
    }
    return 0;
}
#endif



/****************************************************************************/

