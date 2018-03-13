/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    IntermodalRouter.h
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Mon, 03 March 2014
/// @version $Id: IntermodalRouter.h v0_32_0+0557-f9fef86256 oss@behrisch.de 2018-02-20 17:38:42 +0100 $
///
// The IntermodalRouter builds a special network and (delegates to a SUMOAbstractRouter)
/****************************************************************************/
#ifndef IntermodalRouter_h
#define IntermodalRouter_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/SUMOTime.h>
#include <utils/common/ToString.h>
#include <utils/iodevices/OutputDevice.h>
#include "SUMOAbstractRouter.h"
#include "DijkstraRouter.h"
#include "IntermodalNetwork.h"
#include "CarEdge.h"
#include "StopEdge.h"
#include "PedestrianRouter.h"

//#define IntermodalRouter_DEBUG_ROUTES


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class IntermodalRouter
 * The router for pedestrians (on a bidirectional network of sidewalks and crossings)
 */
template<class E, class L, class N, class V, class INTERNALROUTER = DijkstraRouter<IntermodalEdge<E, L, N, V>, IntermodalTrip<E, N, V>, prohibited_withPermissions<IntermodalEdge<E, L, N, V>, IntermodalTrip<E, N, V> > > >
class IntermodalRouter : public SUMOAbstractRouter<E, IntermodalTrip<E, N, V> > {
public:
    typedef IntermodalNetwork<E, L, N, V> Network;

private:
    typedef void(*CreateNetCallback)(IntermodalRouter <E, L, N, V, INTERNALROUTER>&);
    typedef IntermodalEdge<E, L, N, V> _IntermodalEdge;
    typedef IntermodalTrip<E, N, V> _IntermodalTrip;

public:
    struct TripItem {
        TripItem(const std::string& _line = "") : line(_line), cost(0.) {}
        std::string line;
        std::string destStop;
        std::vector<const E*> edges;
        double cost;
    };

    /// Constructor
    IntermodalRouter(CreateNetCallback callback, int carWalkTransfer) :
        SUMOAbstractRouter<E, _IntermodalTrip>(0, "IntermodalRouter"),
        myAmClone(false), myInternalRouter(0), myIntermodalNet(0),
        myCallback(callback), myCarWalkTransfer(carWalkTransfer) {
    }

    /// Destructor
    virtual ~IntermodalRouter() {
        delete myInternalRouter;
        if (!myAmClone) {
            delete myIntermodalNet;
        }
    }

    SUMOAbstractRouter<E, _IntermodalTrip>* clone() {
        createNet();
        return new IntermodalRouter<E, L, N, V, INTERNALROUTER>(myIntermodalNet);
    }

    /** @brief Builds the route between the given edges using the minimum effort at the given time
        The definition of the effort depends on the wished routing scheme */
    bool compute(const E* from, const E* to, const double departPos, const double arrivalPos,
                 const std::string stopID, const double speed,
                 const V* const vehicle, const SVCPermissions modeSet, SUMOTime msTime,
                 std::vector<TripItem>& into) {
        createNet();
        _IntermodalTrip trip(from, to, departPos, arrivalPos, speed, msTime, 0, vehicle, modeSet);
        std::vector<const _IntermodalEdge*> intoEdges;
        const bool success = myInternalRouter->compute(myIntermodalNet->getDepartEdge(from, trip.departPos),
                             stopID != "" ? myIntermodalNet->getStopEdge(stopID) : myIntermodalNet->getArrivalEdge(to, trip.arrivalPos),
                             &trip, msTime, intoEdges);
        if (success) {
            std::string lastLine = "";
            double time = STEPS2TIME(msTime);
            for (const _IntermodalEdge* iEdge : intoEdges) {
                if (iEdge->includeInRoute(false)) {
                    if (iEdge->getLine() == "!stop") {
                        into.back().destStop = iEdge->getID();
                        if (lastLine == "!ped") {
                            lastLine = ""; // a stop always starts a new trip item
                        }
                    } else {
                        if (iEdge->getLine() != lastLine) {
                            lastLine = iEdge->getLine();
                            if (lastLine == "!car") {
                                into.push_back(TripItem(vehicle->getID()));
                            } else if (lastLine == "!ped") {
                                into.push_back(TripItem());
                            } else {
                                into.push_back(TripItem(lastLine));
                            }
                        }
                        if (into.back().edges.empty() || into.back().edges.back() != iEdge->getEdge()) {
                            into.back().edges.push_back(iEdge->getEdge());
                        }
                    }
                }
                const double edgeEffort = myInternalRouter->getEffort(iEdge, &trip, time);
                time += edgeEffort;
                if (!into.empty()) {
                    into.back().cost += edgeEffort;
                }
            }
        }
#ifdef IntermodalRouter_DEBUG_ROUTES
        double time = STEPS2TIME(msTime);
        for (const _IntermodalEdge* iEdge : intoEdges) {
            const double edgeEffort = myInternalRouter->getEffort(iEdge, &trip, time);
            time += edgeEffort;
            std::cout << iEdge->getID() << "(" << iEdge->getLine() << "): " << edgeEffort << std::endl;
        }
        std::cout << TIME2STEPS(msTime) << " trip from " << from->getID() << " to " << to->getID()
                  << " departPos=" << trip.departPos
                  << " arrivalPos=" << trip.arrivalPos
                  << " edges=" << toString(intoEdges)
//                  << " resultEdges=" << toString(into)
                  << " time=" << time
                  << "\n";
#endif
        return success;
    }

    /** @brief Builds the route between the given edges using the minimum effort at the given time
        The definition of the effort depends on the wished routing scheme */
    bool compute(const E*, const E*, const _IntermodalTrip* const,
                 SUMOTime, std::vector<const E*>&) {
        throw ProcessError("Do not use this method");
    }

    double recomputeCosts(const std::vector<const E*>&, const _IntermodalTrip* const, SUMOTime) const {
        throw ProcessError("Do not use this method");
    }

    void prohibit(const std::vector<E*>& toProhibit) {
        createNet();
        std::vector<_IntermodalEdge*> toProhibitPE;
        for (typename std::vector<E*>::const_iterator it = toProhibit.begin(); it != toProhibit.end(); ++it) {
            toProhibitPE.push_back(myIntermodalNet->getBothDirections(*it).first);
            toProhibitPE.push_back(myIntermodalNet->getBothDirections(*it).second);
            toProhibitPE.push_back(myIntermodalNet->getCarEdge(*it));
        }
        myInternalRouter->prohibit(toProhibitPE);
    }

    void writeNetwork(OutputDevice& dev) {
        createNet();
        for (_IntermodalEdge* e : myIntermodalNet->getAllEdges()) {
            dev.openTag(SUMO_TAG_EDGE);
            dev.writeAttr(SUMO_ATTR_ID, e->getID());
            dev.writeAttr(SUMO_ATTR_LENGTH, e->getLength());
            dev.writeAttr("successors", toString(e->getSuccessors(SVC_IGNORING)));
            dev.closeTag();
        }
    }

    void writeWeights(OutputDevice& dev) {
        createNet();
        for (_IntermodalEdge* e : myIntermodalNet->getAllEdges()) {
            dev.openTag(SUMO_TAG_EDGE);
            dev.writeAttr(SUMO_ATTR_ID, e->getID());
            dev.writeAttr("traveltime", 0. /* e->getTravelTime() */);
            dev.writeAttr("effort", 0. /* e->getEffort() */);
            dev.closeTag();
        }
    }

    Network* getNetwork() const {
        return myIntermodalNet;
    }

private:
    IntermodalRouter(Network* net):
        SUMOAbstractRouter<E, _IntermodalTrip>(0, "PedestrianRouter"), myAmClone(true),
        myInternalRouter(new INTERNALROUTER(net->getAllEdges(), true, &_IntermodalEdge::getTravelTimeStatic)),
        myIntermodalNet(net), myCarWalkTransfer(0) {}

    inline void createNet() {
        if (myIntermodalNet == nullptr) {
            myIntermodalNet = new Network(E::getAllEdges(), false, myCarWalkTransfer);
            myIntermodalNet->addCarEdges(E::getAllEdges());
            myCallback(*this);
            myInternalRouter = new INTERNALROUTER(myIntermodalNet->getAllEdges(), true, &_IntermodalEdge::getTravelTimeStatic);
        }
    }

private:
    const bool myAmClone;
    INTERNALROUTER* myInternalRouter;
    Network* myIntermodalNet;
    CreateNetCallback myCallback;
    const int myCarWalkTransfer;


private:
    /// @brief Invalidated assignment operator
    IntermodalRouter& operator=(const IntermodalRouter& s);

};


#endif

/****************************************************************************/
