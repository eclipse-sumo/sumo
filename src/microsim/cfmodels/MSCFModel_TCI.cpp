/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSCFModel_TCI.cpp
/// @author  Leonhard Luecken
/// @date    Tue, 5 Feb 2018
/// @version $Id$
///
// Task Capability Interface car-following model. Basically the Krauss model with perception errors taken from the driver state.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <memory>
#include <microsim/MSVehicle.h>
#include <microsim/pedestrians/MSPerson.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSNet.h>
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include "MSCFModel_TCI.h"
#include <microsim/MSDriverState.h>
#include <microsim/lcmodels/MSAbstractLaneChangeModel.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SUMOTime.h>


// ===========================================================================
// DEBUG constants
// ===========================================================================
#define DEBUG_DRIVER_ERRORS
//#define DEBUG_COND (true)
#define DEBUG_COND (veh->isSelected())


// ===========================================================================
// method definitions
// ===========================================================================

MSCFModel_TCI::MSCFModel_TCI(const MSVehicleType* vtype, double accel, double decel,
                                   double emergencyDecel, double apparentDecel,
                                   double headwayTime) :
    MSCFModel_Krauss(vtype, accel, decel, emergencyDecel, apparentDecel, 0., headwayTime)
{}


MSCFModel_TCI::~MSCFModel_TCI() {}


double
MSCFModel_TCI::stopSpeed(const MSVehicle* const veh, const double speed, double gap) const {
    assert(veh->getDriverState()!=nullptr); // DriverState must be defined for vehicle with MSCFModel_TCI
    const double perceivedGap = veh->getDriverState()->getPerceivedHeadway(gap);
#ifdef DEBUG_DRIVER_ERRORS
    if DEBUG_COND {
        std::cout << SIMTIME << " veh '" << veh->getID() << "' -> MSCFModel_TCI::stopSpeed()\n"
                << "  speed=" << speed << " gap=" << gap << "\n  perceivedGap=" << perceivedGap << std::endl;
        const double exactStopSpeed = MSCFModel_Krauss::stopSpeed(veh, speed, gap);
        const double errorStopSpeed = MSCFModel_Krauss::stopSpeed(veh, speed, perceivedGap);
        const double accelError = SPEED2ACCEL(errorStopSpeed-exactStopSpeed);
        std::cout << "  gapError=" << perceivedGap-gap << "\n  resulting accelError: " << accelError << std::endl;
    }
#endif
    return MSCFModel_Krauss::stopSpeed(veh, speed, perceivedGap);
}


double
MSCFModel_TCI::followSpeed(const MSVehicle* const veh, double speed, double gap, double predSpeed, double predMaxDecel, const MSVehicle* const pred) const {
    assert(veh->getDriverState()!=nullptr); // DriverState must be defined for vehicle with MSCFModel_TCI
    const double perceivedGap = veh->getDriverState()->getPerceivedHeadway(gap);
    const double perceivedSpeedDifference = veh->getDriverState()->getPerceivedSpeedDifference(predSpeed - speed, gap);
#ifdef DEBUG_DRIVER_ERRORS
    if DEBUG_COND {
        std::cout << SIMTIME << " veh '" << veh->getID() << "' -> MSCFModel_TCI::followSpeed()\n"
                << "  speed=" << speed << " gap=" << gap << " leaderSpeed=" << predSpeed
                << "\n  perceivedGap=" << perceivedGap << " perceivedLeaderSpeed=" << speed+perceivedSpeedDifference
                << " perceivedSpeedDifference=" << perceivedSpeedDifference
                << std::endl;
        const double exactFollowSpeed = MSCFModel_Krauss::followSpeed(veh, speed, gap, predSpeed, predMaxDecel);
        const double errorFollowSpeed = MSCFModel_Krauss::followSpeed(veh, speed, perceivedGap, speed + perceivedSpeedDifference, predMaxDecel);
        const double accelError = SPEED2ACCEL(errorFollowSpeed-exactFollowSpeed);
        std::cout << "  gapError=" << perceivedGap-gap << "  dvError=" << perceivedSpeedDifference-(predSpeed - speed)
                << "\n  resulting accelError: " << accelError << std::endl;
    }
#endif
    return MSCFModel_Krauss::followSpeed(veh, speed, perceivedGap, speed + perceivedSpeedDifference, predMaxDecel);
}


MSCFModel*
MSCFModel_TCI::duplicate(const MSVehicleType* vtype) const {
    return new MSCFModel_TCI(vtype, myAccel, myDecel, myEmergencyDecel, myApparentDecel, myHeadwayTime);
}


/****************************************************************************/
