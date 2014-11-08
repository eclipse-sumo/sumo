/****************************************************************************/
/// @file    NBHeightMapper.cpp
/// @author  Jakob Erdmann
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @date    Sept 2011
/// @version $Id$
///
// Set z-values for all network positions based on data from a height map
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2011-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/common/StringUtils.h>
#include <utils/options/OptionsCont.h>
#include <utils/geom/GeomHelper.h>
#include "NBHeightMapper.h"
#include <utils/geom/GeoConvHelper.h>
#include <utils/common/RGBColor.h>
#include <polyconvert/PCPolyContainer.h>

#ifdef HAVE_GDAL
#include <ogrsf_frmts.h>
#include <ogr_api.h>
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// ===========================================================================
// static members
// ===========================================================================
NBHeightMapper NBHeightMapper::Singleton;

// ===========================================================================
// method definitions
// ===========================================================================


NBHeightMapper::NBHeightMapper():
    myRTree(&Triangle::addSelf)
{ }


NBHeightMapper::~NBHeightMapper() {
    clearData();
}


const NBHeightMapper&
NBHeightMapper::get() {
    return Singleton;
}


bool
NBHeightMapper::ready() const {
    return myTriangles.size() > 0;
}



SUMOReal
NBHeightMapper::getZ(const Position& geo) const {
    if (!ready()) {
        WRITE_WARNING("Cannot supply height since no height data was loaded");
        return 0;
    }
    // coordinates in degress hence a small search window
    float minB[2];
    float maxB[2];
    minB[0] = (float)geo.x() - 0.00001f;
    minB[1] = (float)geo.y() - 0.00001f;
    maxB[0] = (float)geo.x() + 0.00001f;
    maxB[1] = (float)geo.y() + 0.00001f;
    QueryResult queryResult;
    int hits = myRTree.Search(minB, maxB, queryResult);
    Triangles result = queryResult.triangles;
    assert(hits == (int)result.size());
    UNUSED_PARAMETER(hits); // only used for assertion

    for (Triangles::iterator it = result.begin(); it != result.end(); it++) {
        const Triangle* triangle = *it;
        if (triangle->contains(geo)) {
            return triangle->getZ(geo);
        }
    }
    WRITE_WARNING("Could not get height data for coordinate " + toString(geo));
    return 0;
}


void
NBHeightMapper::addTriangle(PositionVector corners) {
    Triangle* triangle = new Triangle(corners);
    myTriangles.push_back(triangle);
    Boundary b = corners.getBoxBoundary();
    const float cmin[2] = {(float) b.xmin(), (float) b.ymin()};
    const float cmax[2] = {(float) b.xmax(), (float) b.ymax()};
    myRTree.Insert(cmin, cmax, triangle);
}


void
NBHeightMapper::loadIfSet(OptionsCont& oc) {
    if (oc.isSet("heightmap.shapefiles")) {
        // parse file(s)
        std::vector<std::string> files = oc.getStringVector("heightmap.shapefiles");
        for (std::vector<std::string>::const_iterator file = files.begin(); file != files.end(); ++file) {
            PROGRESS_BEGIN_MESSAGE("Parsing from shape-file '" + *file + "'");
            int numFeatures = Singleton.loadShapeFile(*file);
            MsgHandler::getMessageInstance()->endProcessMsg(
                " done (parsed " + toString(numFeatures) +
                " features, Boundary: " + toString(Singleton.getBoundary()) + ").");
        }
    }
}


int
NBHeightMapper::loadShapeFile(const std::string& file) {
    int numFeatures = 0;
#ifndef HAVE_GDAL
    WRITE_ERROR("Cannot load shape file since SUMO was compiled without GDAL support.");
    return numFeatures;
#else
    OGRRegisterAll();
    OGRDataSource* ds = OGRSFDriverRegistrar::Open(file.c_str(), FALSE);
    if (ds == NULL) {
        throw ProcessError("Could not open shape file '" + file + "'.");
    }

    // begin file parsing
    OGRLayer* layer = ds->GetLayer(0);
    layer->ResetReading();

    // triangle coordinates are stored in WGS84 and later matched with network coordinates in WGS84
    // build coordinate transformation
    OGRSpatialReference* sr_src = layer->GetSpatialRef();
    OGRSpatialReference sr_dest;
    sr_dest.SetWellKnownGeogCS("WGS84");
    OGRCoordinateTransformation* toWGS84 = OGRCreateCoordinateTransformation(sr_src, &sr_dest);
    if (toWGS84 == 0) {
        WRITE_WARNING("Could not create geocoordinates converter; check whether proj.4 is installed.");
    }

    OGRFeature* feature;
    layer->ResetReading();
    while ((feature = layer->GetNextFeature()) != NULL) {
        OGRGeometry* geom = feature->GetGeometryRef();
        assert(geom != 0);

        // @todo gracefull handling of shapefiles with unexpected contents or any error handling for that matter
        assert(std::string(geom->getGeometryName()) == std::string("POLYGON"));
        // try transform to wgs84
        geom->transform(toWGS84);
        OGRLinearRing* cgeom = ((OGRPolygon*) geom)->getExteriorRing();
        // assume TIN with with 4 points and point0 == point3
        assert(cgeom->getNumPoints() == 4);
        PositionVector corners;
        for (int j = 0; j < 3; j++) {
            Position pos((SUMOReal) cgeom->getX(j), (SUMOReal) cgeom->getY(j), (SUMOReal) cgeom->getZ(j));
            corners.push_back(pos);
            myBoundary.add(pos);
        }
        addTriangle(corners);
        numFeatures++;

        /*
        OGRwkbGeometryType gtype = geom->getGeometryType();
        switch (gtype) {
            case wkbPolygon: {
                break;
            }
            case wkbPoint: {
                WRITE_WARNING("got wkbPoint");
                break;
            }
            case wkbLineString: {
                WRITE_WARNING("got wkbLineString");
                break;
            }
            case wkbMultiPoint: {
                WRITE_WARNING("got wkbMultiPoint");
                break;
            }
            case wkbMultiLineString: {
                WRITE_WARNING("got wkbMultiLineString");
                break;
            }
            case wkbMultiPolygon: {
                WRITE_WARNING("got wkbMultiPolygon");
                break;
            }
            default:
                WRITE_WARNING("Unsupported shape type occured");
            break;
        }
        */
        OGRFeature::DestroyFeature(feature);
    }
    OGRDataSource::DestroyDataSource(ds);
    OCTDestroyCoordinateTransformation(toWGS84);
    OGRCleanupAll();
    return numFeatures;
#endif
}


void
NBHeightMapper::clearData() {
    for (Triangles::iterator it = myTriangles.begin(); it != myTriangles.end(); it++) {
        delete *it;
    }
    myTriangles.clear();
    myBoundary.reset();
}


// ===========================================================================
// Triangle member methods
// ===========================================================================
NBHeightMapper::Triangle::Triangle(const PositionVector& corners):
    myCorners(corners) {
    assert(myCorners.size() == 3);
    // @todo assert non-colinearity
}


void
NBHeightMapper::Triangle::addSelf(const QueryResult& queryResult) const {
    queryResult.triangles.push_back(this);
}


bool
NBHeightMapper::Triangle::contains(const Position& pos) const {
    return myCorners.around(pos);
}


SUMOReal
NBHeightMapper::Triangle::getZ(const Position& geo) const {
    // en.wikipedia.org/wiki/Line-plane_intersection
    Position p0 = myCorners.front();
    Position line(0, 0, 1);
    p0.sub(geo); // p0 - l0
    Position normal = normalVector();
    return p0.dotProduct(normal) / line.dotProduct(normal);
}


Position
NBHeightMapper::Triangle::normalVector() const {
    // @todo maybe cache result to avoid multiple computations?
    Position side1 = myCorners[1] - myCorners[0];
    Position side2 = myCorners[2] - myCorners[0];
    return side1.crossProduct(side2);
}


/****************************************************************************/

