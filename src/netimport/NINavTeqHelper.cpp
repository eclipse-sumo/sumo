/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
/// @file    NINavTeqHelper.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Jul 2006
///
// Some parser methods shared around several formats containing NavTeq-Nets
/****************************************************************************/
#include <config.h>

#include <utils/common/StringUtils.h>
#include <utils/common/StringUtils.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <netbuild/NBEdge.h>

#include "NINavTeqHelper.h"


// ===========================================================================
// method definitions
// ===========================================================================
double
NINavTeqHelper::getSpeed(const std::string& id, const std::string& speedClassS) {
    try {
        int speedClass = StringUtils::toInt(speedClassS);
        switch (speedClass) {
            case -1:
                return (double) 1.0 / (double) 3.6;
            case 1:
                return (double) 200 / (double) 3.6; //> 130 KPH / > 80 MPH
            case 2:
                return (double) 120 / (double) 3.6; //101-130 KPH / 65-80 MPH
            case 3:
                return (double) 100 / (double) 3.6; // 91-100 KPH / 55-64 MPH
            case 4:
                return (double) 80 / (double) 3.6; // 71-90 KPH / 41-54 MPH
            case 5:
                return (double) 70 / (double) 3.6; // 51-70 KPH / 31-40 MPH
            case 6:
                return (double) 50 / (double) 3.6; // 31-50 KPH / 21-30 MPH
            case 7:
                return (double) 30 / (double) 3.6; // 11-30 KPH / 6-20 MPH
            case 8:
                return (double) 5 / (double) 3.6; //< 11 KPH / < 6 MPH
            default:
                throw ProcessError(TLF("Invalid speed code (edge '%').", id));
        }
    } catch (NumberFormatException&) {
        throw ProcessError(TLF("Non-numerical value for an edge's speed type occurred (edge '%').", id));
    }
}


int
NINavTeqHelper::getLaneNumber(const std::string& id, const std::string& laneNoS, double speed) {
    try {
        int nolanes = StringUtils::toInt(laneNoS);
        if (nolanes < 0) {
            return 1;
        } else if (nolanes / 10 > 0) {
            return nolanes / 10;
        } else {
            switch (nolanes % 10) {
                case 1:
                    return 1;
                case 2:
                    nolanes = 2;
                    if (speed > 78.0 / 3.6) {
                        nolanes = 3;
                    }
                    return nolanes;
                case 3:
                    return 4;
                default:
                    throw ProcessError(TLF("Invalid lane number (edge '%').", id));
            }
        }
    } catch (NumberFormatException&) {
        throw ProcessError(TLF("Non-numerical value for an edge's lane number occurred (edge '%').", id));
    }
}


bool
NINavTeqHelper::addCommonVehicleClasses(SVCPermissions& permissions, const std::string& classS, const int offset) {
    bool haveCar = false;
    // High Occupancy Vehicle -- becomes SVC_PASSENGER|SVC_HOV
    if (classS[offset] == '1') {
        permissions |= SVC_HOV | SVC_PASSENGER;
        haveCar = true;
    } else {
        permissions &= ~SVC_HOV;
    }
    // Emergency Vehicle -- becomes SVC_EMERGENCY
    if (classS[offset + 1] == '1') {
        permissions |= SVC_EMERGENCY;
    } else {
        permissions &= ~SVC_EMERGENCY;
    }
    // Taxi -- becomes SVC_TAXI
    if (classS[offset + 2] == '1') {
        permissions |= SVC_TAXI;
        haveCar = true;
    } else {
        permissions &= ~SVC_TAXI;
    }
    // Public Bus -- becomes SVC_BUS|SVC_COACH
    if (classS[offset + 3] == '1') {
        permissions |= SVC_BUS | SVC_COACH;
        haveCar = true;
    } else {
        permissions &= ~(SVC_BUS | SVC_COACH);
    }
    // Delivery Truck -- becomes SVC_DELIVERY
    if (classS[offset + 4] == '1') {
        permissions |= SVC_DELIVERY;
        haveCar = true;
    } else {
        permissions &= ~SVC_DELIVERY;
    }
    // Transport Truck -- becomes SVC_TRUCK|SVC_TRAILER
    if (classS[offset + 5] == '1') {
        permissions |= SVC_TRUCK | SVC_TRAILER;
        haveCar = true;
    } else {
        permissions &= ~(SVC_TRUCK | SVC_TRAILER);
    }
    return haveCar;
}


SVCPermissions
NINavTeqHelper::getVehicleClasses(const std::string& oclassS, const SVCPermissions allPermissions, const SVCPermissions defaultPermissions, double version) {
    if (version < 6.0) {
        std::string classS = "0000000000" + oclassS;
        classS = classS.substr(classS.length() - 10);
        if (classS[0] == '1') {
            return allPermissions;
        }
        bool haveCar = false;
        SVCPermissions permissions = defaultPermissions;
        if (classS[1] == '1') {
            permissions |= SVC_PASSENGER;
            haveCar = true;
        } else {
            permissions &= ~SVC_PASSENGER;
        }
        haveCar |= addCommonVehicleClasses(permissions, classS, 2);
        if (!haveCar) {
            permissions = 0;
        }
        if (classS[8] == '1') {
            permissions |= SVC_BICYCLE;
        } else {
            permissions &= ~SVC_BICYCLE;
        }
        if (classS[9] == '1') {
            permissions |= SVC_PEDESTRIAN;
        } else {
            permissions &= ~SVC_PEDESTRIAN;
        }
        return permissions;
    } else {
        std::string classS = "0000000000" + oclassS;
        classS = classS.substr(classS.length() - 12);
        if (classS[0] == '1') {
            return allPermissions;
        }
        bool haveCar = false;
        SVCPermissions permissions = defaultPermissions;
        if (classS[1] == '1') {
            permissions |= SVC_PASSENGER;
            haveCar = true;
        } else {
            permissions &= ~SVC_PASSENGER;
        }
        if (classS[2] == '1') {
            permissions |= SVC_PASSENGER;
            haveCar = true;
        }
        haveCar |= addCommonVehicleClasses(permissions, classS, 3);
        if (!haveCar) {
            permissions = 0;
        }
        if (classS[9] == '1') {
            permissions |= SVC_MOTORCYCLE;
        } else {
            permissions &= ~SVC_MOTORCYCLE;
        }
        if (classS[10] == '1') {
            permissions |= SVC_BICYCLE;
        } else {
            permissions &= ~SVC_BICYCLE;
        }
        if (classS[11] == '1') {
            permissions |= SVC_PEDESTRIAN;
        } else {
            permissions &= ~SVC_PEDESTRIAN;
        }
        return permissions;
    }
}


void
NINavTeqHelper::addVehicleClasses(NBEdge& e, const std::string& oclassS, const SVCPermissions allPermissions, const SVCPermissions defaultPermissions) {
    e.setPermissions(getVehicleClasses(oclassS, allPermissions, defaultPermissions, 5.0));
}


void
NINavTeqHelper::addVehicleClassesV6(NBEdge& e, const std::string& oclassS, const SVCPermissions allPermissions, const SVCPermissions defaultPermissions) {
    e.setPermissions(getVehicleClasses(oclassS, allPermissions, defaultPermissions, 6.0));
}


/****************************************************************************/
