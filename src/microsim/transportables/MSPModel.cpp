/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2014-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSPModel.cpp
/// @author  Jakob Erdmann
/// @date    Mon, 13 Jan 2014
///
// The pedestrian following model (prototype)
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <cmath>
#include <algorithm>
#include <utils/options/OptionsCont.h>
#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <microsim/MSJunction.h>
#include <microsim/MSLane.h>
#include <microsim/MSGlobals.h>
#include "MSPModel_Striping.h"
#include "MSPModel_NonInteracting.h"
#include "MSPModel.h"


// ===========================================================================
// static members
// ===========================================================================
// named constants
const int MSPModel::FORWARD(1);
const int MSPModel::BACKWARD(-1);
const int MSPModel::UNDEFINED_DIRECTION(0);

// parameters shared by all models
const double MSPModel::SAFETY_GAP(1.0);
const double MSPModel::SIDEWALK_OFFSET(3);


// ===========================================================================
// MSPModel method definitions
// ===========================================================================
int
MSPModel::canTraverse(int dir, const ConstMSEdgeVector& route) {
    const MSJunction* junction = nullptr;
    for (ConstMSEdgeVector::const_iterator it = route.begin(); it != route.end(); ++it) {
        const MSEdge* edge = *it;
        if (junction != nullptr) {
            //std::cout << " junction=" << junction->getID() << " edge=" << edge->getID() << "\n";
            if (junction == edge->getFromJunction()) {
                dir = FORWARD;
            } else if (junction == edge->getToJunction()) {
                dir = BACKWARD;
            } else {
                return UNDEFINED_DIRECTION;
            }
        }
        junction = dir == FORWARD ? edge->getToJunction() : edge->getFromJunction();
    }
    return dir;
}


/****************************************************************************/
