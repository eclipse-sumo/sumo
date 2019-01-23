/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    StopEdge.h
/// @author  Michael Behrisch
/// @date    Mon, 03 March 2014
/// @version $Id$
///
// The StopEdge is a special intermodal edge representing bus and train stops
/****************************************************************************/
#ifndef StopEdge_h
#define StopEdge_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "IntermodalEdge.h"


// ===========================================================================
// class definitions
// ===========================================================================
/// @brief the stop edge type representing bus and train stops
template<class E, class L, class N, class V>
class StopEdge : public IntermodalEdge<E, L, N, V> {
public:
    StopEdge(const std::string id, int numericalID, const E* edge) :
        IntermodalEdge<E, L, N, V>(id, numericalID, edge, "!stop") { }

    bool includeInRoute(bool /* allEdges */) const {
        return true;
    }
};


#endif

/****************************************************************************/
