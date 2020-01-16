/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    AccessEdge.h
/// @author  Michael Behrisch
/// @date    Mon, 03 March 2014
///
// The AccessEdge is a special intermodal edge connecting different modes
/****************************************************************************/
#ifndef AccessEdge_h
#define AccessEdge_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "IntermodalEdge.h"


// ===========================================================================
// class definitions
// ===========================================================================
/// @brief the access edge connecting different modes that is given to the internal router (SUMOAbstractRouter)
template<class E, class L, class N, class V>
class AccessEdge : public IntermodalEdge<E, L, N, V> {
private:
    typedef IntermodalEdge<E, L, N, V> _IntermodalEdge;

public:
    AccessEdge(int numericalID, const _IntermodalEdge* inEdge, const _IntermodalEdge* outEdge, const double length, 
            SVCPermissions modeRestriction = SVC_IGNORING,
            SVCPermissions vehicleRestriction = SVC_IGNORING) :
        _IntermodalEdge(inEdge->getID() + ":" + outEdge->getID(), numericalID, outEdge->getEdge(), "!access"),
        myLength(length > 0. ? length : NUMERICAL_EPS),
        myModeRestrictions(modeRestriction),
        myVehicleRestriction(vehicleRestriction)
        { }

    AccessEdge(int numericalID, const std::string& id, const E* edge, const double length = 0,
            SVCPermissions modeRestriction = SVC_IGNORING,
            SVCPermissions vehicleRestriction = SVC_IGNORING) :
        _IntermodalEdge(id, numericalID, edge, "!access"),
        myLength(length > 0. ? length : NUMERICAL_EPS),
        myModeRestrictions(modeRestriction),
        myVehicleRestriction(vehicleRestriction)
        { }

    double getTravelTime(const IntermodalTrip<E, N, V>* const trip, double /* time */) const {
        return myLength / trip->speed;
    }

    bool prohibits(const IntermodalTrip<E, N, V>* const trip) const {
        return ((trip->modeSet & myModeRestrictions) != myModeRestrictions 
                || ((trip->vehicle == nullptr ? SVC_IGNORING : trip->vehicle->getVClass()) & myVehicleRestriction) != myVehicleRestriction);
    }

private:
    const double myLength;
    /// @brief only allow using this edge if the modeSet matches (i.e. entering a taxi)
    const SVCPermissions myModeRestrictions;
    /// @brief only allow using this edge if the vehicle class matches (i.e. exiting a taxi)
    const SVCPermissions myVehicleRestriction;

};


#endif

/****************************************************************************/
