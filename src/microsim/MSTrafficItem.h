/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSTrafficItem.h
/// @author  Michael Behrisch
/// @date    Tue, 21 Apr 2015
/// @version $Id$
///
// The common superclass for modelling transportable objects like persons and containers
/****************************************************************************/
#ifndef MSTrafficItem_h
#define MSTrafficItem_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <memory>
#include <utils/common/SUMOTime.h>

// ===========================================================================
// class declarations
// ===========================================================================
class MSJunction;
class MSLink;
class MSVehicle;
class PedestrianState;


// ===========================================================================
// class definitions
// ===========================================================================

/// @brief base class for VehicleCharacteristics, TLSCharacteristics, PedestrianCharacteristics, SpeedLimitCharacteristics, Junction Characteristics...
/// @see TrafficItemType, @see MSCFModel_TCI
struct MSTrafficItemCharacteristics {
    inline virtual ~MSTrafficItemCharacteristics() {};
};

// @brief Types of traffic items, @see TrafficItem
enum MSTrafficItemType {
    TRAFFIC_ITEM_VEHICLE,
    TRAFFIC_ITEM_TLS,
    TRAFFIC_ITEM_PEDESTRIAN,
    TRAFFIC_ITEM_SPEED_LIMIT,
    TRAFFIC_ITEM_JUNCTION
};

/** @class TrafficItem
 * @brief  An object representing a traffic item. Used for influencing
 *         the task demand of the TCI car-following model.
 * @see MSCFModel_TCI
 */
struct MSTrafficItem {
    MSTrafficItem(MSTrafficItemType type, const std::string& id, std::shared_ptr<MSTrafficItemCharacteristics> data);
    static std::hash<std::string> hash;
    MSTrafficItemType type;
    size_t id_hash;
    std::shared_ptr<MSTrafficItemCharacteristics> data;
    SUMOTime remainingIntegrationTime;
    double integrationDemand;
    double latentDemand;
};

struct JunctionCharacteristics : MSTrafficItemCharacteristics {
    JunctionCharacteristics(MSJunction* junction, MSLink* egoLink, double dist) :
        junction(junction), egoLink(egoLink), dist(dist) {};
    MSJunction* junction;
    MSLink* egoLink;
    double dist;
};

struct PedestrianCharacteristics : MSTrafficItemCharacteristics {
    PedestrianCharacteristics(PedestrianState* pedestrian, double dist) :
        pedestrian(pedestrian), dist(dist) {};
    PedestrianState* pedestrian;
    double dist;
};

struct SpeedLimitCharacteristics : MSTrafficItemCharacteristics {
    SpeedLimitCharacteristics(double dist, double limit) :
        dist(dist), limit(limit) {};
    double dist;
    double limit;
};

struct TLSCharacteristics : MSTrafficItemCharacteristics {
    TLSCharacteristics(double dist, char state, int nrLanes) :
        dist(dist), state(state), nrLanes(nrLanes) {};
    double dist;
    double state;
    int nrLanes;
};

struct VehicleCharacteristics : MSTrafficItemCharacteristics {
    VehicleCharacteristics(MSVehicle* ego, MSVehicle* foe, double longitudinalDist, double lateralDist) :
        longitudinalDist(longitudinalDist), lateralDist(lateralDist), ego(ego), foe(foe) {};
    MSVehicle* ego;
    MSVehicle* foe;
    double longitudinalDist;
    double lateralDist;
};



#endif

/****************************************************************************/
