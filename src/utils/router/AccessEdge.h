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
/// @version $Id$
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
    AccessEdge(int numericalID, const _IntermodalEdge* inEdge, const _IntermodalEdge* outEdge, const double length) :
        _IntermodalEdge(inEdge->getID() + ":" + outEdge->getID(), numericalID, outEdge->getEdge(), "!access"),
        myLength(length > 0. ? length : NUMERICAL_EPS) { }

    double getTravelTime(const IntermodalTrip<E, N, V>* const trip, double /* time */) const {
        return myLength / trip->speed;
    }

private:
    const double myLength;

};


#endif

/****************************************************************************/
