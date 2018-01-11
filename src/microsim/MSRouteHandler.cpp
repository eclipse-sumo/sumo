/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSRouteHandler.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// Parser and container for routes during their loading
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <map>
#include <vector>
#include <microsim/MSRoute.h>
#include <microsim/MSEdge.h>
#include <microsim/MSJunction.h>
#include <microsim/MSVehicleType.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSInsertionControl.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSLane.h>
#include "MSRouteHandler.h"
#include "MSTransportableControl.h"
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/TplConvert.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/UtilExceptions.h>
#include <utils/options/OptionsCont.h>
#include "MSNet.h"

#include "MSParkingArea.h"
#include "MSStoppingPlace.h"
#include <microsim/MSGlobals.h>
#include <microsim/trigger/MSChargingStation.h>
#include <utils/xml/SUMOVehicleParserHelper.h>


// ===========================================================================
// static members
// ===========================================================================
std::mt19937 MSRouteHandler::myParsingRNG;


// ===========================================================================
// method definitions
// ===========================================================================
MSRouteHandler::MSRouteHandler(const std::string& file,
                               bool addVehiclesDirectly) :
    SUMORouteHandler(file),
    myActivePlan(0),
    myActiveContainerPlan(0),
    myAddVehiclesDirectly(addVehiclesDirectly),
    myCurrentVTypeDistribution(0),
    myCurrentRouteDistribution(0),
    myAmLoadingState(false) {
    myActiveRoute.reserve(100);
    // check for valid value has been performed in MSFrame
    myDefaultCFModel = SUMOXMLDefinitions::CarFollowModels.get(OptionsCont::getOptions().getString("carfollow.model"));
}


MSRouteHandler::~MSRouteHandler() {
}

void
MSRouteHandler::deleteActivePlans() {
    MSTransportable::MSTransportablePlan::iterator i;
    if (myActivePlan != 0) {
        for (i = myActivePlan->begin(); i != myActivePlan->end(); i++) {
            delete *i;
        }
        delete myActivePlan;
        myActivePlan = NULL;
    }
    if (myActiveContainerPlan != 0) {
        for (i = myActiveContainerPlan->begin(); i != myActiveContainerPlan->end(); i++) {
            delete *i;
        }
        delete myActiveContainerPlan;
        myActivePlan = NULL;
    }
}


void
MSRouteHandler::parseFromViaTo(std::string element,
                               const SUMOSAXAttributes& attrs) {
    myActiveRoute.clear();
    bool useTaz = OptionsCont::getOptions().getBool("with-taz");
    if (useTaz && !myVehicleParameter->wasSet(VEHPARS_FROM_TAZ_SET) && !myVehicleParameter->wasSet(VEHPARS_TO_TAZ_SET)) {
        WRITE_WARNING("Taz usage was requested but no taz present in " + element + " '" + myVehicleParameter->id + "'!");
        useTaz = false;
    }
    bool ok = true;
    if ((useTaz || !attrs.hasAttribute(SUMO_ATTR_FROM)) && myVehicleParameter->wasSet(VEHPARS_FROM_TAZ_SET)) {
        const MSEdge* fromTaz = MSEdge::dictionary(myVehicleParameter->fromTaz + "-source");
        if (fromTaz == 0) {
            throw ProcessError("Source taz '" + myVehicleParameter->fromTaz + "' not known for " + element + " '" + myVehicleParameter->id + "'!");
        } else if (fromTaz->getNumSuccessors() == 0) {
            throw ProcessError("Source taz '" + myVehicleParameter->fromTaz + "' has no outgoing edges for " + element + " '" + myVehicleParameter->id + "'!");
        } else {
            myActiveRoute.push_back(fromTaz);
        }
    } else {
        MSEdge::parseEdgesList(attrs.getOpt<std::string>(SUMO_ATTR_FROM, myVehicleParameter->id.c_str(), ok, "", true),
                               myActiveRoute, "for " + element + " '" + myVehicleParameter->id + "'");
    }
    if (!attrs.hasAttribute(SUMO_ATTR_VIA) && !attrs.hasAttribute(SUMO_ATTR_ROUTE)) {
        myInsertStopEdgesAt = (int)myActiveRoute.size();
    }
    MSEdge::parseEdgesList(attrs.getOpt<std::string>(SUMO_ATTR_VIA, myVehicleParameter->id.c_str(), ok, "", true),
                           myActiveRoute, "for " + element + " '" + myVehicleParameter->id + "'");
    myVehicleParameter->via = StringTokenizer(attrs.getOpt<std::string>(SUMO_ATTR_VIA, myVehicleParameter->id.c_str(), ok, "", true)).getVector();
    if ((useTaz || !attrs.hasAttribute(SUMO_ATTR_TO)) && myVehicleParameter->wasSet(VEHPARS_TO_TAZ_SET)) {
        const MSEdge* toTaz = MSEdge::dictionary(myVehicleParameter->toTaz + "-sink");
        if (toTaz == 0) {
            throw ProcessError("Sink taz '" + myVehicleParameter->toTaz + "' not known for " + element + " '" + myVehicleParameter->id + "'!");
        } else if (toTaz->getNumPredecessors() == 0) {
            throw ProcessError("Sink taz '" + myVehicleParameter->toTaz + "' has no incoming edges for " + element + " '" + myVehicleParameter->id + "'!");
        } else {
            myActiveRoute.push_back(toTaz);
        }
    } else {
        MSEdge::parseEdgesList(attrs.getOpt<std::string>(SUMO_ATTR_TO, myVehicleParameter->id.c_str(), ok, "", true),
                               myActiveRoute, "for " + element + " '" + myVehicleParameter->id + "'");
    }
    myActiveRouteID = "!" + myVehicleParameter->id;
    if (myVehicleParameter->routeid == "") {
        myVehicleParameter->routeid = myActiveRouteID;
    }
}


void
MSRouteHandler::myStartElement(int element,
                               const SUMOSAXAttributes& attrs) {
    SUMORouteHandler::myStartElement(element, attrs);
    try {
        switch (element) {
            case SUMO_TAG_PERSON:
                if (!MSNet::getInstance()->getVehicleControl().hasVType(myVehicleParameter->vtypeid)) {
                    const std::string error = "The type '" + myVehicleParameter->vtypeid + "' for person '" + myVehicleParameter->id + "' is not known.";
                    delete myVehicleParameter;
                    myVehicleParameter = 0;
                    throw ProcessError(error);
                }
                myActivePlan = new MSTransportable::MSTransportablePlan();
                break;
            case SUMO_TAG_CONTAINER:
                myActiveContainerPlan = new MSTransportable::MSTransportablePlan();
                break;
            case SUMO_TAG_RIDE: {
                const std::string pid = myVehicleParameter->id;
                bool ok = true;
                MSEdge* from = 0;
                const std::string desc = attrs.get<std::string>(SUMO_ATTR_LINES, pid.c_str(), ok);
                StringTokenizer st(desc);
                std::string bsID = attrs.getOpt<std::string>(SUMO_ATTR_BUS_STOP, 0, ok, "");
                MSStoppingPlace* bs = 0;
                MSEdge* to = 0;
                if (bsID != "") {
                    bs = MSNet::getInstance()->getStoppingPlace(bsID, SUMO_TAG_BUS_STOP);
                    if (bs == 0) {
                        throw ProcessError("Unknown bus stop '" + bsID + "' for person '" + myVehicleParameter->id + "'.");
                    }
                    to = &bs->getLane().getEdge();
                }
                double arrivalPos = attrs.getOpt<double>(SUMO_ATTR_ARRIVALPOS, myVehicleParameter->id.c_str(), ok,
                                    bs == 0 ? -NUMERICAL_EPS : bs->getEndLanePosition());
                if (attrs.hasAttribute(SUMO_ATTR_FROM)) {
                    const std::string fromID = attrs.get<std::string>(SUMO_ATTR_FROM, pid.c_str(), ok);
                    from = MSEdge::dictionary(fromID);
                    if (from == 0) {
                        throw ProcessError("The from edge '" + fromID + "' within a ride of person '" + pid + "' is not known.");
                    }
                    if (!myActivePlan->empty() && &myActivePlan->back()->getDestination() != from) {
                        throw ProcessError("Disconnected plan for person '" + myVehicleParameter->id + "' (" + fromID + "!=" + myActivePlan->back()->getDestination().getID() + ").");
                    }
                    if (myActivePlan->empty()) {
                        myActivePlan->push_back(new MSTransportable::Stage_Waiting(
                                                    *from, -1, myVehicleParameter->depart, myVehicleParameter->departPos, "start", true));
                    }
                } else if (myActivePlan->empty()) {
                    throw ProcessError("The start edge for person '" + pid + "' is not known.");
                }
                if (to == 0) {
                    const std::string toID = attrs.get<std::string>(SUMO_ATTR_TO, pid.c_str(), ok);
                    to = MSEdge::dictionary(toID);
                    if (to == 0) {
                        throw ProcessError("The to edge '" + toID + "' within a ride of person '" + pid + "' is not known.");
                    }
                }
                myActivePlan->push_back(new MSPerson::MSPersonStage_Driving(*to, bs, arrivalPos, st.getVector()));
                break;
            }
            case SUMO_TAG_TRANSPORT:
                try {
                    const std::string containerId = myVehicleParameter->id;
                    bool ok = true;
                    MSEdge* from = 0;
                    const std::string desc = attrs.get<std::string>(SUMO_ATTR_LINES, containerId.c_str(), ok);
                    StringTokenizer st(desc);
                    std::string csID = attrs.getOpt<std::string>(SUMO_ATTR_CONTAINER_STOP, 0, ok, "");
                    MSStoppingPlace* cs = 0;
                    if (csID != "") {
                        cs = MSNet::getInstance()->getStoppingPlace(csID, SUMO_TAG_CONTAINER_STOP);
                        if (cs == 0) {
                            throw ProcessError("Unknown container stop '" + csID + "' for container '" + myVehicleParameter->id + "'.");
                        }
                    }
                    double arrivalPos = attrs.getOpt<double>(SUMO_ATTR_ARRIVALPOS, myVehicleParameter->id.c_str(), ok,
                                        cs == 0 ? -NUMERICAL_EPS : cs->getEndLanePosition());
                    if (attrs.hasAttribute(SUMO_ATTR_FROM)) {
                        const std::string fromID = attrs.get<std::string>(SUMO_ATTR_FROM, containerId.c_str(), ok);
                        from = MSEdge::dictionary(fromID);
                        if (from == 0) {
                            throw ProcessError("The from edge '" + fromID + "' within a transport of container '" + containerId + "' is not known.");
                        }
                        if (!myActiveContainerPlan->empty() && &myActiveContainerPlan->back()->getDestination() != from) {
                            throw ProcessError("Disconnected plan for container '" + myVehicleParameter->id + "' (" + fromID + "!=" + myActiveContainerPlan->back()->getDestination().getID() + ").");
                        }
                        if (myActiveContainerPlan->empty()) {
                            myActiveContainerPlan->push_back(new MSTransportable::Stage_Waiting(
                                                                 *from, -1, myVehicleParameter->depart, myVehicleParameter->departPos, "start", true));
                        }
                    } else if (myActiveContainerPlan->empty()) {
                        throw ProcessError("The start edge within a transport of container '" + containerId + "' is not known.");
                    }
                    const std::string toID = attrs.get<std::string>(SUMO_ATTR_TO, containerId.c_str(), ok);
                    MSEdge* to = MSEdge::dictionary(toID);
                    if (to == 0) {
                        throw ProcessError("The to edge '" + toID + "' within a transport of container '" + containerId + "' is not known.");
                    }
                    myActiveContainerPlan->push_back(new MSContainer::MSContainerStage_Driving(*to, cs, arrivalPos, st.getVector()));

                } catch (ProcessError&) {
                    deleteActivePlans();
                    throw;
                }
                break;
            case SUMO_TAG_TRANSHIP: {
                myActiveRoute.clear();
                bool ok = true;
                double departPos = attrs.getOpt<double>(SUMO_ATTR_DEPARTPOS, myVehicleParameter->id.c_str(), ok, 0);
                double arrivalPos = attrs.getOpt<double>(SUMO_ATTR_ARRIVALPOS, myVehicleParameter->id.c_str(), ok, -NUMERICAL_EPS);
                double speed = DEFAULT_CONTAINER_TRANSHIP_SPEED;
                const MSVehicleType* vtype = MSNet::getInstance()->getVehicleControl().getVType(myVehicleParameter->vtypeid);
                // need to check for explicitly set speed since we might have // DEFAULT_VEHTYPE
                if (vtype != 0 && vtype->wasSet(VTYPEPARS_MAXSPEED_SET)) {
                    speed = vtype->getMaxSpeed();
                }
                speed = attrs.getOpt<double>(SUMO_ATTR_SPEED, 0, ok, speed);
                if (speed <= 0) {
                    throw ProcessError("Non-positive tranship speed for container  '" + myVehicleParameter->id + "'.");
                }
                std::string csID = attrs.getOpt<std::string>(SUMO_ATTR_CONTAINER_STOP, 0, ok, "");
                MSStoppingPlace* cs = 0;
                if (csID != "") {
                    cs = MSNet::getInstance()->getStoppingPlace(csID, SUMO_TAG_CONTAINER_STOP);
                    if (cs == 0) {
                        throw ProcessError("Unknown container stop '" + csID + "' for container '" + myVehicleParameter->id + "'.");
                    }
                    arrivalPos = cs->getEndLanePosition();
                }
                if (attrs.hasAttribute(SUMO_ATTR_EDGES)) {
                    MSEdge::parseEdgesList(attrs.get<std::string>(SUMO_ATTR_EDGES, myVehicleParameter->id.c_str(), ok), myActiveRoute, myActiveRouteID);
                } else {
                    if (attrs.hasAttribute(SUMO_ATTR_FROM) && attrs.hasAttribute(SUMO_ATTR_TO)) {
                        const std::string fromID = attrs.get<std::string>(SUMO_ATTR_FROM, myVehicleParameter->id.c_str(), ok);
                        MSEdge* from = MSEdge::dictionary(fromID);
                        if (from == 0) {
                            throw ProcessError("The from edge '" + fromID + "' within a tranship of container '" + myVehicleParameter->id + "' is not known.");
                        }
                        const std::string toID = attrs.get<std::string>(SUMO_ATTR_TO, myVehicleParameter->id.c_str(), ok);
                        MSEdge* to = MSEdge::dictionary(toID);
                        if (to == 0) {
                            throw ProcessError("The to edge '" + toID + "' within a tranship of container '" + myVehicleParameter->id + "' is not known.");
                        }
                        //the route of the container's tranship stage consists only of the 'from' and the 'to' edge
                        myActiveRoute.push_back(from);
                        myActiveRoute.push_back(to);
                        if (myActiveRoute.empty()) {
                            const std::string error = "No connection found between '" + from->getID() + "' and '" + to->getID() + "' for container '" + myVehicleParameter->id + "'.";
                            if (!MSGlobals::gCheckRoutes) {
                                myActiveRoute.push_back(from);
                            } else {
                                WRITE_ERROR(error);
                            }
                        }
                    }
                }
                if (myActiveRoute.empty()) {
                    throw ProcessError("No edges to tranship container '" + myVehicleParameter->id + "'.");
                }
                if (!myActiveContainerPlan->empty() && &myActiveContainerPlan->back()->getDestination() != myActiveRoute.front()) {
                    throw ProcessError("Disconnected plan for container '" + myVehicleParameter->id + "' (" + myActiveRoute.front()->getID() + "!=" + myActiveContainerPlan->back()->getDestination().getID() + ").");
                }
                if (myActiveContainerPlan->empty()) {
                    myActiveContainerPlan->push_back(new MSTransportable::Stage_Waiting(
                                                         *myActiveRoute.front(), -1, myVehicleParameter->depart, departPos, "start", true));
                }
                myActiveContainerPlan->push_back(new MSContainer::MSContainerStage_Tranship(myActiveRoute, cs, speed, departPos, arrivalPos));
                myActiveRoute.clear();
                break;
            }
            case SUMO_TAG_FLOW:
                parseFromViaTo("flow", attrs);
                break;
            case SUMO_TAG_TRIP:
                parseFromViaTo("trip", attrs);
                break;
            default:
                break;
        }
    } catch (ProcessError&) {
        delete myVehicleParameter;
        myVehicleParameter = 0;
        throw;
    }
}


void
MSRouteHandler::openVehicleTypeDistribution(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    myCurrentVTypeDistributionID = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    if (ok) {
        myCurrentVTypeDistribution = new RandomDistributor<MSVehicleType*>();
        if (attrs.hasAttribute(SUMO_ATTR_VTYPES)) {
            const std::string vTypes = attrs.get<std::string>(SUMO_ATTR_VTYPES, myCurrentVTypeDistributionID.c_str(), ok);
            StringTokenizer st(vTypes);
            while (st.hasNext()) {
                std::string vtypeID = st.next();
                MSVehicleType* type = MSNet::getInstance()->getVehicleControl().getVType(vtypeID, &myParsingRNG);
                if (type == 0) {
                    throw ProcessError("Unknown vtype '" + vtypeID + "' in distribution '" + myCurrentVTypeDistributionID + "'.");
                }
                myCurrentVTypeDistribution->add(type, type->getDefaultProbability());
            }
        }
    }
}


void
MSRouteHandler::closeVehicleTypeDistribution() {
    if (myCurrentVTypeDistribution != 0) {
        if (MSGlobals::gStateLoaded && MSNet::getInstance()->getVehicleControl().hasVTypeDistribution(myCurrentVTypeDistributionID)) {
            delete myCurrentVTypeDistribution;
            return;
        }
        if (myCurrentVTypeDistribution->getOverallProb() == 0) {
            delete myCurrentVTypeDistribution;
            throw ProcessError("Vehicle type distribution '" + myCurrentVTypeDistributionID + "' is empty.");
        }
        if (!MSNet::getInstance()->getVehicleControl().addVTypeDistribution(myCurrentVTypeDistributionID, myCurrentVTypeDistribution)) {
            delete myCurrentVTypeDistribution;
            throw ProcessError("Another vehicle type (or distribution) with the id '" + myCurrentVTypeDistributionID + "' exists.");
        }
        myCurrentVTypeDistribution = 0;
    }
}


void
MSRouteHandler::openRoute(const SUMOSAXAttributes& attrs) {
    myActiveRoute.clear();
    myInsertStopEdgesAt = -1;
    // check whether the id is really necessary
    std::string rid;
    if (myCurrentRouteDistribution != 0) {
        myActiveRouteID = myCurrentRouteDistributionID + "#" + toString(myCurrentRouteDistribution->getProbs().size()); // !!! document this
        rid =  "distribution '" + myCurrentRouteDistributionID + "'";
    } else if (myVehicleParameter != 0) {
        // ok, a vehicle is wrapping the route,
        //  we may use this vehicle's id as default
        myActiveRouteID = "!" + myVehicleParameter->id; // !!! document this
        if (attrs.hasAttribute(SUMO_ATTR_ID)) {
            WRITE_WARNING("Ids of internal routes are ignored (vehicle '" + myVehicleParameter->id + "').");
        }
    } else {
        bool ok = true;
        myActiveRouteID = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok, false);
        if (!ok) {
            return;
        }
        rid = "'" + myActiveRouteID + "'";
    }
    if (myVehicleParameter != 0) { // have to do this here for nested route distributions
        rid =  "for vehicle '" + myVehicleParameter->id + "'";
    }
    bool ok = true;
    if (attrs.hasAttribute(SUMO_ATTR_EDGES)) {
        MSEdge::parseEdgesList(attrs.get<std::string>(SUMO_ATTR_EDGES, myActiveRouteID.c_str(), ok), myActiveRoute, rid);
    }
    myActiveRouteRefID = attrs.getOpt<std::string>(SUMO_ATTR_REFID, myActiveRouteID.c_str(), ok, "");
    if (myActiveRouteRefID != "" && MSRoute::dictionary(myActiveRouteRefID, &myParsingRNG) == 0) {
        WRITE_ERROR("Invalid reference to route '" + myActiveRouteRefID + "' in route " + rid + ".");
    }
    myActiveRouteProbability = attrs.getOpt<double>(SUMO_ATTR_PROB, myActiveRouteID.c_str(), ok, DEFAULT_VEH_PROB);
    myActiveRouteColor = attrs.hasAttribute(SUMO_ATTR_COLOR) ? new RGBColor(attrs.get<RGBColor>(SUMO_ATTR_COLOR, myActiveRouteID.c_str(), ok)) : 0;
    myCurrentCosts = attrs.getOpt<double>(SUMO_ATTR_COST, myActiveRouteID.c_str(), ok, -1);
    if (ok && myCurrentCosts != -1 && myCurrentCosts < 0) {
        WRITE_ERROR("Invalid cost for route '" + myActiveRouteID + "'.");
    }
}


void
MSRouteHandler::myEndElement(int element) {
    SUMORouteHandler::myEndElement(element);
    switch (element) {
        case SUMO_TAG_VTYPE: {
            MSVehicleType* vehType = MSVehicleType::build(*myCurrentVType);
            delete myCurrentVType;
            myCurrentVType = 0;
            if (!MSNet::getInstance()->getVehicleControl().addVType(vehType)) {
                const std::string id = vehType->getID();
                delete vehType;
                if (!MSGlobals::gStateLoaded) {
                    throw ProcessError("Another vehicle type (or distribution) with the id '" + id + "' exists.");
                }
            } else {
                if (myCurrentVTypeDistribution != 0) {
                    myCurrentVTypeDistribution->add(vehType, vehType->getDefaultProbability());
                }
            }
        }
        break;
        case SUMO_TAG_TRIP:
            myVehicleParameter->parametersSet |= VEHPARS_FORCE_REROUTE;
            closeRoute(true);
            closeVehicle();
            delete myVehicleParameter;
            myVehicleParameter = 0;
            myInsertStopEdgesAt = -1;
            break;
        default:
            break;
    }
}


void
MSRouteHandler::closeRoute(const bool mayBeDisconnected) {
    std::string type = "vehicle";
    if (mayBeDisconnected) {
        if (myVehicleParameter->repetitionNumber >= 0) {
            type = "flow";
        } else {
            type = "trip";
        }
    }

    try {
        if (myActiveRoute.size() == 0) {
            delete myActiveRouteColor;
            myActiveRouteColor = 0;
            if (myActiveRouteRefID != "" && myCurrentRouteDistribution != 0) {
                const MSRoute* route = MSRoute::dictionary(myActiveRouteRefID, &myParsingRNG);
                if (route != 0) {
                    if (myCurrentRouteDistribution->add(route, myActiveRouteProbability)) {
                        route->addReference();
                    }
                }
                myActiveRouteID = "";
                myActiveRouteRefID = "";
                return;
            }
            if (myVehicleParameter != 0) {
                throw ProcessError("The route for " + type + " '" + myVehicleParameter->id + "' has no edges.");
            } else {
                throw ProcessError("Route '" + myActiveRouteID + "' has no edges.");
            }
        }
        if (myActiveRoute.size() == 1 && myActiveRoute.front()->isTazConnector()) {
            throw ProcessError("The routing information for " + type + " '" + myVehicleParameter->id + "' is insufficient.");
        }
        MSRoute* route = new MSRoute(myActiveRouteID, myActiveRoute,
                                     myVehicleParameter == 0 || myVehicleParameter->repetitionNumber >= 1,
                                     myActiveRouteColor, myActiveRouteStops);
        route->setCosts(myCurrentCosts);
        myActiveRoute.clear();
        if (!MSRoute::dictionary(myActiveRouteID, route)) {
            delete route;
            if (!MSGlobals::gStateLoaded) {
                if (myVehicleParameter != 0) {
                    if (MSNet::getInstance()->getVehicleControl().getVehicle(myVehicleParameter->id) == 0) {
                        throw ProcessError("Another route for " + type + " '" + myVehicleParameter->id + "' exists.");
                    } else {
                        throw ProcessError("A vehicle with id '" + myVehicleParameter->id + "' already exists.");
                    }
                } else {
                    throw ProcessError("Another route (or distribution) with the id '" + myActiveRouteID + "' exists.");
                }
            }
        } else {
            if (myCurrentRouteDistribution != 0) {
                if (myCurrentRouteDistribution->add(route, myActiveRouteProbability)) {
                    route->addReference();
                }
            }
        }
        myActiveRouteID = "";
        myActiveRouteColor = 0;
        myActiveRouteStops.clear();
    } catch (ProcessError&) {
        delete myVehicleParameter;
        throw;
    }
}


void
MSRouteHandler::openRouteDistribution(const SUMOSAXAttributes& attrs) {
    // check whether the id is really necessary
    bool ok = true;
    if (myVehicleParameter != 0) {
        // ok, a vehicle is wrapping the route,
        //  we may use this vehicle's id as default
        myCurrentRouteDistributionID = "!" + myVehicleParameter->id; // !!! document this
    } else {
        myCurrentRouteDistributionID = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
        if (!ok) {
            return;
        }
    }
    myCurrentRouteDistribution = new RandomDistributor<const MSRoute*>();
    std::vector<double> probs;
    if (attrs.hasAttribute(SUMO_ATTR_PROBS)) {
        bool ok = true;
        StringTokenizer st(attrs.get<std::string>(SUMO_ATTR_PROBS, myCurrentRouteDistributionID.c_str(), ok));
        while (st.hasNext()) {
            probs.push_back(TplConvert::_2doubleSec(st.next().c_str(), 1.0));
        }
    }
    if (attrs.hasAttribute(SUMO_ATTR_ROUTES)) {
        bool ok = true;
        StringTokenizer st(attrs.get<std::string>(SUMO_ATTR_ROUTES, myCurrentRouteDistributionID.c_str(), ok));
        int probIndex = 0;
        while (st.hasNext()) {
            std::string routeID = st.next();
            const MSRoute* route = MSRoute::dictionary(routeID, &myParsingRNG);
            if (route == 0) {
                throw ProcessError("Unknown route '" + routeID + "' in distribution '" + myCurrentRouteDistributionID + "'.");
            }
            const double prob = ((int)probs.size() > probIndex ? probs[probIndex] : 1.0);
            if (myCurrentRouteDistribution->add(route, prob, false)) {
                route->addReference();
            }
            probIndex++;
        }
        if (probs.size() > 0 && probIndex != (int)probs.size()) {
            WRITE_WARNING("Got " + toString(probs.size()) + " probabilities for " + toString(probIndex) +
                          " routes in routeDistribution '" + myCurrentRouteDistributionID + "'");
        }
    }
}


void
MSRouteHandler::closeRouteDistribution() {
    if (myCurrentRouteDistribution != 0) {
        const bool haveSameID = MSRoute::dictionary(myCurrentRouteDistributionID, &myParsingRNG) != 0;
        if (MSGlobals::gStateLoaded && haveSameID) {
            delete myCurrentRouteDistribution;
            return;
        }
        if (haveSameID) {
            delete myCurrentRouteDistribution;
            throw ProcessError("Another route (or distribution) with the id '" + myCurrentRouteDistributionID + "' exists.");
        }
        if (myCurrentRouteDistribution->getOverallProb() == 0) {
            delete myCurrentRouteDistribution;
            throw ProcessError("Route distribution '" + myCurrentRouteDistributionID + "' is empty.");
        }
        MSRoute::dictionary(myCurrentRouteDistributionID, myCurrentRouteDistribution, myVehicleParameter == 0);
        myCurrentRouteDistribution = 0;
    }
}


void
MSRouteHandler::closeVehicle() {
    // get nested route
    const MSRoute* route = MSRoute::dictionary("!" + myVehicleParameter->id, &myParsingRNG);
    MSVehicleControl& vehControl = MSNet::getInstance()->getVehicleControl();
    if (myVehicleParameter->departProcedure == DEPART_GIVEN) {
        // let's check whether this vehicle had to depart before the simulation starts
        if (!(myAddVehiclesDirectly || checkLastDepart()) || (myVehicleParameter->depart < string2time(OptionsCont::getOptions().getString("begin")) && !myAmLoadingState)) {
            if (route != 0) {
                route->addReference();
                route->release();
            }
            return;
        }
    }

    // get the vehicle's type
    MSVehicleType* vtype = 0;

    try {
        if (myVehicleParameter->vtypeid != "") {
            vtype = vehControl.getVType(myVehicleParameter->vtypeid, &myParsingRNG);
            if (vtype == 0) {
                throw ProcessError("The vehicle type '" + myVehicleParameter->vtypeid + "' for vehicle '" + myVehicleParameter->id + "' is not known.");
            }
            if (vtype->getVehicleClass() == SVC_PEDESTRIAN) {
                WRITE_WARNING("Vehicle type '" + vtype->getID() + "' with vClass=pedestrian should only be used for persons and not for vehicle '" + myVehicleParameter->id + "'.");
            }
        } else {
            // there should be one (at least the default one)
            vtype = vehControl.getVType(DEFAULT_VTYPE_ID, &myParsingRNG);
        }
        if (myVehicleParameter->wasSet(VEHPARS_ROUTE_SET)) {
            // if the route id was given, prefer that one
            if (route != 0 && !myAmLoadingState) {
                WRITE_WARNING("Ignoring child element 'route' for vehicle '" + myVehicleParameter->id + "' because attribute 'route' is set.");
            }
            route = MSRoute::dictionary(myVehicleParameter->routeid, &myParsingRNG);
        }
        if (route == 0) {
            // nothing found? -> error
            if (myVehicleParameter->routeid != "") {
                throw ProcessError("The route '" + myVehicleParameter->routeid + "' for vehicle '" + myVehicleParameter->id + "' is not known.");
            } else {
                throw ProcessError("Vehicle '" + myVehicleParameter->id + "' has no route.");
            }
        }
        myActiveRouteID = "";

    } catch (ProcessError&) {
        delete myVehicleParameter;
        throw;
    }

    // try to build the vehicle
    SUMOVehicle* vehicle = 0;
    if (vehControl.getVehicle(myVehicleParameter->id) == 0) {
        try {
            vehicle = vehControl.buildVehicle(myVehicleParameter, route, vtype, !MSGlobals::gCheckRoutes);
        } catch (const ProcessError& e) {
            if (!MSGlobals::gCheckRoutes) {
                WRITE_WARNING(e.what());
                vehControl.deleteVehicle(0, true);
                myVehicleParameter = 0;
                vehicle = 0;
                return;
            } else {
                throw e;
            }
        }
        // maybe we do not want this vehicle to be inserted due to scaling
        int quota = myAmLoadingState ? 1 : vehControl.getQuota();
        if (quota > 0) {
            vehControl.addVehicle(myVehicleParameter->id, vehicle);
            for (int i = 1; i < quota; i++) {
                MSNet::getInstance()->getInsertionControl().add(vehicle);
                SUMOVehicleParameter* newPars = new SUMOVehicleParameter(*myVehicleParameter);
                newPars->id = myVehicleParameter->id + "." + toString(i);
                vehicle = vehControl.buildVehicle(newPars, route, vtype, !MSGlobals::gCheckRoutes);
                vehControl.addVehicle(newPars->id, vehicle);
            }
            registerLastDepart();
            myVehicleParameter = 0;
        } else {
            vehControl.deleteVehicle(vehicle, true);
            myVehicleParameter = 0;
            vehicle = 0;
        }
    } else {
        // strange: another vehicle with the same id already exists
        if (!MSGlobals::gStateLoaded) {
            // and was not loaded while loading a simulation state
            // -> error
            std::string veh_id = myVehicleParameter->id;
            delete myVehicleParameter;
            myVehicleParameter = 0;
            throw ProcessError("Another vehicle with the id '" + veh_id + "' exists.");
        } else {
            // ok, it seems to be loaded previously while loading a simulation state
            vehicle = 0;
        }
    }
    // check whether the vehicle shall be added directly to the network or
    //  shall stay in the internal buffer
    if (vehicle != 0) {
        if (vehicle->getParameter().departProcedure == DEPART_GIVEN) {
            MSNet::getInstance()->getInsertionControl().add(vehicle);
        }
    }
}


void
MSRouteHandler::closePerson() {
    if (myActivePlan->size() == 0) {
        const std::string error = "Person '" + myVehicleParameter->id + "' has no plan.";
        delete myVehicleParameter;
        myVehicleParameter = 0;
        deleteActivePlans();
        throw ProcessError(error);
    }
    // let's check whether this person had to depart before the simulation starts
    if (!(myAddVehiclesDirectly || checkLastDepart())
            || (myVehicleParameter->depart < string2time(OptionsCont::getOptions().getString("begin")) && !myAmLoadingState)) {
        delete myVehicleParameter;
        myVehicleParameter = 0;
        deleteActivePlans();
        return;
    }
    // type existence has been checked on opening
    MSVehicleType* type = MSNet::getInstance()->getVehicleControl().getVType(myVehicleParameter->vtypeid, &myParsingRNG);
    MSTransportable* person = MSNet::getInstance()->getPersonControl().buildPerson(myVehicleParameter, type, myActivePlan, &myParsingRNG);
    // @todo: consider myScale?
    if (MSNet::getInstance()->getPersonControl().add(person)) {
        registerLastDepart();
    } else {
        ProcessError error("Another person with the id '" + myVehicleParameter->id + "' exists.");
        delete person;
        throw error;
    }
    myVehicleParameter = 0;
    myActivePlan = 0;
}

void
MSRouteHandler::closeContainer() {
    if (myActiveContainerPlan->size() == 0) {
        throw ProcessError("Container '" + myVehicleParameter->id + "' has no plan.");
    }
    MSVehicleType* type = MSNet::getInstance()->getVehicleControl().getVType(myVehicleParameter->vtypeid);
    if (type == 0) {
        throw ProcessError("The type '" + myVehicleParameter->vtypeid + "' for container '" + myVehicleParameter->id + "' is not known.");
    }
    MSTransportable* container = MSNet::getInstance()->getContainerControl().buildContainer(myVehicleParameter, type, myActiveContainerPlan);
    // @todo: consider myScale?
    if (myAddVehiclesDirectly || checkLastDepart()) {
        if (MSNet::getInstance()->getContainerControl().add(container)) {
            registerLastDepart();
        } else {
            ProcessError error("Another container with the id '" + myVehicleParameter->id + "' exists.");
            delete container;
            throw error;
        }
    } else {
        // warning already given
        delete container;
    }
    myVehicleParameter = 0;
    myActiveContainerPlan = 0;
}


void
MSRouteHandler::closeFlow() {
    myInsertStopEdgesAt = -1;
    if (myVehicleParameter->repetitionNumber == 0) {
        delete myVehicleParameter;
        myVehicleParameter = 0;
        return;
    }
    // let's check whether vehicles had to depart before the simulation starts
    myVehicleParameter->repetitionsDone = 0;
    if (myVehicleParameter->repetitionProbability < 0) {
        const SUMOTime offsetToBegin = string2time(OptionsCont::getOptions().getString("begin")) - myVehicleParameter->depart;
        while (myVehicleParameter->repetitionsDone * myVehicleParameter->repetitionOffset < offsetToBegin) {
            myVehicleParameter->repetitionsDone++;
            if (myVehicleParameter->repetitionsDone == myVehicleParameter->repetitionNumber) {
                delete myVehicleParameter;
                myVehicleParameter = 0;
                return;
            }
        }
    }
    if (MSNet::getInstance()->getVehicleControl().getVType(myVehicleParameter->vtypeid, &myParsingRNG) == 0) {
        throw ProcessError("The vehicle type '" + myVehicleParameter->vtypeid + "' for flow '" + myVehicleParameter->id + "' is not known.");
    }
    if (myVehicleParameter->routeid[0] == '!' && MSRoute::dictionary(myVehicleParameter->routeid, &myParsingRNG) == 0) {
        myVehicleParameter->parametersSet |= VEHPARS_FORCE_REROUTE;
        closeRoute(true);
    }
    if (MSRoute::dictionary(myVehicleParameter->routeid, &myParsingRNG) == 0) {
        throw ProcessError("The route '" + myVehicleParameter->routeid + "' for flow '" + myVehicleParameter->id + "' is not known.");
    }
    myActiveRouteID = "";

    // check whether the vehicle shall be added directly to the network or
    //  shall stay in the internal buffer
    if (myAddVehiclesDirectly || checkLastDepart()) {
        if (MSNet::getInstance()->getInsertionControl().add(myVehicleParameter)) {
            registerLastDepart();
        } else {
            throw ProcessError("Another flow with the id '" + myVehicleParameter->id + "' exists.");
        }
    }
    myVehicleParameter = 0;
}


void
MSRouteHandler::addStop(const SUMOSAXAttributes& attrs) {
    std::string errorSuffix;
    if (myActivePlan != 0) {
        errorSuffix = " in person '" + myVehicleParameter->id + "'.";
    } else if (myVehicleParameter != 0) {
        errorSuffix = " in vehicle '" + myVehicleParameter->id + "'.";
    } else if (myActiveContainerPlan != 0) {
        errorSuffix = " in container '" + myVehicleParameter->id + "'.";
    } else {
        errorSuffix = " in route '" + myActiveRouteID + "'.";
    }
    SUMOVehicleParameter::Stop stop;
    bool ok = parseStop(stop, attrs, errorSuffix, MsgHandler::getErrorInstance());
    if (!ok) {
        return;
    }
    const MSEdge* edge = 0;
    // try to parse the assigned bus stop
    if (stop.busstop != "") {
        // ok, we have a bus stop
        MSStoppingPlace* bs = MSNet::getInstance()->getStoppingPlace(stop.busstop, SUMO_TAG_BUS_STOP);
        if (bs == 0) {
            WRITE_ERROR("The busStop '" + stop.busstop + "' is not known" + errorSuffix);
            return;
        }
        const MSLane& l = bs->getLane();
        stop.lane = l.getID();
        stop.endPos = bs->getEndLanePosition();
        stop.startPos = bs->getBeginLanePosition();
        edge = &l.getEdge();
    } //try to parse the assigned container stop
    else if (stop.containerstop != "") {
        // ok, we have obviously a container stop
        MSStoppingPlace* cs = MSNet::getInstance()->getStoppingPlace(stop.containerstop, SUMO_TAG_CONTAINER_STOP);
        if (cs == 0) {
            WRITE_ERROR("The containerStop '" + stop.containerstop + "' is not known" + errorSuffix);
            return;
        }
        const MSLane& l = cs->getLane();
        stop.lane = l.getID();
        stop.endPos = cs->getEndLanePosition();
        stop.startPos = cs->getBeginLanePosition();
        edge = &l.getEdge();
    } //try to parse the assigned parking area
    else if (stop.parkingarea != "") {
        // ok, we have obviously a parking area
        MSStoppingPlace* pa = MSNet::getInstance()->getStoppingPlace(stop.parkingarea, SUMO_TAG_PARKING_AREA);
        if (pa == 0) {
            WRITE_ERROR("The parkingArea '" + stop.parkingarea + "' is not known" + errorSuffix);
            return;
        }
        const MSLane& l = pa->getLane();
        stop.lane = l.getID();
        stop.endPos = pa->getEndLanePosition();
        stop.startPos = pa->getBeginLanePosition();
        edge = &l.getEdge();
    } else if (stop.chargingStation != "") {
        // ok, we have a charging station
        MSStoppingPlace* cs = MSNet::getInstance()->getStoppingPlace(stop.chargingStation, SUMO_TAG_CHARGING_STATION);
        if (cs != 0) {
            const MSLane& l = cs->getLane();
            stop.lane = l.getID();
            stop.endPos = cs->getEndLanePosition();
            stop.startPos = cs->getBeginLanePosition();
        } else {
            WRITE_ERROR("The chargingStation '" + stop.chargingStation + "' is not known" + errorSuffix);
            return;
        }
    } else {
        // no, the lane and the position should be given
        // get the lane
        stop.lane = attrs.getOpt<std::string>(SUMO_ATTR_LANE, 0, ok, "");
        if (ok && stop.lane != "") {
            if (MSLane::dictionary(stop.lane) == 0) {
                WRITE_ERROR("The lane '" + stop.lane + "' for a stop is not known" + errorSuffix);
                return;
            }
        } else {
            if (myActivePlan && !myActivePlan->empty()) {
                const MSStoppingPlace* bs = myActivePlan->back()->getDestinationStop();
                if (bs != 0) {
                    edge = &bs->getLane().getEdge();
                    stop.lane = bs->getLane().getID();
                    stop.endPos = bs->getEndLanePosition();
                    stop.startPos = bs->getBeginLanePosition();
                } else {
                    edge = &myActivePlan->back()->getDestination();
                    stop.lane = edge->getLanes()[0]->getID();
                    stop.endPos = myActivePlan->back()->getArrivalPos();
                    stop.startPos = stop.endPos - POSITION_EPS;
                }
            } else {
                WRITE_ERROR("A stop must be placed on a busStop, a chargingStation, a containerStop a parkingArea or a lane" + errorSuffix);
                return;
            }
        }
        edge = &MSLane::dictionary(stop.lane)->getEdge();
        if (myActivePlan &&
                !myActivePlan->empty() &&
                &myActivePlan->back()->getDestination() != edge) {
            throw ProcessError("Disconnected plan for person '" + myVehicleParameter->id + "' (" + edge->getID() + "!=" + myActivePlan->back()->getDestination().getID() + ").");
        }
        if (myActivePlan && myActivePlan->empty()) {
            myActivePlan->push_back(new MSTransportable::Stage_Waiting(
                                        *edge, -1, myVehicleParameter->depart, myVehicleParameter->departPos, "start", true));
        }
        if (myActiveContainerPlan &&
                !myActiveContainerPlan->empty() &&
                &myActiveContainerPlan->back()->getDestination() != &MSLane::dictionary(stop.lane)->getEdge()) {
            throw ProcessError("Disconnected plan for container '" + myVehicleParameter->id + "' (" + MSLane::dictionary(stop.lane)->getEdge().getID() + "!=" + myActiveContainerPlan->back()->getDestination().getID() + ").");
        }
        if (myActiveContainerPlan && myActiveContainerPlan->empty()) {
            myActiveContainerPlan->push_back(new MSTransportable::Stage_Waiting(
                                                 MSLane::dictionary(stop.lane)->getEdge(), -1, myVehicleParameter->depart, myVehicleParameter->departPos, "start", true));
        }
        stop.endPos = attrs.getOpt<double>(SUMO_ATTR_ENDPOS, 0, ok, MSLane::dictionary(stop.lane)->getLength());
        if (attrs.hasAttribute(SUMO_ATTR_POSITION)) {
            WRITE_WARNING("Deprecated attribute 'pos' in description of stop" + errorSuffix);
            stop.endPos = attrs.getOpt<double>(SUMO_ATTR_POSITION, 0, ok, stop.endPos);
        }
        stop.startPos = attrs.getOpt<double>(SUMO_ATTR_STARTPOS, 0, ok, MAX2(0., stop.endPos - 2 * POSITION_EPS));
        const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, 0, ok, false);
        if (!ok || !checkStopPos(stop.startPos, stop.endPos, MSLane::dictionary(stop.lane)->getLength(), POSITION_EPS, friendlyPos)) {
            WRITE_ERROR("Invalid start or end position for stop on lane '" + stop.lane + "'" + errorSuffix);
            return;
        }
    }
    if (myActivePlan != 0) {
        std::string actType = attrs.getOpt<std::string>(SUMO_ATTR_ACTTYPE, 0, ok, "waiting");
        double pos = (stop.startPos + stop.endPos) / 2.;
        if (!myActivePlan->empty()) {
            pos = myActivePlan->back()->getArrivalPos();
        }
        myActivePlan->push_back(new MSTransportable::Stage_Waiting(
                                    MSLane::dictionary(stop.lane)->getEdge(), stop.duration, stop.until, pos, actType, false));
    } else if (myActiveContainerPlan != 0) {
        std::string actType = attrs.getOpt<std::string>(SUMO_ATTR_ACTTYPE, 0, ok, "waiting");
        myActiveContainerPlan->push_back(new MSTransportable::Stage_Waiting(
                                             MSLane::dictionary(stop.lane)->getEdge(), stop.duration, stop.until, stop.startPos, actType, false));
    } else if (myVehicleParameter != 0) {
        myVehicleParameter->stops.push_back(stop);
    } else {
        myActiveRouteStops.push_back(stop);
    }
    if (myInsertStopEdgesAt >= 0) {
        myActiveRoute.insert(myActiveRoute.begin() + myInsertStopEdgesAt, edge);
        myInsertStopEdgesAt++;
    }
}


void
MSRouteHandler::parseWalkPositions(const SUMOSAXAttributes& attrs, const std::string& personID,
                                   const MSEdge* fromEdge, const MSEdge*& toEdge,
                                   double& departPos, double& arrivalPos, MSStoppingPlace*& bs,
                                   const MSTransportable::Stage* const lastStage, bool& ok) {
    const std::string description = "person '" + personID + "' walking from " + fromEdge->getID();

    if (attrs.hasAttribute(SUMO_ATTR_DEPARTPOS)) {
        WRITE_WARNING("The attribute departPos is no longer supported for walks, please use the person attribute, the arrivalPos of the previous step or explicit stops.");
    }
    departPos = 0.;
    if (lastStage->getDestinationStop() != nullptr) {
        departPos = lastStage->getDestinationStop()->getAccessPos(fromEdge);
    } else if (&lastStage->getDestination() == fromEdge) {
        departPos = lastStage->getArrivalPos();
    } else if (lastStage->getDestination().getToJunction() == fromEdge->getToJunction()) {
        departPos = fromEdge->getLength();
    }

    std::string bsID = attrs.getOpt<std::string>(SUMO_ATTR_BUS_STOP, 0, ok, "");
    if (bsID != "") {
        bs = MSNet::getInstance()->getStoppingPlace(bsID, SUMO_TAG_BUS_STOP);
        if (bs == 0) {
            throw ProcessError("Unknown bus stop '" + bsID + "' for " + description + ".");
        }
        if (toEdge == 0) {
            toEdge = &bs->getLane().getEdge();
        }
        arrivalPos = bs->getAccessPos(toEdge);
        if (arrivalPos < 0) {
            throw ProcessError("Bus stop '" + bsID + "' is not connected to arrival edge '" + toEdge->getID() + "' for " + description + ".");
        }
        if (attrs.hasAttribute(SUMO_ATTR_ARRIVALPOS)) {
            const double arrPos = SUMOVehicleParserHelper::parseWalkPos(SUMO_ATTR_ARRIVALPOS, description, toEdge->getLength(),
                                  attrs.get<std::string>(SUMO_ATTR_ARRIVALPOS, description.c_str(), ok), &myParsingRNG);
            if (arrPos >= bs->getBeginLanePosition() && arrPos < bs->getEndLanePosition()) {
                arrivalPos = arrPos;
            } else {
                WRITE_WARNING("Ignoring arrivalPos for " + description + " because it is outside the given stop '" + toString(SUMO_ATTR_BUS_STOP) + "'.");
            }
        }
    } else {
        if (toEdge == 0) {
            throw ProcessError("No destination edge for " + description + ".");
        }
        if (attrs.hasAttribute(SUMO_ATTR_ARRIVALPOS)) {
            arrivalPos = SUMOVehicleParserHelper::parseWalkPos(SUMO_ATTR_ARRIVALPOS, description, toEdge->getLength(),
                         attrs.get<std::string>(SUMO_ATTR_ARRIVALPOS, description.c_str(), ok), &myParsingRNG);
        } else {
            arrivalPos = 0.;
        }
    }
}


void
MSRouteHandler::addPersonTrip(const SUMOSAXAttributes& attrs) {
    myActiveRoute.clear();
    bool ok = true;
    MSVehicleControl& vehControl = MSNet::getInstance()->getVehicleControl();
    const char* const id = myVehicleParameter->id.c_str();
    const SUMOTime duration = attrs.getOptSUMOTimeReporting(SUMO_ATTR_DURATION, id, ok, -1);
    if (attrs.hasAttribute(SUMO_ATTR_DURATION) && duration <= 0) {
        throw ProcessError("Non-positive walking duration for  '" + myVehicleParameter->id + "'.");
    }
    const MSVehicleType* pedType = vehControl.getVType(myVehicleParameter->vtypeid, &myParsingRNG);
    const std::string fromID = attrs.getOpt<std::string>(SUMO_ATTR_FROM, id, ok, "");
    const MSEdge* from = fromID != "" || myActivePlan->empty() ? MSEdge::dictionary(fromID) : &myActivePlan->back()->getDestination();
    if (from == 0) {
        throw ProcessError("The from edge '" + fromID + "' within a walk of person '" + myVehicleParameter->id + "' is not known.");
    }
    const std::string toID = attrs.getOpt<std::string>(SUMO_ATTR_TO, myVehicleParameter->id.c_str(), ok, "");
    const MSEdge* to = MSEdge::dictionary(toID);
    if (toID != "" && to == 0) {
        throw ProcessError("The to edge '" + toID + "' within a walk of person '" + myVehicleParameter->id + "' is not known.");
    }
    double departPos = 0;
    double arrivalPos = 0;
    MSStoppingPlace* bs = 0;
    if (myActivePlan->empty()) {
        double initialDepartPos = myVehicleParameter->departPos;
        if (myVehicleParameter->departPosProcedure == DEPART_POS_RANDOM) {
            initialDepartPos = RandHelper::rand(from->getLength(), &myParsingRNG);
        }
        myActivePlan->push_back(new MSTransportable::Stage_Waiting(*from, -1, myVehicleParameter->depart, initialDepartPos, "start", true));
    }
    parseWalkPositions(attrs, myVehicleParameter->id, from, to, departPos, arrivalPos, bs, myActivePlan->back(), ok);

    const std::string modes = attrs.getOpt<std::string>(SUMO_ATTR_MODES, id, ok, "");
    SVCPermissions modeSet = 0;
    for (StringTokenizer st(modes); st.hasNext();) {
        const std::string mode = st.next();
        if (mode == "car") {
            modeSet |= SVC_PASSENGER;
        } else if (mode == "bicycle") {
            modeSet |= SVC_BICYCLE;
        } else if (mode == "public") {
            modeSet |= SVC_BUS;
        } else {
            throw InvalidArgument("Unknown person mode '" + mode + "'.");
        }
    }
    const std::string types = attrs.getOpt<std::string>(SUMO_ATTR_VTYPES, id, ok, "");
    std::vector<SUMOVehicleParameter*> pars;
    for (StringTokenizer st(types); st.hasNext();) {
        pars.push_back(new SUMOVehicleParameter());
        pars.back()->vtypeid = st.next();
        pars.back()->parametersSet |= VEHPARS_VTYPE_SET;
        pars.back()->departProcedure = DEPART_TRIGGERED;
        MSVehicleType* type = vehControl.getVType(pars.back()->vtypeid);
        if (type == 0) {
            throw InvalidArgument("The vehicle type '" + pars.back()->vtypeid + "' in a trip for person '" + myVehicleParameter->id + "' is not known.");
        }
        pars.back()->id = myVehicleParameter->id + "_" + toString(pars.size() - 1);
        modeSet |= SVC_PASSENGER;
    }
    if (pars.empty()) {
        if ((modeSet & SVC_PASSENGER) != 0) {
            pars.push_back(new SUMOVehicleParameter());
            pars.back()->id = myVehicleParameter->id + "_0";
            pars.back()->departProcedure = DEPART_TRIGGERED;
        } else if ((modeSet & SVC_BUS) != 0) {
            pars.push_back(0);
        }
    }

    const double speed = attrs.getOpt<double>(SUMO_ATTR_SPEED, id, ok, -1.);
    if (attrs.hasAttribute(SUMO_ATTR_SPEED) && speed <= 0) {
        throw ProcessError("Non-positive walking speed for  '" + myVehicleParameter->id + "'.");
    }
    const double walkFactor = attrs.getOpt<double>(SUMO_ATTR_WALKFACTOR, id, ok, OptionsCont::getOptions().getFloat("persontrip.walkfactor"));
    const double departPosLat = attrs.getOpt<double>(SUMO_ATTR_DEPARTPOS_LAT, 0, ok, 0);
    if (ok) {
        const std::string error = "No connection found between '" + from->getID() + "' and '" + to->getID() + "' for person '" + myVehicleParameter->id + "'.";
        if (modeSet == 0) {
            MSNet::getInstance()->getPedestrianRouter().compute(from, to, departPos, arrivalPos, speed > 0 ? speed : pedType->getMaxSpeed(), 0, 0, myActiveRoute);
            if (myActiveRoute.empty()) {
                if (!MSGlobals::gCheckRoutes) {
                    myActiveRoute.push_back(from);
                    myActiveRoute.push_back(to); // pedestrian will teleport
                } else {
                    throw ProcessError(error);
                }
            }
            myActivePlan->push_back(new MSPerson::MSPersonStage_Walking(myVehicleParameter->id, myActiveRoute, bs, duration, speed, departPos, arrivalPos, departPosLat));
        } else {
            for (SUMOVehicleParameter* vehPar : pars) {
                SUMOVehicle* vehicle = 0;
                if (vehPar != 0) {
                    MSVehicleType* type = MSNet::getInstance()->getVehicleControl().getVType(vehPar->vtypeid);
                    if (type->getVehicleClass() != SVC_IGNORING && (from->getPermissions() & type->getVehicleClass()) == 0) {
                        WRITE_WARNING("Ignoring vehicle type '" + type->getID() + "' when routing person '" + myVehicleParameter->id + "' because it is not allowed on the start edge.");
                    } else {
                        const MSRoute* const routeDummy = new MSRoute(vehPar->id, ConstMSEdgeVector({ from }), false, 0, std::vector<SUMOVehicleParameter::Stop>());
                        vehicle = vehControl.buildVehicle(vehPar, routeDummy, type, !MSGlobals::gCheckRoutes);
                    }
                }
                bool carUsed = false;
                std::vector<MSNet::MSIntermodalRouter::TripItem> result;
                if (MSNet::getInstance()->getIntermodalRouter().compute(from, to, departPos, arrivalPos,
                        pedType->getMaxSpeed() * walkFactor, vehicle, modeSet, myVehicleParameter->depart, result)) {
                    for (std::vector<MSNet::MSIntermodalRouter::TripItem>::iterator it = result.begin(); it != result.end(); ++it) {
                        if (!it->edges.empty()) {
                            bs = MSNet::getInstance()->getStoppingPlace(it->destStop, SUMO_TAG_BUS_STOP);
                            if (it->line == "") {
                                const double depPos = myActivePlan->back()->getDestinationStop() != 0 ? myActivePlan->back()->getDestinationStop()->getAccessPos(it->edges.front()) : departPos;
                                myActivePlan->push_back(new MSPerson::MSPersonStage_Walking(myVehicleParameter->id, it->edges, bs, duration, speed, depPos, bs != 0 ? bs->getAccessPos(it->edges.back()) : arrivalPos, departPosLat));
                            } else if (vehicle != 0 && it->line == vehicle->getID()) {
                                double vehicleArrivalPos = bs != 0 ? bs->getAccessPos(it->edges.back()) : it->edges.back()->getLength();
                                if (it + 1 == result.end()) {
                                    vehicleArrivalPos = arrivalPos;
                                }
                                myActivePlan->push_back(new MSPerson::MSPersonStage_Driving(*it->edges.back(), bs, vehicleArrivalPos, std::vector<std::string>({ it->line })));
                                vehicle->replaceRouteEdges(it->edges, true);
                                vehicle->setArrivalPos(vehicleArrivalPos);
                                vehControl.addVehicle(vehPar->id, vehicle);
                                carUsed = true;
                            } else {
                                myActivePlan->push_back(new MSPerson::MSPersonStage_Driving(*it->edges.back(), bs, bs != 0 ? bs->getAccessPos(it->edges.back()) : arrivalPos, std::vector<std::string>({ it->line })));
                            }
                        }
                    }
                } else {
                    if (MSGlobals::gCheckRoutes) {
                        throw ProcessError(error);
                    }
                }
                if (vehicle != 0 && !carUsed) {
                    vehControl.deleteVehicle(vehicle, true);
                }
            }
        }
    }
    myActiveRoute.clear();
}


void
MSRouteHandler::addWalk(const SUMOSAXAttributes& attrs) {
    try {
        myActiveRoute.clear();
        bool ok = true;
        const SUMOTime duration = attrs.getOptSUMOTimeReporting(SUMO_ATTR_DURATION, 0, ok, -1);
        if (attrs.hasAttribute(SUMO_ATTR_DURATION) && duration <= 0) {
            throw ProcessError("Non-positive walking duration for  '" + myVehicleParameter->id + "'.");
        }
        double speed = -1; // default to vType speed
        if (attrs.hasAttribute(SUMO_ATTR_SPEED)) {
            speed = attrs.get<double>(SUMO_ATTR_SPEED, 0, ok);
            if (speed <= 0) {
                throw ProcessError("Non-positive walking speed for  '" + myVehicleParameter->id + "'.");
            }
        }
        double departPos = 0;
        double arrivalPos = 0;
        MSStoppingPlace* bs = 0;
        if (attrs.hasAttribute(SUMO_ATTR_ROUTE)) {
            const std::string routeID = attrs.get<std::string>(SUMO_ATTR_ROUTE, myVehicleParameter->id.c_str(), ok);
            const MSRoute* route = MSRoute::dictionary(routeID, &myParsingRNG);
            if (route == 0) {
                throw ProcessError("The route '" + routeID + "' for walk of person '" + myVehicleParameter->id + "' is not known.");
            }
            myActiveRoute = route->getEdges();
        } else {
            MSEdge::parseEdgesList(attrs.get<std::string>(SUMO_ATTR_EDGES, myVehicleParameter->id.c_str(), ok), myActiveRoute, myActiveRouteID);
        }
        if (myActivePlan->empty()) {
            double initialDepartPos = myVehicleParameter->departPos;
            if (myVehicleParameter->departPosProcedure == DEPART_POS_RANDOM) {
                initialDepartPos = RandHelper::rand(myActiveRoute.front()->getLength(), &myParsingRNG);
            }
            myActivePlan->push_back(new MSTransportable::Stage_Waiting(*myActiveRoute.front(), -1, myVehicleParameter->depart, initialDepartPos, "start", true));
        }
        parseWalkPositions(attrs, myVehicleParameter->id, myActiveRoute.front(), myActiveRoute.back(), departPos, arrivalPos, bs, myActivePlan->back(), ok);
        if (myActiveRoute.empty()) {
            throw ProcessError("No edges to walk for person '" + myVehicleParameter->id + "'.");
        }
        if (&myActivePlan->back()->getDestination() != myActiveRoute.front() &&
                myActivePlan->back()->getDestination().getToJunction() != myActiveRoute.front()->getFromJunction() &&
                myActivePlan->back()->getDestination().getToJunction() != myActiveRoute.front()->getToJunction()) {
            if (myActivePlan->back()->getDestinationStop() == 0 || !myActivePlan->back()->getDestinationStop()->hasAccess(myActiveRoute.front())) {
                throw ProcessError("Disconnected plan for person '" + myVehicleParameter->id + "' (" + myActiveRoute.front()->getID() + " not connected to " + myActivePlan->back()->getDestination().getID() + ").");
            }
        }
        const double departPosLat = attrs.getOpt<double>(SUMO_ATTR_DEPARTPOS_LAT, 0, ok, 0);
        myActivePlan->push_back(new MSPerson::MSPersonStage_Walking(myVehicleParameter->id, myActiveRoute, bs, duration, speed, departPos, arrivalPos, departPosLat));
        myActiveRoute.clear();
    } catch (ProcessError&) {
        deleteActivePlans();
        throw;
    }
}


/****************************************************************************/
