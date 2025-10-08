/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2012-2025 German Aerospace Center (DLR) and others.
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
/// @file    MSVTKExport.cpp
/// @author  Mario Krumnow
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    2012-04-26
///
// Realises VTK Export
/****************************************************************************/
#include <config.h>

#include <utils/iodevices/OutputDevice.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSJunctionControl.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSGlobals.h>
#include <microsim/traffic_lights/MSTLLogicControl.h>
#include <mesosim/MESegment.h>
#include <mesosim/MELoop.h>
#include <mesosim/MEVehicle.h>
#include "MSVTKExport.h"


// ===========================================================================
// method definitions
// ===========================================================================

// Helper function to get interpolated position for mesoscopic vehicles
static Position
getInterpolatedMesoPosition(const MEVehicle* mesoVeh) {
    const MESegment* segment = mesoVeh->getSegment();
    if (segment == nullptr || mesoVeh->getQueIndex() == MESegment::PARKING_QUEUE) {
        return mesoVeh->getPosition();
    }

    const double now = SIMTIME;
    const double intendedLeave = MIN2(mesoVeh->getEventTimeSeconds(), mesoVeh->getBlockTimeSeconds());
    const double entry = mesoVeh->getLastEntryTimeSeconds();

    // Calculate segment offset (position of segment start)
    double segmentOffset = 0;
    for (MESegment* seg = MSGlobals::gMesoNet->getSegmentForEdge(*mesoVeh->getEdge());
            seg != nullptr && seg != segment; seg = seg->getNextSegment()) {
        segmentOffset += seg->getLength();
    }

    // Calculate interpolated position within segment
    const double length = segment->getLength();
    const double relPos = segmentOffset + length * (now - entry) / (intendedLeave - entry);

    // Convert to cartesian coordinates
    const MSLane* const lane = mesoVeh->getEdge()->getLanes()[0];
    return lane->geometryPositionAtOffset(relPos);
}
void
MSVTKExport::write(OutputDevice& of, SUMOTime /* timestep */) {

    std::vector<double> speed = getSpeed();
    std::vector<double> points = getPositions();

    of << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";
    of << "<VTKFile type=\"PolyData\" version=\"0.1\" order=\"LittleEndian\">\n";
    of << "<PolyData>\n";
    of << " <Piece NumberOfPoints=\"" << speed.size() << "\" NumberOfVerts=\"1\" NumberOfLines=\"0\" NumberOfStrips=\"0\" NumberOfPolys=\"0\">\n";
    of << "<PointData>\n";
    of << " <DataArray type=\"Float64\" Name=\"speed\" format=\"ascii\">" << List2String(getSpeed()) << "</DataArray>\n";
    of << "</PointData>\n";
    of << "<CellData/>\n";
    of << "<Points>\n";
    of << " <DataArray type=\"Float64\" Name=\"Points\" NumberOfComponents=\"3\" format=\"ascii\">" << List2String(getPositions()) << "</DataArray>\n";
    of << "</Points>\n";
    of << "<Verts>\n";
    of << " <DataArray type=\"Int64\" Name=\"connectivity\" format=\"ascii\">" <<  getOffset((int) speed.size()) << "</DataArray>\n";
    of << " <DataArray type=\"Int64\" Name=\"offsets\" format=\"ascii\">" << speed.size() << "</DataArray>\n";
    of << "</Verts>\n";
    of << "<Lines>\n";
    of << " <DataArray type=\"Int64\" Name=\"connectivity\" format=\"ascii\"/>\n";
    of << " <DataArray type=\"Int64\" Name=\"offsets\" format=\"ascii\"/>\n";
    of << "</Lines>\n";
    of << "<Stripes>\n";
    of << " <DataArray type=\"Int64\" Name=\"connectivity\" format=\"ascii\"/>\n";
    of << " <DataArray type=\"Int64\" Name=\"offsets\" format=\"ascii\"/>\n";
    of << "</Stripes>\n";
    of << "<Polys>\n";
    of << " <DataArray type=\"Int64\" Name=\"connectivity\" format=\"ascii\"/>\n";
    of << " <DataArray type=\"Int64\" Name=\"offsets\" format=\"ascii\"/>\n";
    of << "</Polys>\n";
    of << "</Piece>\n";
    of << "</PolyData>\n";
    of << "</VTKFile>";

}

std::vector<double>
MSVTKExport::getSpeed() {

    std::vector<double> output;

    MSVehicleControl& vc = MSNet::getInstance()->getVehicleControl();
    MSVehicleControl::constVehIt it = vc.loadedVehBegin();
    MSVehicleControl::constVehIt end = vc.loadedVehEnd();


    for (; it != end; ++it) {
        const MSVehicle* veh = static_cast<const MSVehicle*>((*it).second);

        if (veh->isOnRoad()) {

            //Position pos = veh->getLane()->getShape().positionAtOffset(veh->getPositionOnLane());
            output.push_back(veh->getSpeed());
        }

    }

    return output;
}

std::vector<double>
MSVTKExport::getPositions() {

    std::vector<double> output;

    MSVehicleControl& vc = MSNet::getInstance()->getVehicleControl();
    MSVehicleControl::constVehIt it = vc.loadedVehBegin();
    MSVehicleControl::constVehIt end = vc.loadedVehEnd();


    for (; it != end; ++it) {
        const SUMOVehicle* veh = (*it).second;

        if (veh->isOnRoad()) {
            Position pos;
            // Use interpolated position for meso vehicles
            if (MSGlobals::gUseMesoSim) {
                const MEVehicle* mesoVeh = static_cast<const MEVehicle*>(veh);
                pos = getInterpolatedMesoPosition(mesoVeh);
            } else {
                pos = veh->getPosition();
            }

            output.push_back(pos.x());
            output.push_back(pos.y());
            output.push_back(pos.z());

        }

    }

    return output;
}

std::string
MSVTKExport::List2String(std::vector<double> input) {

    std::string output = "";
    for (int i = 0; i < (int)input.size(); i++) {

        std::stringstream ss;

        //for a high precision
        //ss.precision(::std::numeric_limits<double>::digits10);
        //ss.unsetf(::std::ios::dec);
        //ss.setf(::std::ios::scientific);

        ss << input[i] << " ";
        output += ss.str();
    }

    return trim(output);
}

std::string
MSVTKExport::getOffset(int nr) {

    std::string output = "";
    for (int i = 0; i < nr; i++) {

        std::stringstream ss;
        ss << i << " ";
        output += ss.str();
    }

    return trim(output);
}

bool
MSVTKExport::ctype_space(const char c) {
    if (c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == 11) {
        return true;
    }
    return false;
}

std::string
MSVTKExport::trim(std::string istring) {
    bool trimmed = false;

    if (ctype_space(istring[istring.length() - 1])) {
        istring.erase(istring.length() - 1);
        trimmed = true;
    }

    if (ctype_space(istring[0])) {
        istring.erase(0, 1);
        trimmed = true;
    }

    if (!trimmed) {
        return istring;
    } else {
        return trim(istring);
    }

}


/****************************************************************************/
