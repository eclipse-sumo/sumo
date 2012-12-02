/****************************************************************************/
/// @file    MSFCDExport.cpp
/// @author  Mario Krumnow
///
// Realises dumping Floating Car Data (FCD) Data
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
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

#include <utils/iodevices/OutputDevice.h>
#include <utils/options/OptionsCont.h>
#include <utils/geom/GeoConvHelper.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSGlobals.h>
#include "MSFCDExport.h"
#include <microsim/MSNet.h>
#include <microsim/MSVehicle.h>

#ifdef HAVE_MESOSIM
#include <mesosim/MELoop.h>
#include <mesosim/MESegment.h>
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
void
MSFCDExport::write(OutputDevice& of, SUMOTime timestep) {
    const bool useGeo = OptionsCont::getOptions().getBool("fcd-output.geo");
    MSVehicleControl& vc = MSNet::getInstance()->getVehicleControl();
    MSVehicleControl::constVehIt it = vc.loadedVehBegin();
    MSVehicleControl::constVehIt end = vc.loadedVehEnd();

    of.openTag("timestep") << " time=\"" << time2string(timestep) << "\">\n";
    for (; it != end; ++it) {
        const MSVehicle* veh = static_cast<const MSVehicle*>((*it).second);
        if (veh->isOnRoad()) {
            std::string fclass = veh->getVehicleType().getID();
            fclass = fclass.substr(0, fclass.find_first_of("@"));
            Position pos = veh->getLane()->getShape().positionAtLengthPosition(
                               veh->getLane()->interpolateLanePosToGeometryPos(veh->getPositionOnLane()));
            if (useGeo) {
                of.setPrecision(GEO_OUTPUT_ACCURACY);
                GeoConvHelper::getFinal().cartesian2geo(pos);
            }
            of.openTag("vehicle");
            of.writeAttr(SUMO_ATTR_ID, veh->getID());
            of.writeAttr(SUMO_ATTR_X, pos.x());
            of.writeAttr(SUMO_ATTR_Y, pos.y());
            of.writeAttr(SUMO_ATTR_ANGLE, veh->getAngle());
            of.writeAttr(SUMO_ATTR_TYPE, fclass);
            of.writeAttr(SUMO_ATTR_SPEED, veh->getSpeed());
            of.closeTag(true);
        }
    }
    of.closeTag();
}

/****************************************************************************/
