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
#include "utils/vehicle/SUMOTrafficObject.h"
#include "utils/common/SUMOTime.h"


#define DEBUG_DYNAMIC_SHAPES

PolygonDynamics::PolygonDynamics(double creationTime,
        SUMOPolygon* p,
        SUMOTrafficObject* trackedObject,
        std::shared_ptr<std::vector<double> > timeSpan,
        std::shared_ptr<std::vector<double> > alphaSpan) :
    myPolygon(p),
    myCurrentTime(0),
    myLastUpdateTime(creationTime),
    animated(timeSpan != nullptr),
    tracking(trackedObject != nullptr),
    myTrackedObject(trackedObject),
    myTimeSpan(timeSpan),
    myAlphaSpan(alphaSpan)
{
    // Check for consistency (TODO: add analogous checks in libsumo)
    if (animated) {
        assert(myTimeSpan->size() >= 2);
        assert((*myTimeSpan)[0] == 0.0);
        assert(myAlphaSpan == nullptr || myAlphaSpan->size() == myTimeSpan->size());
#ifdef DEBUG_DYNAMIC_SHAPES
        if (myTimeSpan->size() >= 2) {
            for (unsigned int i = 1; i < myTimeSpan->size(); ++i) {
                assert((*myTimeSpan)[i-1] <= (*myTimeSpan)[i]);
            }
        }
#endif
    }
#ifdef DEBUG_DYNAMIC_SHAPES
    else  {
        assert(myAlphaSpan == nullptr);
    }
#endif
    if (tracking) {
        myTrackedPos = std::make_shared<Position>(myTrackedObject->getPosition());
    } else {
        myTrackedPos = nullptr;
    }
}

PolygonDynamics::~PolygonDynamics()
{}


SUMOTime
PolygonDynamics::update(SUMOTime t) {
    const double simtime = STEPS2TIME(t);
    myLastUpdateTime = simtime;
    const double dt = myLastUpdateTime - simtime;
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
    unsigned char a = (unsigned char)(alpha*(double)std::numeric_limits<unsigned char>::max());
    myPolygon->setShapeAlpha(a);
#ifdef DEBUG_DYNAMIC_SHAPES
    std::cout << "DynamicPolygon::setAlpha() Converted alpha=" << alpha << " into myAlpha=" << a << std::endl;
#endif
}
