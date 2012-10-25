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

#include <microsim/MSEdgeControl.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSGlobals.h>
#include <utils/iodevices/OutputDevice.h>
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
   
	
	    of.openTag("timestep") << " time=\"" << time2string(timestep) << "\">\n";
		
		MSVehicleControl& vc = MSNet::getInstance()->getVehicleControl();
		MSVehicleControl::constVehIt it = vc.loadedVehBegin();
		MSVehicleControl::constVehIt end = vc.loadedVehEnd();
		
	    for (; it != end; ++it) {
        const MSVehicle* veh = static_cast<const MSVehicle*>((*it).second);

		if (veh->isOnRoad()) {
           
		    std::string fclass = veh->getVehicleType().getID();
			fclass = fclass.substr(0, fclass.find_first_of("@"));
		   
            Position pos = veh->getLane()->getShape().positionAtLengthPosition(
                    veh->getLane()->interpolateLanePosToGeometryPos(veh->getPositionOnLane()));
			of.openTag("vehicle") << " id=\"" << veh->getID() << "\" x=\"" << pos.x() << "\" y=\"" << pos.y() << "\" angle=\"" << veh->getAngle() << "\" type=\"" << fclass << "\" speed=\"" << veh->getSpeed()*3.6 << "\"";
            of.closeTag(true);	
		}

	}
    of.closeTag();
}

/****************************************************************************/
