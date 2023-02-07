/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    NIImporter_ArcView.cpp
/// @author  Daniel Krajzewicz
/// @author  Eric Nicolay
/// @author  Jakob Erdmann
/// @author  Thimor Bohn
/// @author  Michael Behrisch
/// @date    Sept 2002
///
// Importer for networks stored in ArcView-shape format
/****************************************************************************/
#include <config.h>

#include <string>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/common/StringUtils.h>
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
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4435 5219 5220)
#endif
#if __GNUC__ > 3
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
#include <ogrsf_frmts.h>
#if __GNUC__ > 3
#pragma GCC diagnostic pop
#endif
#ifdef _MSC_VER
#pragma warning(pop)
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
      myRunningEdgeID(0),
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
        WRITE_ERRORF(TL("Could not open shape description '%'."), mySHPName);
        return;
    }

    // begin file parsing
    OGRLayer* poLayer = poDS->GetLayer(0);
    poLayer->ResetReading();

    // build coordinate transformation
    OGRSpatialReference* origTransf = poLayer->GetSpatialRef();
    OGRSpatialReference destTransf;
    // use wgs84 as destination
    destTransf.SetWellKnownGeogCS("WGS84");
#if GDAL_VERSION_MAJOR > 2
    if (myOptions.getBool("shapefile.traditional-axis-mapping")) {
        destTransf.SetAxisMappingStrategy(OAMS_TRADITIONAL_GIS_ORDER);
    }
#endif
    OGRCoordinateTransformation* poCT = origTransf == nullptr ? nullptr : OGRCreateCoordinateTransformation(origTransf, &destTransf);
    if (poCT == nullptr) {
        if (myOptions.getBool("shapefile.guess-projection")) {
            OGRSpatialReference origTransf2;
            origTransf2.SetWellKnownGeogCS("WGS84");
            poCT = OGRCreateCoordinateTransformation(&origTransf2, &destTransf);
        }
    }

    const bool saveOrigIDs = OptionsCont::getOptions().getBool("output.original-names");
    OGRFeature* poFeature;
    poLayer->ResetReading();

    const double nodeJoinDist = myOptions.getFloat("shapefile.node-join-dist");
    const std::vector<std::string> params = myOptions.getStringVector("shapefile.add-params");

    int featureIndex = 0;
    bool warnNotUnique = true;
    bool warnMissingProjection = true;
    std::string idPrefix = ""; // prefix for non-unique street-id values
    std::map<std::string, int> idIndex; // running index to make street-id unique
    while ((poFeature = poLayer->GetNextFeature()) != NULL) {
        // read in edge attributes
        if (featureIndex == 0) {
            WRITE_MESSAGE("Available fields: " + toString(getFieldNames(poFeature)));
        }
        std::string id;
        std::string name;
        std::string from_node;
        std::string to_node;
        if (!getStringEntry(poFeature, "shapefile.street-id", "LINK_ID", true, id)) {
            WRITE_ERRORF(TL("Needed field '%' (street-id) is missing."), id);
            id = "";
        }
        if (id == "") {
            id = toString(myRunningEdgeID++);
        }

        getStringEntry(poFeature, "shapefile.name", "ST_NAME", true, name);
        name = StringUtils::replace(name, "&", "&amp;");

        if (!getStringEntry(poFeature, "shapefile.from-id", "REF_IN_ID", true, from_node)) {
            WRITE_ERRORF(TL("Needed field '%' (from node id) is missing."), from_node);
            from_node = "";
        }
        if (!getStringEntry(poFeature, "shapefile.to-id", "NREF_IN_ID", true, to_node)) {
            WRITE_ERRORF(TL("Needed field '%' (to node id) is missing."), to_node);
            to_node = "";
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
        if ((type != "" || myOptions.isSet("shapefile.type-id")) && !myTypeCont.knows(type)) {
            WRITE_WARNINGF(TL("Unknown type '%' for edge '%'"), type, id);
        }
        bool oneway = myTypeCont.knows(type) ? myTypeCont.getEdgeTypeIsOneWay(type) : false;
        double speed = getSpeed(*poFeature, id);
        int nolanes = getLaneNo(*poFeature, id, speed);
        int priority = getPriority(*poFeature, id);
        double width = getLaneWidth(*poFeature, id, nolanes);
        double length = getLength(*poFeature, id);
        if (nolanes <= 0 || speed <= 0) {
            if (myOptions.getBool("shapefile.use-defaults-on-failure")) {
                nolanes = nolanes <= 0 ? myTypeCont.getEdgeTypeNumLanes(type) : nolanes;
                speed = speed <= 0 ? myTypeCont.getEdgeTypeSpeed(type) : speed;
            } else {
                const std::string lanesField = myOptions.isSet("shapefile.laneNumber") ? myOptions.getString("shapefile.laneNumber") : "nolanes";
                const std::string speedField = myOptions.isSet("shapefile.speed") ? myOptions.getString("shapefile.speed") : "speed";
                WRITE_ERRORF(TL("Required field '%' or '%' is missing (add fields or set option --shapefile.use-defaults-on-failure)."), lanesField, speedField);
                WRITE_ERROR("Available fields: " + toString(getFieldNames(poFeature)));
                OGRFeature::DestroyFeature(poFeature);
                return;
            }
        }
        if (mySpeedInKMH) {
            speed /= 3.6;
        }


        // read in the geometry
        OGRGeometry* poGeometry = poFeature->GetGeometryRef();
        OGRwkbGeometryType gtype = poGeometry->getGeometryType();
        if (gtype != wkbLineString && gtype != wkbLineString25D) {
            OGRFeature::DestroyFeature(poFeature);
            WRITE_ERRORF(TL("Road geometry must be of type 'linestring' or 'linestring25D' (found '%')"), toString(gtype));
            return;
        }
        OGRLineString* cgeom = (OGRLineString*) poGeometry;
        if (poCT == nullptr && warnMissingProjection) {
            int outOfRange = 0;
            for (int j = 0; j < cgeom->getNumPoints(); j++) {
                if (fabs(cgeom->getX(j)) > 180 || fabs(cgeom->getY(j)) > 90) {
                    outOfRange++;
                }
            }
            if (2 * outOfRange > cgeom->getNumPoints()) {
                WRITE_WARNING(TL("No coordinate system found and coordinates look already projected."));
                GeoConvHelper::init("!", GeoConvHelper::getProcessing().getOffset(), GeoConvHelper::getProcessing().getOrigBoundary(), GeoConvHelper::getProcessing().getConvBoundary());
            } else {
                WRITE_WARNING(TL("Could not find geo coordinate system, assuming WGS84."));
            }
            warnMissingProjection = false;
        }
        if (poCT != nullptr) {
            // try transform to wgs84
            cgeom->transform(poCT);
        }

        PositionVector shape;
        for (int j = 0; j < cgeom->getNumPoints(); j++) {
            Position pos(cgeom->getX(j), cgeom->getY(j), cgeom->getZ(j));
            if (!NBNetBuilder::transformCoordinate(pos)) {
                WRITE_WARNINGF(TL("Unable to project coordinates for edge '%'."), id);
            }
            shape.push_back_noDoublePos(pos);
        }

        // build from-node
        NBNode* from = myNodeCont.retrieve(from_node);
        if (from == nullptr) {
            Position from_pos = shape[0];
            from = myNodeCont.retrieve(from_pos, nodeJoinDist);
            if (from == nullptr) {
                from = new NBNode(from_node, from_pos);
                if (!myNodeCont.insert(from)) {
                    WRITE_ERRORF(TL("Node '%' could not be added"), from_node);
                    delete from;
                    continue;
                }
            }
        }
        // build to-node
        NBNode* to = myNodeCont.retrieve(to_node);
        if (to == nullptr) {
            Position to_pos = shape[-1];
            to = myNodeCont.retrieve(to_pos, nodeJoinDist);
            if (to == nullptr) {
                to = new NBNode(to_node, to_pos);
                if (!myNodeCont.insert(to)) {
                    WRITE_ERRORF(TL("Node '%' could not be added"), to_node);
                    delete to;
                    continue;
                }
            }
        }

        if (from == to) {
            WRITE_WARNINGF(TL("Edge '%' connects identical nodes, skipping."), id);
            continue;
        }

        // retrieve the information whether the street is bi-directional
        std::string dir;
        int index = poFeature->GetDefnRef()->GetFieldIndex("DIR_TRAVEL");
        if (index >= 0 && poFeature->IsFieldSet(index)) {
            dir = poFeature->GetFieldAsString(index);
        }
        const std::string origID = saveOrigIDs ? id : "";
        // check for duplicate ids
        NBEdge* const existing = myEdgeCont.retrieve(id);
        NBEdge* const existingReverse = myEdgeCont.retrieve("-" + id);
        if (existing != nullptr || existingReverse != nullptr) {
            if ((existing != nullptr && existing->getGeometry() == shape)
                    || (existingReverse != nullptr && existingReverse->getGeometry() == shape.reverse())) {
                WRITE_ERRORF(TL("Edge '% is not unique."), (existing != nullptr ? id : existingReverse->getID()));
            } else {
                if (idIndex.count(id) == 0) {
                    idIndex[id] = 0;
                }
                idIndex[id]++;
                idPrefix = id;
                id += "#" + toString(idIndex[id]);
                if (warnNotUnique) {
                    WRITE_WARNINGF(TL("Edge '%' is not unique. Renaming subsequent edge to '%'."), idPrefix, id);
                    warnNotUnique = false;
                }
            }
        }
        // add positive direction if wanted
        if (dir == "B" || dir == "F" || dir == "" || myOptions.getBool("shapefile.all-bidirectional")) {
            if (myEdgeCont.retrieve(id) == 0) {
                LaneSpreadFunction spread = dir == "B" || dir == "FALSE" ? LaneSpreadFunction::RIGHT : LaneSpreadFunction::CENTER;
                NBEdge* edge = new NBEdge(id, from, to, type, speed, NBEdge::UNSPECIFIED_FRICTION, nolanes, priority, width, NBEdge::UNSPECIFIED_OFFSET, shape, spread, name, origID);
                edge->setPermissions(myTypeCont.getEdgeTypePermissions(type));
                edge->setLoadedLength(length);
                myEdgeCont.insert(edge);
                checkSpread(edge);
                addParams(edge, poFeature, params);
            } else {
                WRITE_ERRORF(TL("Could not create edge '%'. An edge with the same id already exists."), id);
            }
        }
        // add negative direction if wanted
        if ((dir == "B" || dir == "T" || myOptions.getBool("shapefile.all-bidirectional")) && !oneway) {
            if (myEdgeCont.retrieve("-" + id) == 0) {
                LaneSpreadFunction spread = dir == "B" || dir == "FALSE" ? LaneSpreadFunction::RIGHT : LaneSpreadFunction::CENTER;
                NBEdge* edge = new NBEdge("-" + id, to, from, type, speed, NBEdge::UNSPECIFIED_FRICTION, nolanes, priority, width, NBEdge::UNSPECIFIED_OFFSET, shape.reverse(), spread, name, origID);
                edge->setPermissions(myTypeCont.getEdgeTypePermissions(type));
                edge->setLoadedLength(length);
                myEdgeCont.insert(edge);
                checkSpread(edge);
                addParams(edge, poFeature, params);
            } else {
                WRITE_ERRORF(TL("Could not create edge '-%'. An edge with the same id already exists."), id);
            }
        }
        //
        OGRFeature::DestroyFeature(poFeature);
        featureIndex++;
    }
#if GDAL_VERSION_MAJOR < 2
    OGRDataSource::DestroyDataSource(poDS);
#else
    GDALClose(poDS);
#endif
    PROGRESS_DONE_MESSAGE();
#else
    WRITE_ERROR(TL("SUMO was compiled without GDAL support."));
#endif
}

#ifdef HAVE_GDAL
double
NIImporter_ArcView::getSpeed(OGRFeature& poFeature, const std::string& edgeid) {
    if (myOptions.isSet("shapefile.speed")) {
        int index = poFeature.GetDefnRef()->GetFieldIndex(myOptions.getString("shapefile.speed").c_str());
        if (index >= 0 && poFeature.IsFieldSet(index)) {
            const double speed = poFeature.GetFieldAsDouble(index);
            if (speed <= 0) {
                WRITE_WARNING("invalid value for field: '"
                              + myOptions.getString("shapefile.speed")
                              + "': '" + std::string(poFeature.GetFieldAsString(index)) + "'");
            } else {
                return speed;
            }
        }
    }
    if (myOptions.isSet("shapefile.type-id")) {
        return myTypeCont.getEdgeTypeSpeed(poFeature.GetFieldAsString((char*)(myOptions.getString("shapefile.type-id").c_str())));
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


double
NIImporter_ArcView::getLaneWidth(OGRFeature& poFeature, const std::string& edgeid, int laneNumber) {
    if (myOptions.isSet("shapefile.width")) {
        int index = poFeature.GetDefnRef()->GetFieldIndex(myOptions.getString("shapefile.width").c_str());
        if (index >= 0 && poFeature.IsFieldSet(index)) {
            const double width = poFeature.GetFieldAsDouble(index);
            if (width <= 0) {
                WRITE_WARNING("invalid value for field: '"
                              + myOptions.getString("shapefile.width")
                              + "' of edge '" + edgeid
                              + "': '" + std::string(poFeature.GetFieldAsString(index)) + "'");
            } else {
                return width / laneNumber;
            }
        }
    }
    if (myOptions.isSet("shapefile.type-id")) {
        return myTypeCont.getEdgeTypeWidth(poFeature.GetFieldAsString((char*)(myOptions.getString("shapefile.type-id").c_str())));
    }
    return NBEdge::UNSPECIFIED_WIDTH;
}



double
NIImporter_ArcView::getLength(OGRFeature& poFeature, const std::string& edgeid) {
    if (myOptions.isSet("shapefile.length")) {
        int index = poFeature.GetDefnRef()->GetFieldIndex(myOptions.getString("shapefile.length").c_str());
        if (index >= 0 && poFeature.IsFieldSet(index)) {
            const double length = poFeature.GetFieldAsDouble(index);
            if (length <= 0) {
                WRITE_WARNING("invalid value for field: '"
                              + myOptions.getString("shapefile.length")
                              + "' of edge '" + edgeid
                              + "': '" + std::string(poFeature.GetFieldAsString(index)) + "'");
            } else {
                return length;
            }
        }
    }
    return NBEdge::UNSPECIFIED_LOADED_LENGTH;
}


int
NIImporter_ArcView::getLaneNo(OGRFeature& poFeature, const std::string& edgeid,
                              double speed) {
    if (myOptions.isSet("shapefile.laneNumber")) {
        int index = poFeature.GetDefnRef()->GetFieldIndex(myOptions.getString("shapefile.laneNumber").c_str());
        if (index >= 0 && poFeature.IsFieldSet(index)) {
            const int laneNumber = poFeature.GetFieldAsInteger(index);
            if (laneNumber <= 0) {
                WRITE_WARNING("invalid value for field '"
                              + myOptions.getString("shapefile.laneNumber")
                              + "': '" + std::string(poFeature.GetFieldAsString(index)) + "'");
            } else {
                return laneNumber;
            }
        }
    }
    if (myOptions.isSet("shapefile.type-id")) {
        return (int) myTypeCont.getEdgeTypeNumLanes(poFeature.GetFieldAsString((char*)(myOptions.getString("shapefile.type-id").c_str())));
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
        return myTypeCont.getEdgeTypePriority(poFeature.GetFieldAsString((char*)(myOptions.getString("shapefile.type-id").c_str())));
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
        e->setLaneSpreadFunction(LaneSpreadFunction::RIGHT);
        ret->setLaneSpreadFunction(LaneSpreadFunction::RIGHT);
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

std::vector<std::string>
NIImporter_ArcView::getFieldNames(OGRFeature* poFeature) const {
    std::vector<std::string> fields;
    for (int i = 0; i < poFeature->GetFieldCount(); i++) {
        fields.push_back(poFeature->GetFieldDefnRef(i)->GetNameRef());
    }
    return fields;
}

void
NIImporter_ArcView::addParams(NBEdge* edge, OGRFeature* poFeature, const std::vector<std::string>& params) const {
    for (const std::string& p : params) {
        int index = poFeature->GetDefnRef()->GetFieldIndex(p.c_str());
        if (index >= 0 && poFeature->IsFieldSet(index)) {
            edge->setParameter(p, poFeature->GetFieldAsString(index));
        }
    }
}

#endif


/****************************************************************************/
