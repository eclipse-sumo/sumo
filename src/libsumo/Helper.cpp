/****************************************************************************/
// Eclipse SUMO, Helper of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2017 German Aerospace Center (DLR) and others.
/****************************************************************************/
//
//   This program and the accompanying materials
//   are made available under the terms of the Eclipse Public License v2.0
//   which accompanies this distribution, and is available at
//   http://www.eclipse.org/legal/epl-v20.html
//
/****************************************************************************/
/// @file    Helper.cpp
/// @author  Laura Bieker-Walz
/// @author  Robert Hilbrich
/// @date    15.09.2017
/// @version $Id$
///
// C++ TraCI client API implementation
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <microsim/MSNet.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSInsertionControl.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <libsumo/TraCIDefs.h>
#include "Helper.h"


// ===========================================================================
// member definitions
// ===========================================================================
namespace libsumo {
    TraCIPositionVector
    Helper::makeTraCIPositionVector(const PositionVector& positionVector) {
        TraCIPositionVector tp;
        for (int i = 0; i < (int)positionVector.size(); ++i) {
            tp.push_back(makeTraCIPosition(positionVector[i]));
        }
        return tp;
    }


    PositionVector
    Helper::makePositionVector(const TraCIPositionVector& vector) {
        PositionVector pv;
        for (int i = 0; i < (int)vector.size(); i++) {
            pv.push_back(Position(vector[i].x, vector[i].y));
        }
        return pv;
    }


    TraCIColor
    Helper::makeTraCIColor(const RGBColor& color) {
        TraCIColor tc;
        tc.a = color.alpha();
        tc.b = color.blue();
        tc.g = color.green();
        tc.r = color.red();
        return tc;
    }


    RGBColor
    Helper::makeRGBColor(const TraCIColor& c) {
        return RGBColor((unsigned char)c.r, (unsigned char)c.g, (unsigned char)c.b, (unsigned char)c.a);
    }


    TraCIPosition
    Helper::makeTraCIPosition(const Position& position) {
        TraCIPosition p;
        p.x = position.x();
        p.y = position.y();
        p.z = position.z();
        return p;
    }


    Position
    Helper::makePosition(const TraCIPosition& tpos) {
        Position p;
        p.set(tpos.x, tpos.y, tpos.z);
        return p;
    }


    MSEdge*
    Helper::getEdge(const std::string& edgeID) {
        MSEdge* edge = MSEdge::dictionary(edgeID);
        if (edge == 0) {
            throw TraCIException("Referenced edge '" + edgeID + "' is not known.");
        }
        return edge;
    }


    const MSLane*
    Helper::getLaneChecking(const std::string& edgeID, int laneIndex, double pos) {
        const MSEdge* edge = MSEdge::dictionary(edgeID);
        if (edge == 0) {
            throw TraCIException("Unknown edge " + edgeID);
        }
        if (laneIndex < 0 || laneIndex >= (int)edge->getLanes().size()) {
            throw TraCIException("Invalid lane index for " + edgeID);
        }
        const MSLane* lane = edge->getLanes()[laneIndex];
        if (pos < 0 || pos > lane->getLength()) {
            throw TraCIException("Position on lane invalid");
        }
        return lane;
    }


    std::pair<MSLane*, double>
    Helper::convertCartesianToRoadMap(Position pos) {
        /// XXX use rtree instead
        std::pair<MSLane*, double> result;
        std::vector<std::string> allEdgeIds;
        double minDistance = std::numeric_limits<double>::max();

        allEdgeIds = MSNet::getInstance()->getEdgeControl().getEdgeNames();
        for (std::vector<std::string>::iterator itId = allEdgeIds.begin(); itId != allEdgeIds.end(); itId++) {
            const std::vector<MSLane*>& allLanes = MSEdge::dictionary((*itId))->getLanes();
            for (std::vector<MSLane*>::const_iterator itLane = allLanes.begin(); itLane != allLanes.end(); itLane++) {
                const double newDistance = (*itLane)->getShape().distance2D(pos);
                if (newDistance < minDistance) {
                    minDistance = newDistance;
                    result.first = (*itLane);
                }
            }
        }
        // @todo this may be a place where 3D is required but 2D is delivered
        result.second = result.first->getShape().nearest_offset_to_point2D(pos, false);
        return result;
    }
}


/****************************************************************************/
