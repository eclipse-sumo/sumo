/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2017 German Aerospace Center (DLR) and others.
/****************************************************************************/
//
//   This program and the accompanying materials
//   are made available under the terms of the Eclipse Public License v2.0
//   which accompanies this distribution, and is available at
//   http://www.eclipse.org/legal/epl-v20.html
//
/****************************************************************************/
/// @file    TraCI_Simulation.cpp
/// @author  Laura Bieker-Walz
/// @author  Robert Hilbrich
/// @date    15.09.2017
/// @version $Id$
///
// C++ TraCI client API implementation
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifndef NO_TRACI

#include <utils/common/StdDefs.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/StringUtils.h>
#include <utils/geom/GeoConvHelper.h>
#include <microsim/MSNet.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSInsertionControl.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSStateHandler.h>
#include <microsim/MSStoppingPlace.h>
#include <traci-server/lib/TraCI.h>
#include "TraCI_Simulation.h"
#include <traci-server/TraCIDefs.h>

// ===========================================================================
// member definitions
// ===========================================================================
SUMOTime 
TraCI_Simulation::getCurrentTime(){
    return MSNet::getInstance()->getCurrentTimeStep();
}


SUMOTime 
TraCI_Simulation::getDeltaT(){
    return DELTA_T;
}


TraCIBoundary
TraCI_Simulation::getNetBoundary() {
	Boundary b = GeoConvHelper::getFinal().getConvBoundary();
	TraCIBoundary tb;
	tb.xMin = b.xmin();
	tb.xMax = b.xmax();
	tb.yMin = b.ymin();
	tb.yMax = b.ymax();
	tb.zMin = b.zmin();
	tb.zMax = b.zmax();
	return tb;
}

int
TraCI_Simulation::getMinExpectedNumber() {
    return MSNet::getInstance()->getVehicleControl().getActiveVehicleCount() + MSNet::getInstance()->getInsertionControl().getPendingFlowCount();
}


std::string 
TraCI_Simulation::getParameter(const std::string& objectID, const std::string& key) {
    if (StringUtils::startsWith(key, "chargingStation.")) {
        const std::string attrName = key.substr(16);
        MSChargingStation* cs = MSNet::getInstance()->getChargingStation(objectID);
        if (cs == 0) {
            throw TraCIException("Invalid chargingStation '" + objectID + "'");
        }
        if (attrName == toString(SUMO_ATTR_TOTALENERGYCHARGED)) {
            return toString(cs->getTotalCharged());
        } else {
            throw TraCIException("Invalid chargingStation parameter '" + attrName + "'");
        }
    } else {
        throw TraCIException("Parameter '" + key + "' is not supported.");
    }
}


#endif


/****************************************************************************/

