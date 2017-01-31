/****************************************************************************/
/// @file    PCPolyContainer.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Melanie Knocke
/// @date    Mon, 05 Dec 2005
/// @version $Id$
///
// A storage for loaded polygons and pois
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2005-2017 DLR (http://www.dlr.de/) and contributors
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
#include <algorithm>
#include <map>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/StringUtils.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/shapes/Polygon.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/xml/SUMOSAXAttributes.h>
#include <utils/options/OptionsCont.h>
#include "PCPolyContainer.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
PCPolyContainer::PCPolyContainer(bool prune,
                                 const Boundary& pruningBoundary,
                                 const std::vector<std::string>& removeByNames)
    : myPruningBoundary(pruningBoundary), myDoPrune(prune),
      myRemoveByNames(removeByNames) {}


PCPolyContainer::~PCPolyContainer() {
    myPolygons.clear();
    myPOIs.clear();
}


bool
PCPolyContainer::add(SUMO::Polygon* poly, bool ignorePruning) {
    // check whether the polygon lies within the wished area
    //  - if such an area was given
    if (myDoPrune && !ignorePruning) {
        Boundary b = poly->getShape().getBoxBoundary();
        if (!b.partialWithin(myPruningBoundary)) {
            delete poly;
            return false;
        }
    }
    // check whether the polygon was named to be a removed one
    if (find(myRemoveByNames.begin(), myRemoveByNames.end(), poly->getID()) != myRemoveByNames.end()) {
        delete poly;
        return false;
    }
    return ShapeContainer::add(poly);
}


bool
PCPolyContainer::add(PointOfInterest* poi, bool ignorePruning) {
    // check whether the poi lies within the wished area
    //  - if such an area was given
    if (myDoPrune && !ignorePruning) {
        if (!myPruningBoundary.around(*poi)) {
            delete poi;
            return false;
        }
    }
    // check whether the polygon was named to be a removed one
    if (find(myRemoveByNames.begin(), myRemoveByNames.end(), poi->getID()) != myRemoveByNames.end()) {
        delete poi;
        return false;
    }
    return ShapeContainer::add(poi);
}


void
PCPolyContainer::addLanePos(const std::string& poiID, const std::string& laneID, SUMOReal lanePos) {
    myLanePosPois[poiID] = std::make_pair(laneID, lanePos);
}


void
PCPolyContainer::save(const std::string& file, bool useGeo) {
    const GeoConvHelper& gch = GeoConvHelper::getFinal();
    if (useGeo && !gch.usingGeoProjection()) {
        WRITE_WARNING("Ignoring option \"proj.plain-geo\" because no geo-conversion has been defined");
        useGeo = false;
    }
    OutputDevice& out = OutputDevice::getDevice(file);
    out.writeXMLHeader("additional", "additional_file.xsd");
    if (useGeo) {
        out.setPrecision(gPrecisionGeo);
    } else if (gch.usingGeoProjection()) {
        GeoConvHelper::writeLocation(out);
    }
    // write polygons
    for (std::map<std::string, SUMO::Polygon*>::const_iterator i = myPolygons.getMyMap().begin(); i != myPolygons.getMyMap().end(); ++i) {
        i->second->writeXML(out, useGeo);
    }
    // write pois
    const SUMOReal zOffset = OptionsCont::getOptions().getFloat("poi-layer-offset");
    for (std::map<std::string, PointOfInterest*>::const_iterator i = myPOIs.getMyMap().begin(); i != myPOIs.getMyMap().end(); ++i) {
        std::map<std::string, std::pair<std::string, SUMOReal> >::const_iterator it = myLanePosPois.find(i->first);
        if (it == myLanePosPois.end()) {
            i->second->writeXML(out, useGeo, zOffset);
        } else {
            i->second->writeXML(out, useGeo, zOffset, it->second.first, it->second.second);
        }
    }
    out.close();
}


void PCPolyContainer::writeDlrTDPHeader(OutputDevice& device, const OptionsCont& oc) {
    // XXX duplicate of NWWriter_DlrNavteq::writeHeader()
    const std::string OUTPUT_VERSION = "6.5"; // XXX duplicate of NWWriter_DlrNavteq OUTPUT_VERSION
    time_t rawtime;
    time(&rawtime);
    char buffer [80];
    strftime(buffer, 80, "on %c", localtime(&rawtime));
    device << "# Generated " << buffer << " by " << oc.getFullName() << "\n";
    device << "# Format matches Extraction version: V" << OUTPUT_VERSION << " \n";
    std::stringstream tmp;
    oc.writeConfiguration(tmp, true, false, false);
    tmp.seekg(std::ios_base::beg);
    std::string line;
    while (!tmp.eof()) {
        std::getline(tmp, line);
        device << "# " << line << "\n";
    }
    device << "#\n";
}


void
PCPolyContainer::saveDlrTDP(const std::string& prefix) {
    const OptionsCont& oc = OptionsCont::getOptions();
    const GeoConvHelper& gch = GeoConvHelper::getFinal();
    const bool haveGeo = gch.usingGeoProjection();
    const SUMOReal geoScale = pow(10.0f, haveGeo ? 5 : 2); // see NIImporter_DlrNavteq::GEO_SCALE
    // write pois
    OutputDevice& out = OutputDevice::getDevice(prefix + "_points_of_interest.txt");
    out.setPrecision(0);
    writeDlrTDPHeader(out, oc);
    // write format specifier
    out << "# ID\tCITY\tTYPE\tNAME\tgeo_x\tgeo_y\n";
    int id = 0;
    for (std::map<std::string, PointOfInterest*>::const_iterator i = myPOIs.getMyMap().begin(); i != myPOIs.getMyMap().end(); ++i) {
        Position pos(*(i->second));
        gch.cartesian2geo(pos);
        pos.mul(geoScale);
        out << id << "\t";
        out << "" << "\t";
        out << i->second->getType() << "\t";
        out << i->first << "\t";
        out << pos.x() << "\t";
        out << pos.y() << "\t";
        id++;
    }
    out.close();
    // write polygons
    OutputDevice& out2 = OutputDevice::getDevice(prefix + "_polygons.txt");
    out2.setPrecision(0);
    writeDlrTDPHeader(out2, oc);
    // write format specifier
    out2 << "# ID\tCITY\tTYPE\tNAME\tgeo_x1\tgeo_y1\t[geo_x2 geo_y2 ...]\n";
    id = 0;
    for (std::map<std::string, SUMO::Polygon*>::const_iterator i = myPolygons.getMyMap().begin(); i != myPolygons.getMyMap().end(); ++i) {
        out2 << id << "\t";
        out2 << "" << "\t";
        out2 << i->second->getType() << "\t";
        out2 << i->first << "\t";

        PositionVector shape(i->second->getShape());
        for (int i = 0; i < (int) shape.size(); i++) {
            Position pos = shape[i];
            gch.cartesian2geo(pos);
            pos.mul(geoScale);
            out2 << pos.x() << "\t";
            out2 << pos.y() << "\t";
        }
        id++;
    }
    out2.close();
}


int
PCPolyContainer::getEnumIDFor(const std::string& key) {
    return myIDEnums[key]++;
}



/****************************************************************************/

