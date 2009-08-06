/****************************************************************************/
/// @file    PCLoaderArcView.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A reader of pois and polygons from shape files
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/common/StringUtils.h>
#include <utils/options/OptionsCont.h>
#include <utils/geom/GeomHelper.h>
#include "PCLoaderArcView.h"
#include <utils/geom/GeoConvHelper.h>
#include <utils/common/RGBColor.h>
#include <polyconvert/PCPolyContainer.h>

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
void
PCLoaderArcView::loadIfSet(OptionsCont &oc, PCPolyContainer &toFill,
                           PCTypeMap &tm) throw(ProcessError) {
    if (!oc.isSet("shape-files")) {
        return;
    }
    // parse file(s)
    vector<string> files = oc.getStringVector("shape-files");
    for (vector<string>::const_iterator file=files.begin(); file!=files.end(); ++file) {
        MsgHandler::getMessageInstance()->beginProcessMsg("Parsing from shape-file '" + *file + "'...");
        load(*file, oc, toFill, tm);
        MsgHandler::getMessageInstance()->endProcessMsg("done.");
    }
}



void
PCLoaderArcView::load(const string &file, OptionsCont &oc, PCPolyContainer &toFill,
                      PCTypeMap &) throw(ProcessError) {
#ifdef HAVE_GDAL
    // get defaults
    string prefix = oc.getString("prefix");
    string type = oc.getString("type");
    RGBColor color = RGBColor::parseColor(oc.getString("color"));
    int layer = oc.getInt("layer");
    string idField = oc.getString("shape-file.id-name");
    // start parsing
    string shpName = file + ".shp";
    OGRRegisterAll();
    OGRDataSource *poDS = OGRSFDriverRegistrar::Open(shpName.c_str(), FALSE);
    if (poDS == NULL) {
        throw ProcessError("Could not open shape description '" + shpName + "'.");
    }

    // begin file parsing
    OGRLayer  *poLayer = poDS->GetLayer(0);
    poLayer->ResetReading();

    // build coordinate transformation
    OGRSpatialReference *origTransf = poLayer->GetSpatialRef();
    OGRSpatialReference destTransf;
    // use wgs84 as destination
    destTransf.SetWellKnownGeogCS("WGS84");
    OGRCoordinateTransformation *poCT = OGRCreateCoordinateTransformation(origTransf, &destTransf);
    if (poCT == NULL) {
        if (oc.isSet("arcview.guess-projection")) {
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
        string id = poFeature->GetFieldAsString(idField.c_str());
        id = StringUtils::prune(id);
        if (id=="") {
            throw ProcessError("Missing id under '" + idField + "'");
        }
        id = prefix + id;
        // read in the geometry
        OGRGeometry *poGeometry = poFeature->GetGeometryRef();
        if (poGeometry!=0) {
            // try transform to wgs84
            poGeometry->transform(poCT);
        }
        OGRwkbGeometryType gtype = poGeometry->getGeometryType();
        switch (gtype) {
        case wkbPoint: {
            OGRPoint *cgeom = (OGRPoint*) poGeometry;
            Position2D pos((SUMOReal) cgeom->getX(), (SUMOReal) cgeom->getY());
            if (!GeoConvHelper::x2cartesian(pos)) {
                MsgHandler::getErrorInstance()->inform("Unable to project coordinates for POI '" + id + "'.");
            }
            PointOfInterest *poi = new PointOfInterest(id, type, pos, color);
            if (!toFill.insert(id, poi, layer)) {
                MsgHandler::getErrorInstance()->inform("POI '" + id + "' could not been added.");
                delete poi;
            }
        }
        break;
        case wkbLineString: {
            OGRLineString *cgeom = (OGRLineString*) poGeometry;
            Position2DVector shape;
            for (int j=0; j<cgeom->getNumPoints(); j++) {
                Position2D pos((SUMOReal) cgeom->getX(j), (SUMOReal) cgeom->getY(j));
                if (!GeoConvHelper::x2cartesian(pos)) {
                    MsgHandler::getErrorInstance()->inform("Unable to project coordinates for polygon '" + id + "'.");
                }
                shape.push_back_noDoublePos(pos);
            }
            Polygon2D *poly = new Polygon2D(id, type, color, shape, false);
            if (!toFill.insert(id, poly, layer)) {
                MsgHandler::getErrorInstance()->inform("Polygon '" + id + "' could not been added.");
                delete poly;
            }
        }
        break;
        case wkbPolygon: {
            OGRLinearRing *cgeom = ((OGRPolygon*) poGeometry)->getExteriorRing();
            Position2DVector shape;
            for (int j=0; j<cgeom->getNumPoints(); j++) {
                Position2D pos((SUMOReal) cgeom->getX(j), (SUMOReal) cgeom->getY(j));
                if (!GeoConvHelper::x2cartesian(pos)) {
                    MsgHandler::getErrorInstance()->inform("Unable to project coordinates for polygon '" + id + "'.");
                }
                shape.push_back_noDoublePos(pos);
            }
            Polygon2D *poly = new Polygon2D(id, type, color, shape, true);
            if (!toFill.insert(id, poly, layer)) {
                MsgHandler::getErrorInstance()->inform("Polygon '" + id + "' could not been added.");
                delete poly;
            }
        }
        break;
        case wkbMultiPoint: {
            OGRMultiPoint *cgeom = (OGRMultiPoint*) poGeometry;
            for (int i=0; i<cgeom->getNumGeometries(); ++i) {
                OGRPoint *cgeom2 = (OGRPoint*) cgeom->getGeometryRef(i);
                Position2D pos((SUMOReal) cgeom2->getX(), (SUMOReal) cgeom2->getY());
                string tid = id + "#" + toString(i);
                if (!GeoConvHelper::x2cartesian(pos)) {
                    MsgHandler::getErrorInstance()->inform("Unable to project coordinates for POI '" + tid + "'.");
                }
                PointOfInterest *poi = new PointOfInterest(tid, type, pos, color);
                if (!toFill.insert(tid, poi, layer)) {
                    MsgHandler::getErrorInstance()->inform("POI '" + tid + "' could not been added.");
                    delete poi;
                }
            }
        }
        break;
        case wkbMultiLineString: {
            OGRMultiLineString *cgeom = (OGRMultiLineString*) poGeometry;
            for (int i=0; i<cgeom->getNumGeometries(); ++i) {
                OGRLineString *cgeom2 = (OGRLineString*) cgeom->getGeometryRef(i);
                Position2DVector shape;
                string tid = id + "#" + toString(i);
                for (int j=0; j<cgeom2->getNumPoints(); j++) {
                    Position2D pos((SUMOReal) cgeom2->getX(j), (SUMOReal) cgeom2->getY(j));
                    if (!GeoConvHelper::x2cartesian(pos)) {
                        MsgHandler::getErrorInstance()->inform("Unable to project coordinates for polygon '" + tid + "'.");
                    }
                    shape.push_back_noDoublePos(pos);
                }
                Polygon2D *poly = new Polygon2D(tid, type, color, shape, false);
                if (!toFill.insert(tid, poly, layer)) {
                    MsgHandler::getErrorInstance()->inform("Polygon '" + tid + "' could not been added.");
                    delete poly;
                }
            }
        }
        break;
        case wkbMultiPolygon: {
            OGRMultiPolygon *cgeom = (OGRMultiPolygon*) poGeometry;
            for (int i=0; i<cgeom->getNumGeometries(); ++i) {
                OGRLinearRing *cgeom2 = ((OGRPolygon*) cgeom->getGeometryRef(i))->getExteriorRing();
                Position2DVector shape;
                string tid = id + "#" + toString(i);
                for (int j=0; j<cgeom2->getNumPoints(); j++) {
                    Position2D pos((SUMOReal) cgeom2->getX(j), (SUMOReal) cgeom2->getY(j));
                    if (!GeoConvHelper::x2cartesian(pos)) {
                        MsgHandler::getErrorInstance()->inform("Unable to project coordinates for polygon '" + tid + "'.");
                    }
                    shape.push_back_noDoublePos(pos);
                }
                Polygon2D *poly = new Polygon2D(tid, type, color, shape, true);
                if (!toFill.insert(tid, poly, layer)) {
                    MsgHandler::getErrorInstance()->inform("Polygon '" + tid + "' could not been added.");
                    delete poly;
                }
            }
        }
        break;
        default:
            MsgHandler::getWarningInstance()->inform("Unsupported shape type occured (id='" + id + "').");
            break;
        }
        OGRFeature::DestroyFeature(poFeature);
    }
    MsgHandler::getMessageInstance()->endProcessMsg("done.");
#else
    MsgHandler::getErrorInstance()->inform("SUMO was compiled without GDAL support.");
#endif
}


/****************************************************************************/

