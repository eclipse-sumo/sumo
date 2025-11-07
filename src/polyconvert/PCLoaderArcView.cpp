/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    PCLoaderArcView.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
///
// A reader of pois and polygons from shape files
/****************************************************************************/
#include <config.h>

#include <string>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/common/StringUtils.h>
#include <utils/options/OptionsCont.h>
#include <utils/geom/GeomHelper.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/common/RGBColor.h>
#include <polyconvert/PCPolyContainer.h>
#include <polyconvert/PCTypeMap.h>
#include "PCLoaderArcView.h"

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
// static member variables
// ===========================================================================
bool PCLoaderArcView::myWarnMissingProjection = true;


// ===========================================================================
// method definitions
// ===========================================================================
void
PCLoaderArcView::loadIfSet(OptionsCont& oc, PCPolyContainer& toFill, PCTypeMap& tm) {
    if (!oc.isSet("shapefile-prefixes") && !oc.isSet("geojson-files")) {
        return;
    }
    // parse file(s)
    for (std::string file : oc.getStringVector("shapefile-prefixes")) {
        file += ".shp";
        PROGRESS_BEGIN_MESSAGE("Parsing from shape-file '" + file + "'");
        load(file, oc, toFill, tm);
        PROGRESS_DONE_MESSAGE();
    }
    for (const std::string& file : oc.getStringVector("geojson-files")) {
        PROGRESS_BEGIN_MESSAGE("Parsing from geojson-file '" + file + "'");
        load(file, oc, toFill, tm);
        PROGRESS_DONE_MESSAGE();
    }
}


#ifdef HAVE_GDAL
const PositionVector
PCLoaderArcView::toShape(OGRLineString* geom, const std::string& tid) {
    if (myWarnMissingProjection) {
        int outOfRange = 0;
        for (int j = 0; j < geom->getNumPoints(); j++) {
            if (fabs(geom->getX(j)) > 180 || fabs(geom->getY(j)) > 90) {
                outOfRange++;
            }
        }
        if (2 * outOfRange > geom->getNumPoints()) {
            WRITE_WARNING(TL("No coordinate system found and coordinates look already projected."));
            GeoConvHelper::init("!", GeoConvHelper::getProcessing().getOffset(), GeoConvHelper::getProcessing().getOrigBoundary(), GeoConvHelper::getProcessing().getConvBoundary());
        } else {
            WRITE_WARNING(TL("Could not find geo coordinate system, assuming WGS84."));
        }
        myWarnMissingProjection = false;
    }
    GeoConvHelper& geoConvHelper = GeoConvHelper::getProcessing();
    PositionVector shape;
#if GDAL_VERSION_MAJOR < 3
    for (int j = 0; j < geom->getNumPoints(); j++) {
        Position pos(geom->getX(j), geom->getY(j));
#else
    for (const OGRPoint& p : *geom) {
        Position pos(p.getX(), p.getY(), p.Is3D() ? p.getZ() : 0.0);
#endif
        if (!geoConvHelper.x2cartesian(pos)) {
            WRITE_ERRORF(TL("Unable to project coordinates for polygon '%'."), tid);
        }
        shape.push_back_noDoublePos(pos);
    }
    return shape;
}
#endif


void
PCLoaderArcView::load(const std::string& file, OptionsCont& oc, PCPolyContainer& toFill, PCTypeMap& tm) {
#ifdef HAVE_GDAL
    GeoConvHelper& geoConvHelper = GeoConvHelper::getProcessing();
    // get defaults
    const std::string idField = oc.getString("shapefile.id-column");
    const bool useRunningID = oc.getBool("shapefile.use-running-id") || idField == "";
    int fillType = -1;
    if (oc.getString("shapefile.fill") == "true") {
        fillType = 1;
    } else if (oc.getString("shapefile.fill") == "false") {
        fillType = 0;
    }
#if GDAL_VERSION_MAJOR < 2
    OGRRegisterAll();
    OGRDataSource* poDS = OGRSFDriverRegistrar::Open(file.c_str(), FALSE);
#else
    GDALAllRegister();
    GDALDataset* poDS = (GDALDataset*) GDALOpenEx(file.c_str(), GDAL_OF_VECTOR | GA_ReadOnly, NULL, NULL, NULL);
#endif
    if (poDS == NULL) {
        throw ProcessError(TLF("Could not open shape description '%'.", file));
    }

    // begin file parsing
    OGRLayer*  poLayer = poDS->GetLayer(0);
    poLayer->ResetReading();

    // build coordinate transformation
    const OGRSpatialReference* origTransf = poLayer->GetSpatialRef();
    OGRSpatialReference destTransf;
    // use wgs84 as destination
    destTransf.SetWellKnownGeogCS("WGS84");
#if GDAL_VERSION_MAJOR > 2
    if (oc.getBool("shapefile.traditional-axis-mapping") || origTransf != nullptr) {
        destTransf.SetAxisMappingStrategy(OAMS_TRADITIONAL_GIS_ORDER);
    }
#endif
    OGRCoordinateTransformation* poCT = origTransf == nullptr ? nullptr : OGRCreateCoordinateTransformation(origTransf, &destTransf);
    if (poCT == nullptr) {
        if (oc.getBool("shapefile.guess-projection")) {
            OGRSpatialReference origTransf2;
            origTransf2.SetWellKnownGeogCS("WGS84");
            poCT = OGRCreateCoordinateTransformation(&origTransf2, &destTransf);
        }
    } else {
        myWarnMissingProjection = false;
    }

    OGRFeature* poFeature;
    poLayer->ResetReading();
    int runningID = 0;
    while ((poFeature = poLayer->GetNextFeature()) != nullptr) {
        if (runningID == 0) {
            std::vector<std::string> fields;
            for (int i = 0; i < poFeature->GetFieldCount(); i++) {
                fields.push_back(poFeature->GetFieldDefnRef(i)->GetNameRef());
            }
            WRITE_MESSAGE("Available fields: " + toString(fields));
        }
        std::vector<Parameterised*> parCont;
        // read in edge attributes
        std::string id = useRunningID ? toString(runningID) : poFeature->GetFieldAsString(idField.c_str());
        ++runningID;
        id = StringUtils::latin1_to_utf8(StringUtils::prune(id));
        if (id == "") {
            throw ProcessError(TLF("Missing id under '%'", idField));
        }
        id = oc.getString("prefix") + id;
        std::string type;
        for (const std::string& typeField : oc.getStringVector("shapefile.type-columns")) {
            if (type != "") {
                type += ".";
            }
            type += poFeature->GetFieldAsString(typeField.c_str());
        }
        RGBColor color = RGBColor::parseColor(oc.getString("color"));
        std::string icon = oc.getString("icon");
        double layer = oc.getFloat("layer");
        double angle = Shape::DEFAULT_ANGLE;
        std::string imgFile = Shape::DEFAULT_IMG_FILE;
        if (type != "") {
            if (tm.has(type)) {
                const PCTypeMap::TypeDef& def = tm.get(type);
                if (def.discard) {
                    continue;
                }
                color = def.color;
                icon = def.icon;
                layer = def.layer;
                angle = def.angle;
                imgFile = def.imgFile;
                type = def.id;
            }
        } else {
            type = oc.getString("type");
        }
        if (poFeature->GetFieldIndex("angle") >= 0) {
            angle = poFeature->GetFieldAsDouble("angle");
        }
        // read in the geometry
        OGRGeometry* poGeometry = poFeature->GetGeometryRef();
        if (poGeometry == 0) {
            OGRFeature::DestroyFeature(poFeature);
            continue;
        }
        // try transform to wgs84
        if (poCT != nullptr) {
            poGeometry->transform(poCT);
        }
        OGRwkbGeometryType gtype = poGeometry->getGeometryType();
        switch (gtype) {
            case wkbPoint:
            case wkbPoint25D: {
                OGRPoint* cgeom = (OGRPoint*) poGeometry;
                Position pos(cgeom->getX(), cgeom->getY());
                if (!geoConvHelper.x2cartesian(pos)) {
                    WRITE_ERRORF(TL("Unable to project coordinates for POI '%'."), id);
                }
                PointOfInterest* poi = new PointOfInterest(id, type, color, pos, false, "", 0, false, 0, icon, layer, angle, imgFile);
                if (toFill.add(poi)) {
                    parCont.push_back(poi);
                }
            }
            break;
            case wkbLineString:
            case wkbLineString25D: {
                const PositionVector shape = toShape((OGRLineString*) poGeometry, id);
                SUMOPolygon* poly = new SUMOPolygon(id, type, color, shape, false, fillType == 1, 1, layer, angle, imgFile);
                if (toFill.add(poly)) {
                    parCont.push_back(poly);
                }
            }
            break;
            case wkbPolygon:
            case wkbPolygon25D: {
                const bool fill = fillType < 0 || fillType == 1;
                const PositionVector shape = toShape(((OGRPolygon*) poGeometry)->getExteriorRing(), id);
                SUMOPolygon* poly = new SUMOPolygon(id, type, color, shape, false, fill, 1, layer, angle, imgFile);
                if (toFill.add(poly)) {
                    parCont.push_back(poly);
                }
            }
            break;
            case wkbMultiPoint:
            case wkbMultiPoint25D: {
                OGRMultiPoint* cgeom = (OGRMultiPoint*) poGeometry;
                for (int i = 0; i < cgeom->getNumGeometries(); ++i) {
                    OGRPoint* cgeom2 = (OGRPoint*) cgeom->getGeometryRef(i);
                    Position pos(cgeom2->getX(), cgeom2->getY());
                    const std::string tid = id + "#" + toString(i);
                    if (!geoConvHelper.x2cartesian(pos)) {
                        WRITE_ERRORF(TL("Unable to project coordinates for POI '%'."), tid);
                    }
                    PointOfInterest* poi = new PointOfInterest(tid, type, color, pos, false, "", 0, false, 0, icon, layer, angle, imgFile);
                    if (toFill.add(poi)) {
                        parCont.push_back(poi);
                    }
                }
            }
            break;
            case wkbMultiLineString:
            case wkbMultiLineString25D: {
                OGRMultiLineString* cgeom = (OGRMultiLineString*) poGeometry;
                for (int i = 0; i < cgeom->getNumGeometries(); ++i) {
                    const std::string tid = id + "#" + toString(i);
                    const PositionVector shape = toShape((OGRLineString*) cgeom->getGeometryRef(i), tid);
                    SUMOPolygon* poly = new SUMOPolygon(tid, type, color, shape, false, fillType == 1, 1, layer, angle, imgFile);
                    if (toFill.add(poly)) {
                        parCont.push_back(poly);
                    }
                }
            }
            break;
            case wkbMultiPolygon:
            case wkbMultiPolygon25D: {
                const bool fill = fillType < 0 || fillType == 1;
                OGRMultiPolygon* cgeom = (OGRMultiPolygon*) poGeometry;
                for (int i = 0; i < cgeom->getNumGeometries(); ++i) {
                    const std::string tid = id + "#" + toString(i);
                    const PositionVector shape = toShape(((OGRPolygon*) cgeom->getGeometryRef(i))->getExteriorRing(), tid);
                    SUMOPolygon* poly = new SUMOPolygon(tid, type, color, shape, false, fill, 1, layer, angle, imgFile);
                    if (toFill.add(poly)) {
                        parCont.push_back(poly);
                    }
                }
            }
            break;
            default:
                WRITE_WARNINGF(TL("Unsupported shape type occurred (id='%')."), id);
                break;
        }
        if (oc.getBool("shapefile.add-param") || oc.getBool("all-attributes")) {
            for (std::vector<Parameterised*>::const_iterator it = parCont.begin(); it != parCont.end(); ++it) {
                OGRFeatureDefn* poFDefn = poLayer->GetLayerDefn();
                for (int iField = 0; iField < poFDefn->GetFieldCount(); iField++) {
                    OGRFieldDefn* poFieldDefn = poFDefn->GetFieldDefn(iField);
                    if (poFieldDefn->GetNameRef() != idField) {
                        (*it)->setParameter(poFieldDefn->GetNameRef(), StringUtils::latin1_to_utf8(poFeature->GetFieldAsString(iField)));
                    }
                }
            }
        }
        OGRFeature::DestroyFeature(poFeature);
    }
#if GDAL_VERSION_MAJOR < 2
    OGRDataSource::DestroyDataSource(poDS);
#else
    GDALClose(poDS);
#endif
    PROGRESS_DONE_MESSAGE();
#else
    UNUSED_PARAMETER(file);
    UNUSED_PARAMETER(oc);
    UNUSED_PARAMETER(toFill);
    UNUSED_PARAMETER(tm);
    WRITE_ERROR(TL("SUMO was compiled without GDAL support."));
#endif
}


/****************************************************************************/
