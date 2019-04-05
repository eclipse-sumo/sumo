/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2004-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    DynamicPolygon.cpp
/// @author  Leonhard Luecken
/// @date    Apr 2019
/// @version $Id$
///
// A polygon, which holds a timeSpan for displaying dynamic properties
/****************************************************************************/


#include "PolygonDynamics.h"

#include <assert.h>
#include "MSNet.h"
#include "MSEventControl.h"
#include "SUMOTrafficObject.h"
#include "WrappingCommand.h"



#define DEBUG_DYNAMIC_SHAPES

PolygonDynamics::PolygonDynamics(SUMOPolygon* p,
        SUMOTrafficObject* trackedObject,
        std::shared_ptr<std::vector<double> > timeSpan,
        std::shared_ptr<std::vector<double> > alphaSpan) :
    myPolygon(p),
    animated(timeSpan != nullptr),
    myTrackedObject(trackedObject),
    tracking(trackedObject != nullptr),
    myTimeSpan(timeSpan),
    myAlphaSpan(alphaSpan),
    myCurrentTime(0),
    myLastUpdateTime(SIMTIME)
{
    // Check for consistency (TODO: add analogous checks in libsumo)
    if (animated) {
        assert(myTimeSpan->size() >= 2);
        assert(myTimeSpan[0] == 0.0);
        assert(myAlphaSpan == nullptr || myAlphaSpan->size() == myTimeSpan->size());
#ifdef DEBUG_DYNAMIC_SHAPES
        if (myTimeSpan->size() >= 2) {
            for (int i = 1; i < myTimeSpan->size(); ++i) {
                assert(myTimeSpan[i-1] <= myTimeSpan[i]);
            }
        }
#endif
    }
#ifdef DEBUG_DYNAMIC_SHAPES
    else  {
        assert(myAlphaSpan == nullptr);
    }
    if (tracking) {
        myTrackedPos = std::make_shared<Position>(myTrackedObject->getPosition());
    } else {
        myTrackedPos(nullptr);
    }
}

PolygonDynamics::~PolygonDynamics()
{}


SUMOTime
PolygonDynamics::update(SUMOTime t) {
    double dt = STEPS2TIME(myLastUpdateTime - t);
    assert(dt > 0);

    SUMOTime ret = DELTA_T;

    if (tracking) {
        // Update shape position according to the movement of the tracked object
        myPolygon->myShape.add(myTrackedObject->getPosition() - *myTrackedPos);
        myTrackedPos->set(myTrackedObject->getPosition());
    }

    if (animated) {
        // Continue animation
        myCurrentTime += dt;
        while (myCurrentTime >= *myNextTime) {
            // step forward along time lines to appropriate anchor points
            ++myPrevTime; ++myNextTime;
            if (myNextTime != myTimeSpan->end()) {
                // Set iterators back to point to valid positions
                --myPrevTime; --myNextTime;
                break;
            }
            // Forward corresponding iterators for property time lines
            if (myAlphaSpan != nullptr) {
                ++myPrevAlpha; ++myNextAlpha;
            }
        }

        // Linear interpolation factor between previous and next time
        double theta = 1.0;
        if (myCurrentTime >= *myNextTime) {
            // Reached the end of the dynamics, indicate expiration by returning zero
            // and set all properties to the final state (theta remains one)
            ret = 0;
        } else {
            // Animation is still going on, schedule next update
            if (*myNextTime - *myPrevTime != 0) {
                theta = (myCurrentTime - *myPrevTime) / (*myNextTime - *myPrevTime);
            }
        }
        // Interpolate values of properties
        setAlpha(*myPrevAlpha + theta*(*myNextAlpha - *myPrevAlpha));
    }
    return ret;
}


void
PolygonDynamics::setAlpha(double alpha) {
    myPolygon->myColor.myAlpha = (unsigned char)(alpha*(double)std::numeric_limits<unsigned char>::max());
#ifdef DEBUG_DYNAMIC_SHAPES
    std::cout << "DynamicPolygon::setAlpha() Converted alpha=" << alpha << " into myAlpha="<<myPolygon->myColor.myAlpha << std::endl;
#endif
}
