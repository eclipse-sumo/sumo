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
#include <microsim/MSVehicleControl.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSInsertionControl.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSTransportable.h>
#include <libsumo/TraCIDefs.h>
#include <libsumo/InductionLoop.h>
#include <libsumo/Junction.h>
#include <libsumo/POI.h>
#include <libsumo/Polygon.h>
#include <traci-server/TraCIConstants.h>
#include "Helper.h"

void
LaneStoringVisitor::add(const MSLane* const l) const {
    switch (myDomain) {
        case CMD_GET_VEHICLE_VARIABLE: {
            const MSLane::VehCont& vehs = l->getVehiclesSecure();
            for (MSLane::VehCont::const_iterator j = vehs.begin(); j != vehs.end(); ++j) {
                if (myShape.distance2D((*j)->getPosition()) <= myRange) {
                    myIDs.insert((*j)->getID());
                }
            }
            l->releaseVehicles();
        }
        break;
        case CMD_GET_PERSON_VARIABLE: {
            l->getVehiclesSecure();
            std::vector<MSTransportable*> persons = l->getEdge().getSortedPersons(MSNet::getInstance()->getCurrentTimeStep(), true);
            for (auto p : persons) {
                if (myShape.distance2D(p->getPosition()) <= myRange) {
                    myIDs.insert(p->getID());
                }
            }
            l->releaseVehicles();
        }
        break;
        case CMD_GET_EDGE_VARIABLE: {
            if (myShape.size() != 1 || l->getShape().distance2D(myShape[0]) <= myRange) {
                myIDs.insert(l->getEdge().getID());
            }
        }
        break;
        case CMD_GET_LANE_VARIABLE: {
            if (myShape.size() != 1 || l->getShape().distance2D(myShape[0]) <= myRange) {
                myIDs.insert(l->getID());
            }
        }
        break;
        default:
            break;

    }
}

namespace libsumo {
    // ===========================================================================
    // static member definitions
    // ===========================================================================
    std::map<int, NamedRTree*> Helper::myObjects;
    LANE_RTREE_QUAL* Helper::myLaneTree;
    std::map<std::string, MSVehicle*> Helper::myVTDControlledVehicles;


    // ===========================================================================
    // member definitions
    // ===========================================================================
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

    void
    Helper::cleanup() {
        for (std::map<int, NamedRTree*>::const_iterator i = myObjects.begin(); i != myObjects.end(); ++i) {
            delete(*i).second;
        }
        myObjects.clear();
        delete myLaneTree;
        myLaneTree = 0;
    }


    void
    Helper::collectObjectsInRange(int domain, const PositionVector& shape, double range, std::set<std::string>& into) {
        // build the look-up tree if not yet existing
        if (myObjects.find(domain) == myObjects.end()) {
            switch (domain) {
                case CMD_GET_INDUCTIONLOOP_VARIABLE:
                    myObjects[CMD_GET_INDUCTIONLOOP_VARIABLE] = InductionLoop::getTree();
                    break;
                case CMD_GET_EDGE_VARIABLE:
                case CMD_GET_LANE_VARIABLE:
                case CMD_GET_PERSON_VARIABLE:
                case CMD_GET_VEHICLE_VARIABLE:
                    myObjects[CMD_GET_EDGE_VARIABLE] = 0;
                    myObjects[CMD_GET_LANE_VARIABLE] = 0;
                    myObjects[CMD_GET_PERSON_VARIABLE] = 0;
                    myObjects[CMD_GET_VEHICLE_VARIABLE] = 0;
                    myLaneTree = new LANE_RTREE_QUAL(&MSLane::visit);
                    MSLane::fill(*myLaneTree);
                    break;
                case CMD_GET_POI_VARIABLE:
                    myObjects[CMD_GET_POI_VARIABLE] = POI::getTree();
                    break;
                case CMD_GET_POLYGON_VARIABLE:
                    myObjects[CMD_GET_POLYGON_VARIABLE] = Polygon::getTree();
                    break;
                case CMD_GET_JUNCTION_VARIABLE:
                    myObjects[CMD_GET_JUNCTION_VARIABLE] = Junction::getTree();
                    break;
                default:
                    break;
            }
        }
        const Boundary b = shape.getBoxBoundary().grow(range);
        const float cmin[2] = {(float) b.xmin(), (float) b.ymin()};
        const float cmax[2] = {(float) b.xmax(), (float) b.ymax()};
        switch (domain) {
            case CMD_GET_INDUCTIONLOOP_VARIABLE:
            case CMD_GET_POI_VARIABLE:
            case CMD_GET_POLYGON_VARIABLE:
            case CMD_GET_JUNCTION_VARIABLE: {
                Named::StoringVisitor sv(into);
                myObjects[domain]->Search(cmin, cmax, sv);
            }
            break;
            case CMD_GET_EDGE_VARIABLE:
            case CMD_GET_LANE_VARIABLE:
            case CMD_GET_PERSON_VARIABLE:
            case CMD_GET_VEHICLE_VARIABLE: {
                LaneStoringVisitor sv(into, shape, range, domain);
                myLaneTree->Search(cmin, cmax, sv);
            }
            break;
            default:
                break;
        }
    }

    void
        Helper::setVTDControlled(MSVehicle* v, Position xyPos, MSLane* l, double pos, double posLat, double angle,
                                  int edgeOffset, ConstMSEdgeVector route, SUMOTime t) {
        myVTDControlledVehicles[v->getID()] = v;
        v->getInfluencer().setVTDControlled(xyPos, l, pos, posLat, angle, edgeOffset, route, t);
    }


    void
        Helper::postProcessVTD() {
        for (std::map<std::string, MSVehicle*>::const_iterator i = myVTDControlledVehicles.begin(); i != myVTDControlledVehicles.end(); ++i) {
            if (MSNet::getInstance()->getVehicleControl().getVehicle((*i).first) != 0) {
                (*i).second->getInfluencer().postProcessVTD((*i).second);
            } else {
                WRITE_WARNING("Vehicle '" + (*i).first + "' was removed though being controlled by VTD");
            }
        }
        myVTDControlledVehicles.clear();
    }

}


/****************************************************************************/
