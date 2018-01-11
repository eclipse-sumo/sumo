/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NIImporter_ArcView.cpp
/// @author  Daniel Krajzewicz
/// @author  Eric Nicolay
/// @author  Jakob Erdmann
/// @author  Thimor Bohn
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// Importer for networks stored in ArcView-shape format
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
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/common/TplConvert.h>
#include <utils/common/StringUtils.h>
#include <utils/options/OptionsCont.h>
#include <utils/geom/GeomHelper.h>
#include <netbuild/NBNetBuilder.h>
#include <netbuild/NBHelpers.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBTypeCont.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include <netimport/NINavTeqHelper.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/common/FileHelpers.h>
#include "NILoader.h"
#include "NIImporter_ArcView.h"

#ifdef HAVE_GDAL
#if __GNUC__ > 3
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
#include <ogrsf_frmts.h>
#if __GNUC__ > 3
#pragma GCC diagnostic pop
#endif
#endif


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static methods (interface in this case)
// ---------------------------------------------------------------------------
void
NIImporter_ArcView::loadNetwork(const OptionsCont& oc, NBNetBuilder& nb) {
    if (!oc.isSet("shapefile-prefix")) {
        return;
    }
    // check whether the correct set of entries is given
    //  and compute both file names
    std::string dbf_file = oc.getString("shapefile-prefix") + ".dbf";
    std::string shp_file = oc.getString("shapefile-prefix") + ".shp";
    std::string shx_file = oc.getString("shapefile-prefix") + ".shx";
    // check whether the files do exist
    if (!FileHelpers::isReadable(dbf_file)) {
        WRITE_ERROR("File not accessible: " + dbf_file);
    }
    if (!FileHelpers::isReadable(shp_file)) {
        WRITE_ERROR("File not accessible: " + shp_file);
    }
    if (!FileHelpers::isReadable(shx_file)) {
        WRITE_ERROR("File not accessible: " + shx_file);
    }
    if (MsgHandler::getErrorInstance()->wasInformed()) {
        return;
    }
    // load the arcview files
    NIImporter_ArcView loader(oc,
                              nb.getNodeCont(), nb.getEdgeCont(), nb.getTypeCont(),
                              dbf_file, shp_file, oc.getBool("speed-in-kmh"));
    loader.load();
}



// ---------------------------------------------------------------------------
// loader methods
// ---------------------------------------------------------------------------
NIImporter_ArcView::NIImporter_ArcView(const OptionsCont& oc,
                                       NBNodeCont& nc,
                                       NBEdgeCont& ec,
                                       NBTypeCont& tc,
                                       const std::string& dbf_name,
                                       const std::string& shp_name,
                                       bool speedInKMH)
    : myOptions(oc), mySHPName(shp_name),
      myNameAddition(0),
      myNodeCont(nc), myEdgeCont(ec), myTypeCont(tc),
      mySpeedInKMH(speedInKMH),
      myRunningNodeID(0) {
    UNUSED_PARAMETER(dbf_name);
}


NIImporter_ArcView::~NIImporter_ArcView() {}


void
NIImporter_ArcView::load() {
#ifdef HAVE_GDAL
    PROGRESS_BEGIN_MESSAGE("Loading data from '" + mySHPName + "'");
#if GDAL_VERSION_MAJOR < 2
    OGRRegisterAll();
    OGRDataSource* poDS = OGRSFDriverRegistrar::Open(mySHPName.c_str(), FALSE);
#else
    GDALAllRegister();
    GDALDataset* poDS = (GDALDataset*)GDALOpenEx(mySHPName.c_str(), GDAL_OF_VECTOR | GA_ReadOnly, NULL, NULL, NULL);
#endif
    if (poDS == NULL) {
        WRITE_ERROR("Could not open shape description '" + mySHPName + "'.");
        return;
    }

    // begin file parsing
    OGRLayer*  poLayer = poDS->GetLayer(0);
    poLayer->ResetReading();

    // build coordinate transformation
    OGRSpatialReference* origTransf = poLayer->GetSpatialRef();
    OGRSpatialReference destTransf;
    // use wgs84 as destination
    destTransf.SetWellKnownGeogCS("WGS84");
    OGRCoordinateTransformation* poCT = OGRCreateCoordinateTransformation(origTransf, &destTransf);
    if (poCT == NULL) {
        if (myOptions.isSet("shapefile.guess-projection")) {
            OGRSpatialReference origTransf2;
            origTransf2.SetWellKnownGeogCS("WGS84");
            poCT = OGRCreateCoordinateTransformation(&origTransf2, &destTransf);
        }
        if (poCT == 0) {
            WRITE_WARNING("Could not create geocoordinates converter; check whether proj.4 is installed.");
        }
    }

    const bool saveOrigIDs = OptionsCont::getOptions().getBool("output.original-names");
    OGRFeature* poFeature;
    poLayer->ResetReading();
    while ((poFeature = poLayer->GetNextFeature()) != NULL) {
        // read in edge attributes
        std::string id, name, from_node, to_node;
        if (!getStringEntry(poFeature, "shapefile.street-id", "LINK_ID", true, id)) {
            WRITE_ERROR("Needed field '" + id + "' (from node id) is missing.");
        }
        if (id == "") {
            WRITE_ERROR("Could not obtain edge id.");
            return;
        }

        getStringEntry(poFeature, "shapefile.street-id", "ST_NAME", true, name);
        name = StringUtils::replace(name, "&", "&amp;");

        if (!getStringEntry(poFeature, "shapefile.from-id", "REF_IN_ID", true, from_node)) {
            WRITE_ERROR("Needed field '" + from_node + "' (from node id) is missing.");
        }
        if (!getStringEntry(poFeature, "shapefile.to-id", "NREF_IN_ID", true, to_node)) {
            WRITE_ERROR("Needed field '" + to_node + "' (to node id) is missing.");
        }

        if (from_node == "" || to_node == "") {
            from_node = toString(myRunningNodeID++);
            to_node = toString(myRunningNodeID++);
        }

        std::string type;
        if (myOptions.isSet("shapefile.type-id") && poFeature->GetFieldIndex(myOptions.getString("shapefile.type-id").c_str()) >= 0) {
            type = poFeature->GetFieldAsString(myOptions.getString("shapefile.type-id").c_str());
        } else if (poFeature->GetFieldIndex("ST_TYP_AFT") >= 0) {
            type = poFeature->GetFieldAsString("ST_TYP_AFT");
        }
        double width = myTypeCont.getWidth(type);
        double speed = getSpeed(*poFeature, id);
        int nolanes = getLaneNo(*poFeature, id, speed);
        int priority = getPriority(*poFeature, id);
        if (nolanes == 0 || speed == 0) {
            if (myOptions.getBool("shapefile.use-defaults-on-failure")) {
                nolanes = myTypeCont.getNumLanes("");
                speed = myTypeCont.getSpeed("");
            } else {
                OGRFeature::DestroyFeature(poFeature);
                WRITE_ERROR("Required field 'nolanes' or 'speed' is missing (add fields or set option --shapefile.use-defaults-on-failure).");
                return;
            }
        }
        if (mySpeedInKMH) {
            speed = speed / (double) 3.6;
        }


        // read in the geometry
        OGRGeometry* poGeometry = poFeature->GetGeometryRef();
        OGRwkbGeometryType gtype = poGeometry->getGeometryType();
        if (gtype != wkbLineString) {
            OGRFeature::DestroyFeature(poFeature);
            WRITE_ERROR("Road geometry must be of type 'linestring'.");
            return;
        }
        OGRLineString* cgeom = (OGRLineString*) poGeometry;
        if (poCT != 0) {
            // try transform to wgs84
            cgeom->transform(poCT);
        }

        PositionVector shape;
        for (int j = 0; j < cgeom->getNumPoints(); j++) {
            Position pos((double) cgeom->getX(j), (double) cgeom->getY(j));
            if (!NBNetBuilder::transformCoordinate(pos)) {
                WRITE_WARNING("Unable to project coordinates for edge '" + id + "'.");
            }
            shape.push_back_noDoublePos(pos);
        }

        // build from-node
        NBNode* from = myNodeCont.retrieve(from_node);
        if (from == 0) {
            Position from_pos = shape[0];
            from = myNodeCont.retrieve(from_pos);
            if (from == 0) {
                from = new NBNode(from_node, from_pos);
                if (!myNodeCont.insert(from)) {
                    WRITE_ERROR("Node '" + from_node + "' could not be added");
                    delete from;
                    continue;
                }
            }
        }
        // build to-node
        NBNode* to = myNodeCont.retrieve(to_node);
        if (to == 0) {
            Position to_pos = shape[-1];
            to = myNodeCont.retrieve(to_pos);
            if (to == 0) {
                to = new NBNode(to_node, to_pos);
                if (!myNodeCont.insert(to)) {
                    WRITE_ERROR("Node '" + to_node + "' could not be added");
                    delete to;
                    continue;
                }
            }
        }

        if (from == to) {
            WRITE_WARNING("Edge '" + id + "' connects identical nodes, skipping.");
            continue;
        }

        // retrieve the information whether the street is bi-directional
        std::string dir;
        int index = poFeature->GetDefnRef()->GetFieldIndex("DIR_TRAVEL");
        if (index >= 0 && poFeature->IsFieldSet(index)) {
            dir = poFeature->GetFieldAsString(index);
        }
        const std::string origID = saveOrigIDs ? id : "";
        // add positive direction if wanted
        if (dir == "B" || dir == "F" || dir == "" || myOptions.getBool("shapefile.all-bidirectional")) {
            if (myEdgeCont.retrieve(id) == 0) {
                LaneSpreadFunction spread = dir == "B" || dir == "FALSE" ? LANESPREAD_RIGHT : LANESPREAD_CENTER;
                NBEdge* edge = new NBEdge(id, from, to, type, speed, nolanes, priority, width, NBEdge::UNSPECIFIED_OFFSET, shape, name, origID, spread);
                myEdgeCont.insert(edge);
                checkSpread(edge);
            }
        }
        // add negative direction if wanted
        if (dir == "B" || dir == "T" || myOptions.getBool("shapefile.all-bidirectional")) {
            if (myEdgeCont.retrieve("-" + id) == 0) {
                LaneSpreadFunction spread = dir == "B" || dir == "FALSE" ? LANESPREAD_RIGHT : LANESPREAD_CENTER;
                NBEdge* edge = new NBEdge("-" + id, to, from, type, speed, nolanes, priority, width, NBEdge::UNSPECIFIED_OFFSET, shape.reverse(), name, origID, spread);
                myEdgeCont.insert(edge);
                checkSpread(edge);
            }
        }
        //
        OGRFeature::DestroyFeature(poFeature);
    }
#if GDAL_VERSION_MAJOR < 2
    OGRDataSource::DestroyDataSource(poDS);
#else
    GDALClose(poDS);
#endif
    PROGRESS_DONE_MESSAGE();
#else
    WRITE_ERROR("SUMO was compiled without GDAL support.");
#endif
}

#ifdef HAVE_GDAL
double
NIImporter_ArcView::getSpeed(OGRFeature& poFeature, const std::string& edgeid) {
    if (myOptions.isSet("shapefile.type-id")) {
        return myTypeCont.getSpeed(poFeature.GetFieldAsString((char*)(myOptions.getString("shapefile.type-id").c_str())));
    }
    // try to get definitions as to be found in SUMO-XML-definitions
    //  idea by John Michael Calandrino
    int index = poFeature.GetDefnRef()->GetFieldIndex("speed");
    if (index >= 0 && poFeature.IsFieldSet(index)) {
        return (double) poFeature.GetFieldAsDouble(index);
    }
    index = poFeature.GetDefnRef()->GetFieldIndex("SPEED");
    if (index >= 0 && poFeature.IsFieldSet(index)) {
        return (double) poFeature.GetFieldAsDouble(index);
    }
    // try to get the NavTech-information
    index = poFeature.GetDefnRef()->GetFieldIndex("SPEED_CAT");
    if (index >= 0 && poFeature.IsFieldSet(index)) {
        std::string def = poFeature.GetFieldAsString(index);
        return NINavTeqHelper::getSpeed(edgeid, def);
    }
    return -1;
}


int
NIImporter_ArcView::getLaneNo(OGRFeature& poFeature, const std::string& edgeid,
                              double speed) {
    if (myOptions.isSet("shapefile.type-id")) {
        return (int) myTypeCont.getNumLanes(poFeature.GetFieldAsString((char*)(myOptions.getString("shapefile.type-id").c_str())));
    }
    // try to get definitions as to be found in SUMO-XML-definitions
    //  idea by John Michael Calandrino
    int index = poFeature.GetDefnRef()->GetFieldIndex("nolanes");
    if (index >= 0 && poFeature.IsFieldSet(index)) {
        return (int) poFeature.GetFieldAsInteger(index);
    }
    index = poFeature.GetDefnRef()->GetFieldIndex("NOLANES");
    if (index >= 0 && poFeature.IsFieldSet(index)) {
        return (int) poFeature.GetFieldAsInteger(index);
    }
    index = poFeature.GetDefnRef()->GetFieldIndex("rnol");
    if (index >= 0 && poFeature.IsFieldSet(index)) {
        return (int) poFeature.GetFieldAsInteger(index);
    }
    index = poFeature.GetDefnRef()->GetFieldIndex("LANE_CAT");
    if (index >= 0 && poFeature.IsFieldSet(index)) {
        std::string def = poFeature.GetFieldAsString(index);
        return NINavTeqHelper::getLaneNumber(edgeid, def, speed);
    }
    return 0;
}


int
NIImporter_ArcView::getPriority(OGRFeature& poFeature, const std::string& /*edgeid*/) {
    if (myOptions.isSet("shapefile.type-id")) {
        return myTypeCont.getPriority(poFeature.GetFieldAsString((char*)(myOptions.getString("shapefile.type-id").c_str())));
    }
    // try to get definitions as to be found in SUMO-XML-definitions
    //  idea by John Michael Calandrino
    int index = poFeature.GetDefnRef()->GetFieldIndex("priority");
    if (index >= 0 && poFeature.IsFieldSet(index)) {
        return poFeature.GetFieldAsInteger(index);
    }
    index = poFeature.GetDefnRef()->GetFieldIndex("PRIORITY");
    if (index >= 0 && poFeature.IsFieldSet(index)) {
        return poFeature.GetFieldAsInteger(index);
    }
    // try to determine priority from NavTechs FUNC_CLASS attribute
    index = poFeature.GetDefnRef()->GetFieldIndex("FUNC_CLASS");
    if (index >= 0 && poFeature.IsFieldSet(index)) {
        return poFeature.GetFieldAsInteger(index);
    }
    return 0;
}

void
NIImporter_ArcView::checkSpread(NBEdge* e) {
    NBEdge* ret = e->getToNode()->getConnectionTo(e->getFromNode());
    if (ret != 0) {
        e->setLaneSpreadFunction(LANESPREAD_RIGHT);
        ret->setLaneSpreadFunction(LANESPREAD_RIGHT);
    }
}

bool
NIImporter_ArcView::getStringEntry(OGRFeature* poFeature, const std::string& optionName, const char* defaultName, bool prune, std::string& into) {
    std::string v(defaultName);
    if (myOptions.isSet(optionName)) {
        v = myOptions.getString(optionName);
    }
    if (poFeature->GetFieldIndex(v.c_str()) < 0) {
        if (myOptions.isSet(optionName)) {
            into = v;
            return false;
        }
        into = "";
        return true;
    }
    into = poFeature->GetFieldAsString((char*)v.c_str());
    if (prune) {
        into = StringUtils::prune(into);
    }
    return true;
}



#endif



/****************************************************************************/

