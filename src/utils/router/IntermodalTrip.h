/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    IntermodalTrip.h
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Robert Hilbrich
/// @date    Mon, 03 March 2014
/// @version $Id$
///
// The "vehicle" definition for the Intermodal Router
/****************************************************************************/
#ifndef IntermodalTrip_h
#define IntermodalTrip_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <string>
#include <vector>

#include "EffortCalculator.h"


// ===========================================================================
// class definitions
// ===========================================================================
/// @brief the "vehicle" type that is given to the internal router (SUMOAbstractRouter)
template<class E, class N, class V>
class IntermodalTrip {
public:
    IntermodalTrip(const E* _from, const E* _to, double _departPos, double _arrivalPos,
                   double _speed, SUMOTime _departTime, const N* _node,
                   const V* _vehicle = 0, const SVCPermissions _modeSet = SVC_PEDESTRIAN,
                   const EffortCalculator* const _calc = nullptr, const double _externalFactor = 0.) :
        from(_from),
        to(_to),
        departPos(_departPos < 0 ? _from->getLength() + _departPos : _departPos),
        arrivalPos(_arrivalPos < 0 ? _to->getLength() + _arrivalPos : _arrivalPos),
        speed(_speed),
        departTime(_departTime),
        node(_node),
        vehicle(_vehicle),
        modeSet(_modeSet),
        calc(_calc),
        externalFactor(_externalFactor) {
    }

    // exists just for debugging purposes
    std::string getID() const {
        return from->getID() + ":" + to->getID() + ":" + time2string(departTime);
    }


    inline SUMOVehicleClass getVClass() const {
        return vehicle != 0 ? vehicle->getVClass() : SVC_PEDESTRIAN;
    }

    // only used by AStar
    inline double getMaxSpeed() const {
        return vehicle != nullptr ? vehicle->getMaxSpeed() : speed;
    }

    // only used by AStar
    inline double getChosenSpeedFactor() const {
        return vehicle != nullptr ? vehicle->getChosenSpeedFactor() : 1.0;
    }

    const E* const from;
    const E* const to;
    const double departPos;
    const double arrivalPos;
    const double speed;
    const SUMOTime departTime;
    const N* const node; // indicates whether only routing across this node shall be performed
    const V* const vehicle; // indicates which vehicle may be used
    const SVCPermissions modeSet;
    const EffortCalculator* const calc;
    const double externalFactor;

private:
    /// @brief Invalidated assignment operator.
    IntermodalTrip& operator=(const IntermodalTrip&);
};


#endif

/****************************************************************************/
