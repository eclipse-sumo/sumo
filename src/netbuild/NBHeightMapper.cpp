/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2011-2023 German Aerospace Center (DLR) and others.
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
/// @file    NBHeightMapper.cpp
/// @author  Jakob Erdmann
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @date    Sept 2011
///
// Set z-values for all network positions based on data from a height map
/****************************************************************************/
#include <config.h>

#include <string>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/common/StringUtils.h>
#include <utils/options/OptionsCont.h>
#include <utils/geom/GeomHelper.h>
#include "NBHeightMapper.h"
#include <utils/geom/GeoConvHelper.h>
#include <utils/common/RGBColor.h>

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
#include <ogr_api.h>
#include <gdal_priv.h>
#if __GNUC__ > 3
#pragma GCC diagnostic pop
#endif
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif

// ===========================================================================
// static members
// ===========================================================================
NBHeightMapper NBHeightMapper::myInstance;


// ===========================================================================
// method definitions
// ===========================================================================
NBHeightMapper::NBHeightMapper():
    myRTree(&Triangle::addSelf) {
}


NBHeightMapper::~NBHeightMapper() {
    clearData();
}


const NBHeightMapper&
NBHeightMapper::get() {
    return myInstance;
}


bool
NBHeightMapper::ready() const {
    return myRasters.size() > 0 || myTriangles.size() > 0;
}


double
NBHeightMapper::getZ(const Position& geo) const {
    if (!ready()) {
        WRITE_WARNING(TL("Cannot supply height since no height data was loaded"));
        return 0;
    }
    for (auto& item : myRasters) {
        const Boundary& boundary = item.boundary;
        int16_t* raster = item.raster;
        double result = -1e6;
        if (boundary.around(geo)) {
            const int xSize = item.xSize;
            const double normX = (geo.x() - boundary.xmin()) / mySizeOfPixel.x();
            const double normY = (geo.y() - boundary.ymax()) / mySizeOfPixel.y();
            PositionVector corners;
            corners.push_back(Position(floor(normX) + 0.5, floor(normY) + 0.5, raster[(int)normY * xSize + (int)normX]));
            if (normX - floor(normX) > 0.5) {
                corners.push_back(Position(floor(normX) + 1.5, floor(normY) + 0.5, raster[(int)normY * xSize + (int)normX + 1]));
            } else {
                corners.push_back(Position(floor(normX) - 0.5, floor(normY) + 0.5, raster[(int)normY * xSize + (int)normX - 1]));
            }
            if (normY - floor(normY) > 0.5 && ((int)normY + 1) < item.ySize) {
                corners.push_back(Position(floor(normX) + 0.5, floor(normY) + 1.5, raster[((int)normY + 1) * xSize + (int)normX]));
            } else {
                corners.push_back(Position(floor(normX) + 0.5, floor(normY) - 0.5, raster[((int)normY - 1) * xSize + (int)normX]));
            }
            result = Triangle(corners).getZ(Position(normX, normY));
        }
        if (result > -1e5 && result < 1e5) {
            return result;
        }
    }
    // coordinates in degrees hence a small search window
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
    WRITE_WARNINGF(TL("Could not get height data for coordinate %"), toString(geo));
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
    if (oc.isSet("heightmap.geotiff")) {
        // parse file(s)
        std::vector<std::string> files = oc.getStringVector("heightmap.geotiff");
        for (std::vector<std::string>::const_iterator file = files.begin(); file != files.end(); ++file) {
            PROGRESS_BEGIN_MESSAGE("Parsing from GeoTIFF '" + *file + "'");
            int numFeatures = myInstance.loadTiff(*file);
            MsgHandler::getMessageInstance()->endProcessMsg(
                " done (parsed " + toString(numFeatures) +
                " features, Boundary: " + toString(myInstance.getBoundary()) + ").");
        }
    }
    if (oc.isSet("heightmap.shapefiles")) {
        // parse file(s)
        std::vector<std::string> files = oc.getStringVector("heightmap.shapefiles");
        for (std::vector<std::string>::const_iterator file = files.begin(); file != files.end(); ++file) {
            PROGRESS_BEGIN_MESSAGE("Parsing from shape-file '" + *file + "'");
            int numFeatures = myInstance.loadShapeFile(*file);
            MsgHandler::getMessageInstance()->endProcessMsg(
                " done (parsed " + toString(numFeatures) +
                " features, Boundary: " + toString(myInstance.getBoundary()) + ").");
        }
    }
}


int
NBHeightMapper::loadShapeFile(const std::string& file) {
#ifdef HAVE_GDAL
#if GDAL_VERSION_MAJOR < 2
    OGRRegisterAll();
    OGRDataSource* ds = OGRSFDriverRegistrar::Open(file.c_str(), FALSE);
#else
    GDALAllRegister();
    GDALDataset* ds = (GDALDataset*)GDALOpenEx(file.c_str(), GDAL_OF_VECTOR | GA_ReadOnly, nullptr, nullptr, nullptr);
#endif
    if (ds == nullptr) {
        throw ProcessError(TLF("Could not open shape file '%'.", file));
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
    if (toWGS84 == nullptr) {
        WRITE_WARNING(TL("Could not create geocoordinates converter; check whether proj.4 is installed."));
    }

    int numFeatures = 0;
    OGRFeature* feature;
    layer->ResetReading();
    while ((feature = layer->GetNextFeature()) != nullptr) {
        OGRGeometry* geom = feature->GetGeometryRef();
        assert(geom != 0);

        OGRwkbGeometryType gtype = geom->getGeometryType();
        if (gtype == wkbPolygon) {
            assert(std::string(geom->getGeometryName()) == std::string("POLYGON"));
            // try transform to wgs84
            geom->transform(toWGS84);
            OGRLinearRing* cgeom = ((OGRPolygon*) geom)->getExteriorRing();
            // assume TIN with with 4 points and point0 == point3
            assert(cgeom->getNumPoints() == 4);
            PositionVector corners;
            for (int j = 0; j < 3; j++) {
                Position pos((double) cgeom->getX(j), (double) cgeom->getY(j), (double) cgeom->getZ(j));
                corners.push_back(pos);
                myBoundary.add(pos);
            }
            addTriangle(corners);
            numFeatures++;
        } else {
            WRITE_WARNINGF(TL("Ignored heightmap feature type %"), geom->getGeometryName());
        }

        /*
        switch (gtype) {
            case wkbPolygon: {
                break;
            }
            case wkbPoint: {
                WRITE_WARNING(TL("got wkbPoint"));
                break;
            }
            case wkbLineString: {
                WRITE_WARNING(TL("got wkbLineString"));
                break;
            }
            case wkbMultiPoint: {
                WRITE_WARNING(TL("got wkbMultiPoint"));
                break;
            }
            case wkbMultiLineString: {
                WRITE_WARNING(TL("got wkbMultiLineString"));
                break;
            }
            case wkbMultiPolygon: {
                WRITE_WARNING(TL("got wkbMultiPolygon"));
                break;
            }
            default:
                WRITE_WARNING(TL("Unsupported shape type occurred"));
            break;
        }
        */
        OGRFeature::DestroyFeature(feature);
    }
#if GDAL_VERSION_MAJOR < 2
    OGRDataSource::DestroyDataSource(ds);
#else
    GDALClose(ds);
#endif
    OCTDestroyCoordinateTransformation(reinterpret_cast<OGRCoordinateTransformationH>(toWGS84));
    OGRCleanupAll();
    return numFeatures;
#else
    UNUSED_PARAMETER(file);
    WRITE_ERROR(TL("Cannot load shape file since SUMO was compiled without GDAL support."));
    return 0;
#endif
}


int
NBHeightMapper::loadTiff(const std::string& file) {
#ifdef HAVE_GDAL
    GDALAllRegister();
    GDALDataset* poDataset = (GDALDataset*)GDALOpen(file.c_str(), GA_ReadOnly);
    if (poDataset == 0) {
        WRITE_ERROR(TL("Cannot load GeoTIFF file."));
        return 0;
    }
    Boundary boundary;
    const int xSize = poDataset->GetRasterXSize();
    const int ySize = poDataset->GetRasterYSize();
    double adfGeoTransform[6];
    if (poDataset->GetGeoTransform(adfGeoTransform) == CE_None) {
        Position topLeft(adfGeoTransform[0], adfGeoTransform[3]);
        mySizeOfPixel.set(adfGeoTransform[1], adfGeoTransform[5]);
        const double horizontalSize = xSize * mySizeOfPixel.x();
        const double verticalSize = ySize * mySizeOfPixel.y();
        boundary.add(topLeft);
        boundary.add(topLeft.x() + horizontalSize, topLeft.y() + verticalSize);
    } else {
        WRITE_ERRORF(TL("Could not parse geo information from %."), file);
        return 0;
    }
    const int picSize = xSize * ySize;
    int16_t* raster = (int16_t*)CPLMalloc(sizeof(int16_t) * picSize);
    bool ok = true;
    for (int i = 1; i <= poDataset->GetRasterCount(); i++) {
        GDALRasterBand* poBand = poDataset->GetRasterBand(i);
        if (poBand->GetColorInterpretation() != GCI_GrayIndex) {
            WRITE_ERRORF(TL("Unknown color band in %."), file);
            clearData();
            ok = false;
            break;
        }
        assert(xSize == poBand->GetXSize() && ySize == poBand->GetYSize());
        if (poBand->RasterIO(GF_Read, 0, 0, xSize, ySize, raster, xSize, ySize, GDT_Int16, 0, 0) == CE_Failure) {
            WRITE_ERRORF(TL("Failure in reading %."), file);
            clearData();
            ok = false;
            break;
        }
    }
    double min = std::numeric_limits<double>::max();
    double max = -std::numeric_limits<double>::max();
    for (int i = 0; i < picSize; i++) {
        min = MIN2(min, (double)raster[i]);
        max = MAX2(max, (double)raster[i]);
    }
    GDALClose(poDataset);
    if (ok) {
        WRITE_MESSAGE("Read geotiff heightmap with size " + toString(xSize) + "," + toString(ySize)
                      + " for geo boundary [" + toString(boundary)
                      + "] with elevation range [" + toString(min) + "," + toString(max) + "].");
        RasterData rasterData;
        rasterData.raster = raster;
        rasterData.boundary = boundary;
        rasterData.xSize = xSize;
        rasterData.ySize = ySize;
        myRasters.push_back(rasterData);
        return picSize;
    } else {
        return 0;
    }
#else
    UNUSED_PARAMETER(file);
    WRITE_ERROR(TL("Cannot load GeoTIFF file since SUMO was compiled without GDAL support."));
    return 0;
#endif
}


void
NBHeightMapper::clearData() {
    for (Triangles::iterator it = myTriangles.begin(); it != myTriangles.end(); it++) {
        delete *it;
    }
    myTriangles.clear();
#ifdef HAVE_GDAL
    for (auto& item : myRasters) {
        CPLFree(item.raster);
    }
    myRasters.clear();
#endif
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


double
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
