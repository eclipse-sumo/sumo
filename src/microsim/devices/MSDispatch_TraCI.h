/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2007-2024 German Aerospace Center (DLR) and others.
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
/// @file    MSDispatch_TraCI.h
/// @author  Jakob Erdmann
/// @date    16.12.2019
///
// An algorithm that performs dispatch for the taxi device
/****************************************************************************/
#pragma once
#include <config.h>

#include <set>
#include <vector>
#include <map>
#include <utils/common/Parameterised.h>
#include <utils/common/SUMOTime.h>
#include "MSDispatch.h"
#include "MSDevice_Taxi.h"


// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class MSDispatch_TraCI
 * @brief A dispatch algorithm that services customers in reservation order and always sends the closest available taxi
 */
class MSDispatch_TraCI : public MSDispatch {
public:
    MSDispatch_TraCI(const Parameterised::Map& params) :
        MSDispatch(params)
    { }

    /// @brief add a new reservation
    Reservation* addReservation(MSTransportable* person,
                                SUMOTime reservationTime,
                                SUMOTime pickupTime,
                                SUMOTime earliestPickupTime,
                                const MSEdge* from, double fromPos,
                                const MSStoppingPlace* fromStop,
                                const MSEdge* to, double toPos,
                                const MSStoppingPlace* toStop,
                                std::string group,
                                const std::string& line,
                                int maxCapacity,
                                int maxContainerCapacity) override;

    /// @brief remove person from reservation. If the whole reservation is removed, return its id
    std::string removeReservation(MSTransportable* person,
                                  const MSEdge* from, double fromPos,
                                  const MSEdge* to, double toPos,
                                  std::string group) override;

    /// @brief do nothing (dispatch happens via TraCI calls)
    void computeDispatch(SUMOTime /*now*/, const std::vector<MSDevice_Taxi*>& /*fleet*/) override {}

    /// @brief trigger taxi dispatch. @note: method exists so subclasses can inject code at this point (ride sharing)
    void interpretDispatch(MSDevice_Taxi* taxi, const std::vector<std::string>& reservationsIDs);

    /// @brief split existing reservations and return the new reservation id
    std::string splitReservation(std::string resID, std::vector<std::string> personIDs);

    /// @brief erase reservation from storage
    void fulfilledReservation(const Reservation* res) override;

private:
    StringBijection<const Reservation*> myReservationLookup;

private:
    /// @brief Invalidated assignment operator.
    MSDispatch_TraCI& operator=(const MSDispatch_TraCI&) = delete;

};
