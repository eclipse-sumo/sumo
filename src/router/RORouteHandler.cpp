/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    RORouteHandler.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Mon, 9 Jul 2001
///
// Parser and container for routes during their loading
/****************************************************************************/
#include <config.h>

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <utils/iodevices/OutputDevice.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/UtilExceptions.h>
#include <utils/options/OptionsCont.h>
#include <utils/vehicle/SUMOVehicleParserHelper.h>
#include <utils/xml/SUMOSAXReader.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/iodevices/OutputDevice_String.h>
#include "RONet.h"
#include "ROEdge.h"
#include "ROLane.h"
#include "RORouteDef.h"
#include "RORouteHandler.h"

// ===========================================================================
// method definitions
// ===========================================================================
RORouteHandler::RORouteHandler(RONet& net, const std::string& file,
                               const bool tryRepair,
                               const bool emptyDestinationsAllowed,
                               const bool ignoreErrors,
                               const bool checkSchema) :
    SUMORouteHandler(file, checkSchema ? "routes" : "", true),
    MapMatcher(OptionsCont::getOptions().getBool("mapmatch.junctions"),
               OptionsCont::getOptions().getFloat("mapmatch.distance"),
               ignoreErrors ? MsgHandler::getWarningInstance() : MsgHandler::getErrorInstance()),
    myNet(net),
    myActiveRouteRepeat(0),
    myActiveRoutePeriod(0),
    myActivePlan(nullptr),
    myActiveContainerPlan(nullptr),
    myActiveContainerPlanSize(0),
    myTryRepair(tryRepair),
    myEmptyDestinationsAllowed(emptyDestinationsAllowed),
    myErrorOutput(ignoreErrors ? MsgHandler::getWarningInstance() : MsgHandler::getErrorInstance()),
    myBegin(string2time(OptionsCont::getOptions().getString("begin"))),
    myKeepVTypeDist(OptionsCont::getOptions().getBool("keep-vtype-distributions")),
    myUnsortedInput(OptionsCont::getOptions().exists("unsorted-input") && OptionsCont::getOptions().getBool("unsorted-input")),
    myCurrentVTypeDistribution(nullptr),
    myCurrentAlternatives(nullptr) {
    myActiveRoute.reserve(100);
}


RORouteHandler::~RORouteHandler() {
    delete myCurrentAlternatives;
}


void
RORouteHandler::deleteActivePlanAndVehicleParameter() {
    if (myActivePlan != nullptr) {
        for (ROPerson::PlanItem* const it : *myActivePlan) {
            delete it;
        }
        delete myActivePlan;
        myActivePlan = nullptr;
    }
    delete myActiveContainerPlan;
    myActiveContainerPlan = nullptr;
    delete myVehicleParameter;
    myVehicleParameter = nullptr;
}


void
RORouteHandler::parseFromViaTo(SumoXMLTag tag, const SUMOSAXAttributes& attrs, bool& ok) {
    const std::string element = toString(tag);
    myActiveRoute.clear();
    bool useTaz = OptionsCont::getOptions().getBool("with-taz");
    if (useTaz && !myVehicleParameter->wasSet(VEHPARS_FROM_TAZ_SET) && !myVehicleParameter->wasSet(VEHPARS_TO_TAZ_SET)) {
        WRITE_WARNINGF(TL("Taz usage was requested but no taz present in % '%'!"), element, myVehicleParameter->id);
        useTaz = false;
    }
    SUMOVehicleClass vClass = SVC_PASSENGER;
    if (!myNet.getVTypeDistribution(myVehicleParameter->vtypeid)) {
        SUMOVTypeParameter* type = myNet.getVehicleTypeSecure(myVehicleParameter->vtypeid);
        if (type != nullptr) {
            vClass = type->vehicleClass;
        }
    }
    // from-attributes
    const std::string rid = "for " + element + " '" + myVehicleParameter->id + "'";
    if ((useTaz || !attrs.hasAttribute(SUMO_ATTR_FROM)) &&
            (attrs.hasAttribute(SUMO_ATTR_FROM_TAZ) || attrs.hasAttribute(SUMO_ATTR_FROM_JUNCTION))) {
        const bool useJunction = attrs.hasAttribute(SUMO_ATTR_FROM_JUNCTION);
        const std::string tazType = useJunction ? "junction" : "taz";
        const std::string tazID = attrs.get<std::string>(useJunction ? SUMO_ATTR_FROM_JUNCTION : SUMO_ATTR_FROM_TAZ, myVehicleParameter->id.c_str(), ok, true);
        const ROEdge* fromTaz = myNet.getEdge(tazID + "-source");
        if (fromTaz == nullptr) {
            myErrorOutput->inform("Source " + tazType + " '" + tazID + "' not known for " + element + " '" + myVehicleParameter->id + "'!"
                                  + (useJunction ? JUNCTION_TAZ_MISSING_HELP : ""));
            ok = false;
        } else if (fromTaz->getNumSuccessors() == 0 && tag != SUMO_TAG_PERSON) {
            myErrorOutput->inform("Source " + tazType + " '" + tazID + "' has no outgoing edges for " + element + " '" + myVehicleParameter->id + "'!");
            ok = false;
        } else {
            myActiveRoute.push_back(fromTaz);
        }
    } else if (attrs.hasAttribute(SUMO_ATTR_FROMXY)) {
        parseGeoEdges(attrs.get<PositionVector>(SUMO_ATTR_FROMXY, myVehicleParameter->id.c_str(), ok), false, vClass, myActiveRoute, rid, true, ok);
    } else if (attrs.hasAttribute(SUMO_ATTR_FROMLONLAT)) {
        parseGeoEdges(attrs.get<PositionVector>(SUMO_ATTR_FROMLONLAT, myVehicleParameter->id.c_str(), ok), true, vClass, myActiveRoute, rid, true, ok);
    } else {
        parseEdges(attrs.getOpt<std::string>(SUMO_ATTR_FROM, myVehicleParameter->id.c_str(), ok), myActiveRoute, rid, ok);
    }
    if (!attrs.hasAttribute(SUMO_ATTR_VIA) && !attrs.hasAttribute(SUMO_ATTR_VIALONLAT) && !attrs.hasAttribute(SUMO_ATTR_VIAXY)) {
        myInsertStopEdgesAt = (int)myActiveRoute.size();
    }

    // via-attributes
    ConstROEdgeVector viaEdges;
    if (attrs.hasAttribute(SUMO_ATTR_VIAXY)) {
        parseGeoEdges(attrs.get<PositionVector>(SUMO_ATTR_VIAXY, myVehicleParameter->id.c_str(), ok), false, vClass, viaEdges, rid, false, ok);
    } else if (attrs.hasAttribute(SUMO_ATTR_VIALONLAT)) {
        parseGeoEdges(attrs.get<PositionVector>(SUMO_ATTR_VIALONLAT, myVehicleParameter->id.c_str(), ok), true, vClass, viaEdges, rid, false, ok);
    } else if (attrs.hasAttribute(SUMO_ATTR_VIAJUNCTIONS)) {
        for (std::string junctionID : attrs.get<std::vector<std::string> >(SUMO_ATTR_VIAJUNCTIONS, myVehicleParameter->id.c_str(), ok)) {
            const ROEdge* viaSink = myNet.getEdge(junctionID + "-sink");
            if (viaSink == nullptr) {
                myErrorOutput->inform("Junction-taz '" + junctionID + "' not found." + JUNCTION_TAZ_MISSING_HELP);
                ok = false;
            } else {
                viaEdges.push_back(viaSink);
            }
        }
    } else {
        parseEdges(attrs.getOpt<std::string>(SUMO_ATTR_VIA, myVehicleParameter->id.c_str(), ok, "", true), viaEdges, rid, ok);
    }
    for (const ROEdge* e : viaEdges) {
        myActiveRoute.push_back(e);
        myVehicleParameter->via.push_back(e->getID());
    }

    // to-attributes
    if ((useTaz || !attrs.hasAttribute(SUMO_ATTR_TO)) &&
            (attrs.hasAttribute(SUMO_ATTR_TO_TAZ) || attrs.hasAttribute(SUMO_ATTR_TO_JUNCTION))) {
        const bool useJunction = attrs.hasAttribute(SUMO_ATTR_TO_JUNCTION);
        const std::string tazType = useJunction ? "junction" : "taz";
        const std::string tazID = attrs.get<std::string>(useJunction ? SUMO_ATTR_TO_JUNCTION : SUMO_ATTR_TO_TAZ, myVehicleParameter->id.c_str(), ok, true);
        const ROEdge* toTaz = myNet.getEdge(tazID + "-sink");
        if (toTaz == nullptr) {
            myErrorOutput->inform("Sink " + tazType + " '" + tazID + "' not known for " + element + " '" + myVehicleParameter->id + "'!"
                                  + (useJunction ? JUNCTION_TAZ_MISSING_HELP : ""));
            ok = false;
        } else if (toTaz->getNumPredecessors() == 0 && tag != SUMO_TAG_PERSON) {
            myErrorOutput->inform("Sink " + tazType + " '" + tazID + "' has no incoming edges for " + element + " '" + myVehicleParameter->id + "'!");
            ok = false;
        } else {
            myActiveRoute.push_back(toTaz);
        }
    } else if (attrs.hasAttribute(SUMO_ATTR_TOXY)) {
        parseGeoEdges(attrs.get<PositionVector>(SUMO_ATTR_TOXY, myVehicleParameter->id.c_str(), ok, true), false, vClass, myActiveRoute, rid, false, ok);
    } else if (attrs.hasAttribute(SUMO_ATTR_TOLONLAT)) {
        parseGeoEdges(attrs.get<PositionVector>(SUMO_ATTR_TOLONLAT, myVehicleParameter->id.c_str(), ok, true), true, vClass, myActiveRoute, rid, false, ok);
    } else {
        parseEdges(attrs.getOpt<std::string>(SUMO_ATTR_TO, myVehicleParameter->id.c_str(), ok, "", true), myActiveRoute, rid, ok);
    }
    myActiveRouteID = "!" + myVehicleParameter->id;
    if (myVehicleParameter->routeid == "") {
        myVehicleParameter->routeid = myActiveRouteID;
    }
}


void
RORouteHandler::myStartElement(int element,
                               const SUMOSAXAttributes& attrs) {
    try {
        if (myActivePlan != nullptr && myActivePlan->empty() && myVehicleParameter->departProcedure == DepartDefinition::TRIGGERED && element != SUMO_TAG_RIDE && element != SUMO_TAG_PARAM) {
            throw ProcessError(TLF("Triggered departure for person '%' requires starting with a ride.", myVehicleParameter->id));
        } else if (myActiveContainerPlan != nullptr && myActiveContainerPlanSize == 0 && myVehicleParameter->departProcedure == DepartDefinition::TRIGGERED && element != SUMO_TAG_TRANSPORT && element != SUMO_TAG_PARAM) {
            throw ProcessError(TLF("Triggered departure for container '%' requires starting with a transport.", myVehicleParameter->id));
        }
        SUMORouteHandler::myStartElement(element, attrs);
        bool ok = true;
        switch (element) {
            case SUMO_TAG_PERSON:
            case SUMO_TAG_PERSONFLOW: {
                myActivePlan = new std::vector<ROPerson::PlanItem*>();
                break;
            }
            case SUMO_TAG_RIDE:
                break; // handled in addRide, called from SUMORouteHandler::myStartElement
            case SUMO_TAG_CONTAINER:
            case SUMO_TAG_CONTAINERFLOW:
                myActiveContainerPlan = new OutputDevice_String(1);
                myActiveContainerPlanSize = 0;
                myActiveContainerPlan->openTag((SumoXMLTag)element);
                (*myActiveContainerPlan) << attrs;
                break;
            case SUMO_TAG_TRANSPORT:
            case SUMO_TAG_TRANSHIP:
                if (myActiveContainerPlan == nullptr) {
                    throw ProcessError(TLF("Found % outside container element", toString((SumoXMLTag)element)));
                }
                // copy container elements
                myActiveContainerPlan->openTag((SumoXMLTag)element);
                (*myActiveContainerPlan) << attrs;
                myActiveContainerPlan->closeTag();
                myActiveContainerPlanSize++;
                break;
            case SUMO_TAG_FLOW:
                myActiveRouteProbability = DEFAULT_VEH_PROB;
                parseFromViaTo((SumoXMLTag)element, attrs, ok);
                break;
            case SUMO_TAG_TRIP:
                myActiveRouteProbability = DEFAULT_VEH_PROB;
                parseFromViaTo((SumoXMLTag)element, attrs, ok);
                break;
            default:
                break;
        }
    } catch (ProcessError&) {
        deleteActivePlanAndVehicleParameter();
        throw;
    }
}


void
RORouteHandler::openVehicleTypeDistribution(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    myCurrentVTypeDistributionID = attrs.get<std::string>(SUMO_ATTR_ID, nullptr, ok);
    if (ok) {
        myCurrentVTypeDistribution = new RandomDistributor<SUMOVTypeParameter*>();
        if (attrs.hasAttribute(SUMO_ATTR_VTYPES)) {
            std::vector<double> probs;
            if (attrs.hasAttribute(SUMO_ATTR_PROBS)) {
                StringTokenizer st(attrs.get<std::string>(SUMO_ATTR_PROBS, myCurrentVTypeDistributionID.c_str(), ok));
                while (st.hasNext()) {
                    probs.push_back(StringUtils::toDoubleSecure(st.next(), 1.0));
                }
            }
            const std::string vTypes = attrs.get<std::string>(SUMO_ATTR_VTYPES, myCurrentVTypeDistributionID.c_str(), ok);
            StringTokenizer st(vTypes);
            int probIndex = 0;
            while (st.hasNext()) {
                const std::string typeID = st.next();
                const RandomDistributor<SUMOVTypeParameter*>* const dist = myNet.getVTypeDistribution(typeID);
                if (dist != nullptr) {
                    const double distProb = ((int)probs.size() > probIndex ? probs[probIndex] : 1.) / dist->getOverallProb();
                    std::vector<double>::const_iterator probIt = dist->getProbs().begin();
                    for (SUMOVTypeParameter* const type : dist->getVals()) {
                        myCurrentVTypeDistribution->add(type, distProb * *probIt);
                        probIt++;
                    }
                } else {
                    SUMOVTypeParameter* const type = myNet.getVehicleTypeSecure(typeID);
                    if (type == nullptr) {
                        myErrorOutput->inform("Unknown vehicle type '" + typeID + "' in distribution '" + myCurrentVTypeDistributionID + "'.");
                    } else {
                        const double prob = ((int)probs.size() > probIndex ? probs[probIndex] : type->defaultProbability);
                        myCurrentVTypeDistribution->add(type, prob);
                    }
                }
                probIndex++;
            }
            if (probs.size() > 0 && probIndex != (int)probs.size()) {
                WRITE_WARNING("Got " + toString(probs.size()) + " probabilities for " + toString(probIndex) +
                              " types in vTypeDistribution '" + myCurrentVTypeDistributionID + "'");
            }
        }
    }
}


void
RORouteHandler::closeVehicleTypeDistribution() {
    if (myCurrentVTypeDistribution != nullptr) {
        if (myCurrentVTypeDistribution->getOverallProb() == 0) {
            delete myCurrentVTypeDistribution;
            myErrorOutput->inform("Vehicle type distribution '" + myCurrentVTypeDistributionID + "' is empty.");
        } else if (!myNet.addVTypeDistribution(myCurrentVTypeDistributionID, myCurrentVTypeDistribution)) {
            delete myCurrentVTypeDistribution;
            myErrorOutput->inform("Another vehicle type (or distribution) with the id '" + myCurrentVTypeDistributionID + "' exists.");
        }
        myCurrentVTypeDistribution = nullptr;
    }
}


void
RORouteHandler::openRoute(const SUMOSAXAttributes& attrs) {
    myActiveRoute.clear();
    myInsertStopEdgesAt = -1;
    // check whether the id is really necessary
    std::string rid;
    if (myCurrentAlternatives != nullptr) {
        myActiveRouteID = myCurrentAlternatives->getID();
        rid =  "distribution '" + myCurrentAlternatives->getID() + "'";
    } else if (myVehicleParameter != nullptr) {
        // ok, a vehicle is wrapping the route,
        //  we may use this vehicle's id as default
        myVehicleParameter->routeid = myActiveRouteID = "!" + myVehicleParameter->id; // !!! document this
        if (attrs.hasAttribute(SUMO_ATTR_ID)) {
            WRITE_WARNINGF(TL("Ids of internal routes are ignored (vehicle '%')."), myVehicleParameter->id);
        }
    } else {
        bool ok = true;
        myActiveRouteID = attrs.get<std::string>(SUMO_ATTR_ID, nullptr, ok);
        if (!ok) {
            return;
        }
        rid = "'" + myActiveRouteID + "'";
    }
    if (myVehicleParameter != nullptr) { // have to do this here for nested route distributions
        rid =  "for vehicle '" + myVehicleParameter->id + "'";
    }
    bool ok = true;
    if (attrs.hasAttribute(SUMO_ATTR_EDGES)) {
        parseEdges(attrs.get<std::string>(SUMO_ATTR_EDGES, myActiveRouteID.c_str(), ok), myActiveRoute, rid, ok);
    }
    myActiveRouteRefID = attrs.getOpt<std::string>(SUMO_ATTR_REFID, myActiveRouteID.c_str(), ok, "");
    if (myActiveRouteRefID != "" && myNet.getRouteDef(myActiveRouteRefID) == nullptr) {
        myErrorOutput->inform("Invalid reference to route '" + myActiveRouteRefID + "' in route " + rid + ".");
    }
    if (myCurrentAlternatives != nullptr && !attrs.hasAttribute(SUMO_ATTR_PROB)) {
        WRITE_WARNINGF(TL("No probability for route %, using default."), rid);
    }
    myActiveRouteProbability = attrs.getOpt<double>(SUMO_ATTR_PROB, myActiveRouteID.c_str(), ok, DEFAULT_VEH_PROB);
    if (ok && myActiveRouteProbability < 0) {
        myErrorOutput->inform("Invalid probability for route '" + myActiveRouteID + "'.");
    }
    myActiveRouteColor = attrs.hasAttribute(SUMO_ATTR_COLOR) ? new RGBColor(attrs.get<RGBColor>(SUMO_ATTR_COLOR, myActiveRouteID.c_str(), ok)) : nullptr;
    ok = true;
    myActiveRouteRepeat = attrs.getOpt<int>(SUMO_ATTR_REPEAT, myActiveRouteID.c_str(), ok, 0);
    myActiveRoutePeriod = attrs.getOptSUMOTimeReporting(SUMO_ATTR_CYCLETIME, myActiveRouteID.c_str(), ok, 0);
    if (myActiveRouteRepeat > 0) {
        SUMOVehicleClass vClass = SVC_IGNORING;
        if (myVehicleParameter != nullptr) {
            SUMOVTypeParameter* type = myNet.getVehicleTypeSecure(myVehicleParameter->vtypeid);
            if (type != nullptr) {
                vClass = type->vehicleClass;
            }
        }
        if (myActiveRoute.size() > 0 && !myActiveRoute.back()->isConnectedTo(*myActiveRoute.front(), vClass)) {
            myErrorOutput->inform("Disconnected route " + rid + " when repeating.");
        }
    }
    myCurrentCosts = attrs.getOpt<double>(SUMO_ATTR_COST, myActiveRouteID.c_str(), ok, -1);
    if (ok && myCurrentCosts != -1 && myCurrentCosts < 0) {
        myErrorOutput->inform("Invalid cost for route '" + myActiveRouteID + "'.");
    }
}


void
RORouteHandler::openFlow(const SUMOSAXAttributes& /*attrs*/) {
    // currently unused
}


void
RORouteHandler::openRouteFlow(const SUMOSAXAttributes& /*attrs*/) {
    // currently unused
}


void
RORouteHandler::openTrip(const SUMOSAXAttributes& /*attrs*/) {
    // currently unused
}


void
RORouteHandler::closeRoute(const bool mayBeDisconnected) {
    const bool mustReroute = myActiveRoute.size() == 0 && myActiveRouteStops.size() != 0;
    if (mustReroute) {
        // implicit route from stops
        for (const SUMOVehicleParameter::Stop& stop : myActiveRouteStops) {
            ROEdge* edge = myNet.getEdge(stop.edge);
            myActiveRoute.push_back(edge);
        }
    }
    if (myActiveRoute.size() == 0) {
        if (myActiveRouteRefID != "" && myCurrentAlternatives != nullptr) {
            myCurrentAlternatives->addAlternativeDef(myNet.getRouteDef(myActiveRouteRefID));
            myActiveRouteID = "";
            myActiveRouteRefID = "";
            return;
        }
        if (myVehicleParameter != nullptr) {
            myErrorOutput->inform("The route for vehicle '" + myVehicleParameter->id + "' has no edges.");
        } else {
            myErrorOutput->inform("Route '" + myActiveRouteID + "' has no edges.");
        }
        myActiveRouteID = "";
        myActiveRouteStops.clear();
        return;
    }
    if (myActiveRoute.size() == 1 && myActiveRoute.front()->isTazConnector()) {
        myErrorOutput->inform("The routing information for vehicle '" + myVehicleParameter->id + "' is insufficient.");
        myActiveRouteID = "";
        myActiveRouteStops.clear();
        return;
    }
    if (!mayBeDisconnected && OptionsCont::getOptions().exists("no-internal-links") && !OptionsCont::getOptions().getBool("no-internal-links")) {
        // fix internal edges which did not get parsed
        const ROEdge* last = nullptr;
        ConstROEdgeVector fullRoute;
        for (const ROEdge* roe : myActiveRoute) {
            if (last != nullptr) {
                for (const ROEdge* intern : last->getSuccessors()) {
                    if (intern->isInternal() && intern->getSuccessors().size() == 1 && intern->getSuccessors().front() == roe) {
                        fullRoute.push_back(intern);
                    }
                }
            }
            fullRoute.push_back(roe);
            last = roe;
        }
        myActiveRoute = fullRoute;
    }
    if (myActiveRouteRepeat > 0) {
        // duplicate route
        ConstROEdgeVector tmpEdges = myActiveRoute;
        auto tmpStops = myActiveRouteStops;
        for (int i = 0; i < myActiveRouteRepeat; i++) {
            myActiveRoute.insert(myActiveRoute.begin(), tmpEdges.begin(), tmpEdges.end());
            for (SUMOVehicleParameter::Stop stop : tmpStops) {
                if (stop.until > 0) {
                    if (myActiveRoutePeriod <= 0) {
                        const std::string description = myVehicleParameter != nullptr
                                                        ?  "for vehicle '" + myVehicleParameter->id + "'"
                                                        :  "'" + myActiveRouteID + "'";
                        throw ProcessError(TLF("Cannot repeat stops with 'until' in route % because no cycleTime is defined.", description));
                    }
                    stop.until += myActiveRoutePeriod * (i + 1);
                    stop.arrival += myActiveRoutePeriod * (i + 1);
                }
                myActiveRouteStops.push_back(stop);
            }
        }
    }
    RORoute* route = new RORoute(myActiveRouteID, myCurrentCosts, myActiveRouteProbability, myActiveRoute,
                                 myActiveRouteColor, myActiveRouteStops);
    myActiveRoute.clear();
    if (myCurrentAlternatives == nullptr) {
        if (myNet.getRouteDef(myActiveRouteID) != nullptr) {
            delete route;
            if (myVehicleParameter != nullptr) {
                myErrorOutput->inform("Another route for vehicle '" + myVehicleParameter->id + "' exists.");
            } else {
                myErrorOutput->inform("Another route (or distribution) with the id '" + myActiveRouteID + "' exists.");
            }
            myActiveRouteID = "";
            myActiveRouteStops.clear();
            return;
        } else {
            myCurrentAlternatives = new RORouteDef(myActiveRouteID, 0, mayBeDisconnected || myTryRepair, mayBeDisconnected);
            myCurrentAlternatives->addLoadedAlternative(route);
            myNet.addRouteDef(myCurrentAlternatives);
            myCurrentAlternatives = nullptr;
        }
    } else {
        myCurrentAlternatives->addLoadedAlternative(route);
    }
    myActiveRouteID = "";
    myActiveRouteStops.clear();
}


void
RORouteHandler::openRouteDistribution(const SUMOSAXAttributes& attrs) {
    // check whether the id is really necessary
    bool ok = true;
    std::string id;
    if (myVehicleParameter != nullptr) {
        // ok, a vehicle is wrapping the route,
        //  we may use this vehicle's id as default
        myVehicleParameter->routeid = id = "!" + myVehicleParameter->id; // !!! document this
        if (attrs.hasAttribute(SUMO_ATTR_ID)) {
            WRITE_WARNINGF(TL("Ids of internal route distributions are ignored (vehicle '%')."), myVehicleParameter->id);
        }
    } else {
        id = attrs.get<std::string>(SUMO_ATTR_ID, nullptr, ok);
        if (!ok) {
            return;
        }
    }
    // try to get the index of the last element
    int index = attrs.getOpt<int>(SUMO_ATTR_LAST, id.c_str(), ok, 0);
    if (ok && index < 0) {
        myErrorOutput->inform("Negative index of a route alternative (id='" + id + "').");
        return;
    }
    // build the alternative cont
    myCurrentAlternatives = new RORouteDef(id, index, myTryRepair, false);
    if (attrs.hasAttribute(SUMO_ATTR_ROUTES)) {
        ok = true;
        StringTokenizer st(attrs.get<std::string>(SUMO_ATTR_ROUTES, id.c_str(), ok));
        while (st.hasNext()) {
            const std::string routeID = st.next();
            const RORouteDef* route = myNet.getRouteDef(routeID);
            if (route == nullptr) {
                myErrorOutput->inform("Unknown route '" + routeID + "' in distribution '" + id + "'.");
            } else {
                myCurrentAlternatives->addAlternativeDef(route);
            }
        }
    }
}


void
RORouteHandler::closeRouteDistribution() {
    if (myCurrentAlternatives != nullptr) {
        if (myCurrentAlternatives->getOverallProb() == 0) {
            myErrorOutput->inform("Route distribution '" + myCurrentAlternatives->getID() + "' is empty.");
            delete myCurrentAlternatives;
        } else if (!myNet.addRouteDef(myCurrentAlternatives)) {
            myErrorOutput->inform("Another route (or distribution) with the id '" + myCurrentAlternatives->getID() + "' exists.");
            delete myCurrentAlternatives;
        }
        myCurrentAlternatives = nullptr;
    }
}


void
RORouteHandler::closeVehicle() {
    checkLastDepart();
    // get the vehicle id
    if (myVehicleParameter->departProcedure == DepartDefinition::GIVEN && myVehicleParameter->depart < myBegin) {
        return;
    }
    // get vehicle type
    SUMOVTypeParameter* type = myNet.getVehicleTypeSecure(myVehicleParameter->vtypeid);
    if (type == nullptr) {
        myErrorOutput->inform("The vehicle type '" + myVehicleParameter->vtypeid + "' for vehicle '" + myVehicleParameter->id + "' is not known.");
        type = myNet.getVehicleTypeSecure(DEFAULT_VTYPE_ID);
    } else {
        if (!myKeepVTypeDist) {
            // fix the type id in case we used a distribution
            myVehicleParameter->vtypeid = type->id;
        }
    }
    if (type->vehicleClass == SVC_PEDESTRIAN) {
        WRITE_WARNINGF(TL("Vehicle type '%' with vClass=pedestrian should only be used for persons and not for vehicle '%'."), type->id, myVehicleParameter->id);
    }
    // get the route
    RORouteDef* route = myNet.getRouteDef(myVehicleParameter->routeid);
    if (route == nullptr) {
        myErrorOutput->inform("The route of the vehicle '" + myVehicleParameter->id + "' is not known.");
        return;
    }
    if (MsgHandler::getErrorInstance()->wasInformed()) {
        return;
    }
    const bool needCopy = route->getID()[0] != '!';
    if (needCopy) {
        route = route->copy("!" + myVehicleParameter->id, myVehicleParameter->depart);
    }
    // build the vehicle
    ROVehicle* veh = new ROVehicle(*myVehicleParameter, route, type, &myNet, myErrorOutput);
    if (myNet.addVehicle(myVehicleParameter->id, veh)) {
        registerLastDepart();
    } else if (needCopy) {
        delete route;
    }
    delete myVehicleParameter;
    myVehicleParameter = nullptr;
}


void
RORouteHandler::closeVType() {
    if (myNet.addVehicleType(myCurrentVType)) {
        if (myCurrentVTypeDistribution != nullptr) {
            myCurrentVTypeDistribution->add(myCurrentVType, myCurrentVType->defaultProbability);
        }
    }
    if (OptionsCont::getOptions().isSet("restriction-params")) {
        const std::vector<std::string> paramKeys = OptionsCont::getOptions().getStringVector("restriction-params");
        myCurrentVType->cacheParamRestrictions(paramKeys);
    }
    myCurrentVType = nullptr;
}


void
RORouteHandler::closePerson() {
    SUMOVTypeParameter* type = myNet.getVehicleTypeSecure(myVehicleParameter->vtypeid);
    if (type == nullptr) {
        myErrorOutput->inform("The vehicle type '" + myVehicleParameter->vtypeid + "' for person '" + myVehicleParameter->id + "' is not known.");
        type = myNet.getVehicleTypeSecure(DEFAULT_PEDTYPE_ID);
    }
    if (myActivePlan == nullptr || myActivePlan->empty()) {
        WRITE_WARNINGF(TL("Discarding person '%' because her plan is empty"), myVehicleParameter->id);
    } else {
        ROPerson* person = new ROPerson(*myVehicleParameter, type);
        for (ROPerson::PlanItem* item : *myActivePlan) {
            person->getPlan().push_back(item);
        }
        if (myNet.addPerson(person)) {
            checkLastDepart();
            registerLastDepart();
        }
    }
    delete myVehicleParameter;
    myVehicleParameter = nullptr;
    delete myActivePlan;
    myActivePlan = nullptr;
}


void
RORouteHandler::closePersonFlow() {
    std::string typeID = DEFAULT_PEDTYPE_ID;
    if (myNet.getVehicleTypeSecure(myVehicleParameter->vtypeid) == nullptr) {
        myErrorOutput->inform("The vehicle type '" + myVehicleParameter->vtypeid + "' for personFlow '" + myVehicleParameter->id + "' is not known.");
    } else {
        typeID = myVehicleParameter->vtypeid;
    }
    if (myActivePlan == nullptr || myActivePlan->empty()) {
        WRITE_WARNINGF(TL("Discarding personFlow '%' because their plan is empty"), myVehicleParameter->id);
    } else {
        checkLastDepart();
        // instantiate all persons of this flow
        int i = 0;
        std::string baseID = myVehicleParameter->id;
        if (myVehicleParameter->repetitionProbability > 0) {
            if (myVehicleParameter->repetitionEnd == SUMOTime_MAX) {
                throw ProcessError(TLF("probabilistic personFlow '%' must specify end time", myVehicleParameter->id));
            } else {
                for (SUMOTime t = myVehicleParameter->depart; t < myVehicleParameter->repetitionEnd; t += TIME2STEPS(1)) {
                    if (RandHelper::rand() < myVehicleParameter->repetitionProbability) {
                        addFlowPerson(typeID, t, baseID, i++);
                    }
                }
            }
        } else {
            SUMOTime depart = myVehicleParameter->depart;
            // uniform sampling of departures from range is equivalent to poisson flow (encoded by negative offset)
            if (OptionsCont::getOptions().getBool("randomize-flows") && myVehicleParameter->repetitionOffset >= 0) {
                std::vector<SUMOTime> departures;
                const SUMOTime range = myVehicleParameter->repetitionNumber * myVehicleParameter->repetitionOffset;
                for (int j = 0; j < myVehicleParameter->repetitionNumber; ++j) {
                    departures.push_back(depart + RandHelper::rand(range));
                }
                std::sort(departures.begin(), departures.end());
                std::reverse(departures.begin(), departures.end());
                for (; i < myVehicleParameter->repetitionNumber; i++) {
                    addFlowPerson(typeID, departures[i], baseID, i);
                    depart += myVehicleParameter->repetitionOffset;
                }
            } else {
                const bool triggered = myVehicleParameter->departProcedure == DepartDefinition::TRIGGERED;
                if (myVehicleParameter->repetitionOffset < 0) {
                    // poisson: randomize first depart
                    myVehicleParameter->incrementFlow(1);
                }
                for (; i < myVehicleParameter->repetitionNumber && (triggered || depart + myVehicleParameter->repetitionTotalOffset <= myVehicleParameter->repetitionEnd); i++) {
                    addFlowPerson(typeID, depart + myVehicleParameter->repetitionTotalOffset, baseID, i);
                    if (myVehicleParameter->departProcedure != DepartDefinition::TRIGGERED) {
                        myVehicleParameter->incrementFlow(1);
                    }
                }
            }
        }
    }
    delete myVehicleParameter;
    myVehicleParameter = nullptr;
    for (ROPerson::PlanItem* const it : *myActivePlan) {
        delete it;
    }
    delete myActivePlan;
    myActivePlan = nullptr;
}


void
RORouteHandler::addFlowPerson(const std::string& typeID, SUMOTime depart, const std::string& baseID, int i) {
    SUMOVehicleParameter pars = *myVehicleParameter;
    pars.id = baseID + "." + toString(i);
    pars.depart = depart;
    const SUMOVTypeParameter* const type = myNet.getVehicleTypeSecure(typeID);
    if (!myKeepVTypeDist) {
        pars.vtypeid = type->id;
    }
    ROPerson* person = new ROPerson(pars, type);
    for (ROPerson::PlanItem* item : *myActivePlan) {
        person->getPlan().push_back(item->clone());
    }
    if (myNet.addPerson(person)) {
        if (i == 0) {
            registerLastDepart();
        }
    }
}


void
RORouteHandler::closeContainer() {
    myActiveContainerPlan->closeTag();
    if (myActiveContainerPlanSize > 0) {
        myNet.addContainer(myVehicleParameter->depart, myActiveContainerPlan->getString());
        checkLastDepart();
        registerLastDepart();
    } else {
        WRITE_WARNINGF(TL("Discarding container '%' because its plan is empty"), myVehicleParameter->id);
    }
    delete myVehicleParameter;
    myVehicleParameter = nullptr;
    delete myActiveContainerPlan;
    myActiveContainerPlan = nullptr;
    myActiveContainerPlanSize = 0;
}


void RORouteHandler::closeContainerFlow() {
    myActiveContainerPlan->closeTag();
    if (myActiveContainerPlanSize > 0) {
        myNet.addContainer(myVehicleParameter->depart, myActiveContainerPlan->getString());
        checkLastDepart();
        registerLastDepart();
    } else {
        WRITE_WARNINGF(TL("Discarding containerFlow '%' because its plan is empty"), myVehicleParameter->id);
    }
    delete myVehicleParameter;
    myVehicleParameter = nullptr;
    delete myActiveContainerPlan;
    myActiveContainerPlan = nullptr;
    myActiveContainerPlanSize = 0;
}


void
RORouteHandler::closeFlow() {
    checkLastDepart();
    // @todo: consider myScale?
    if (myVehicleParameter->repetitionNumber == 0) {
        delete myVehicleParameter;
        myVehicleParameter = nullptr;
        return;
    }
    // let's check whether vehicles had to depart before the simulation starts
    myVehicleParameter->repetitionsDone = 0;
    const SUMOTime offsetToBegin = myBegin - myVehicleParameter->depart;
    while (myVehicleParameter->repetitionTotalOffset < offsetToBegin) {
        myVehicleParameter->incrementFlow(1);
        if (myVehicleParameter->repetitionsDone == myVehicleParameter->repetitionNumber) {
            delete myVehicleParameter;
            myVehicleParameter = nullptr;
            return;
        }
    }
    if (myNet.getVehicleTypeSecure(myVehicleParameter->vtypeid) == nullptr) {
        myErrorOutput->inform("The vehicle type '" + myVehicleParameter->vtypeid + "' for flow '" + myVehicleParameter->id + "' is not known.");
    }
    if (myVehicleParameter->routeid[0] == '!' && myNet.getRouteDef(myVehicleParameter->routeid) == nullptr) {
        closeRoute(true);
    }
    if (myNet.getRouteDef(myVehicleParameter->routeid) == nullptr) {
        myErrorOutput->inform("The route '" + myVehicleParameter->routeid + "' for flow '" + myVehicleParameter->id + "' is not known.");
        delete myVehicleParameter;
        myVehicleParameter = nullptr;
        return;
    }
    myActiveRouteID = "";
    if (!MsgHandler::getErrorInstance()->wasInformed()) {
        if (myNet.addFlow(myVehicleParameter, OptionsCont::getOptions().getBool("randomize-flows"))) {
            registerLastDepart();
        } else {
            myErrorOutput->inform("Another flow with the id '" + myVehicleParameter->id + "' exists.");
            delete myVehicleParameter;
        }
    } else {
        delete myVehicleParameter;
    }
    myVehicleParameter = nullptr;
    myInsertStopEdgesAt = -1;
}


void
RORouteHandler::closeTrip() {
    closeRoute(true);
    closeVehicle();
}


const SUMOVehicleParameter::Stop*
RORouteHandler::retrieveStoppingPlace(const SUMOSAXAttributes& attrs, const std::string& errorSuffix, std::string& id, const SUMOVehicleParameter::Stop* stopParam) {
    // dummy stop parameter to hold the attributes
    SUMOVehicleParameter::Stop stop;
    if (stopParam != nullptr) {
        stop = *stopParam;
    } else {
        bool ok = true;
        stop.busstop = attrs.getOpt<std::string>(SUMO_ATTR_BUS_STOP, nullptr, ok, "");
        stop.busstop = attrs.getOpt<std::string>(SUMO_ATTR_TRAIN_STOP, nullptr, ok, stop.busstop); // alias
        stop.chargingStation = attrs.getOpt<std::string>(SUMO_ATTR_CHARGING_STATION, nullptr, ok, "");
        stop.overheadWireSegment = attrs.getOpt<std::string>(SUMO_ATTR_OVERHEAD_WIRE_SEGMENT, nullptr, ok, "");
        stop.containerstop = attrs.getOpt<std::string>(SUMO_ATTR_CONTAINER_STOP, nullptr, ok, "");
        stop.parkingarea = attrs.getOpt<std::string>(SUMO_ATTR_PARKING_AREA, nullptr, ok, "");
    }
    const SUMOVehicleParameter::Stop* toStop = nullptr;
    if (stop.busstop != "") {
        toStop = myNet.getStoppingPlace(stop.busstop, SUMO_TAG_BUS_STOP);
        id = stop.busstop;
        if (toStop == nullptr) {
            WRITE_ERROR("The busStop '" + stop.busstop + "' is not known" + errorSuffix);
        }
    } else if (stop.containerstop != "") {
        toStop = myNet.getStoppingPlace(stop.containerstop, SUMO_TAG_CONTAINER_STOP);
        id = stop.containerstop;
        if (toStop == nullptr) {
            WRITE_ERROR("The containerStop '" + stop.containerstop + "' is not known" + errorSuffix);
        }
    } else if (stop.parkingarea != "") {
        toStop = myNet.getStoppingPlace(stop.parkingarea, SUMO_TAG_PARKING_AREA);
        id = stop.parkingarea;
        if (toStop == nullptr) {
            WRITE_ERROR("The parkingArea '" + stop.parkingarea + "' is not known" + errorSuffix);
        }
    } else if (stop.chargingStation != "") {
        // ok, we have a charging station
        toStop = myNet.getStoppingPlace(stop.chargingStation, SUMO_TAG_CHARGING_STATION);
        id = stop.chargingStation;
        if (toStop == nullptr) {
            WRITE_ERROR("The chargingStation '" + stop.chargingStation + "' is not known" + errorSuffix);
        }
    } else if (stop.overheadWireSegment != "") {
        // ok, we have an overhead wire segment
        toStop = myNet.getStoppingPlace(stop.overheadWireSegment, SUMO_TAG_OVERHEAD_WIRE_SEGMENT);
        id = stop.overheadWireSegment;
        if (toStop == nullptr) {
            WRITE_ERROR("The overhead wire segment '" + stop.overheadWireSegment + "' is not known" + errorSuffix);
        }
    }
    return toStop;
}

Parameterised*
RORouteHandler::addStop(const SUMOSAXAttributes& attrs) {
    Parameterised* result = nullptr;
    if (myActiveContainerPlan != nullptr) {
        myActiveContainerPlan->openTag(SUMO_TAG_STOP);
        (*myActiveContainerPlan) << attrs;
        myActiveContainerPlan->closeTag();
        myActiveContainerPlanSize++;
        return result;
    }
    std::string errorSuffix;
    if (myActivePlan != nullptr) {
        errorSuffix = " in person '" + myVehicleParameter->id + "'.";
    } else if (myActiveContainerPlan != nullptr) {
        errorSuffix = " in container '" + myVehicleParameter->id + "'.";
    } else if (myVehicleParameter != nullptr) {
        errorSuffix = " in vehicle '" + myVehicleParameter->id + "'.";
    } else {
        errorSuffix = " in route '" + myActiveRouteID + "'.";
    }
    SUMOVehicleParameter::Stop stop;
    bool ok = parseStop(stop, attrs, errorSuffix, myErrorOutput);
    if (!ok) {
        return result;
    }
    // try to parse the assigned bus stop
    const ROEdge* edge = nullptr;
    std::string stoppingPlaceID;
    const SUMOVehicleParameter::Stop* stoppingPlace = retrieveStoppingPlace(attrs, errorSuffix, stoppingPlaceID, &stop);
    bool hasPos = false;
    if (stoppingPlace != nullptr) {
        stop.lane = stoppingPlace->lane;
        stop.endPos = stoppingPlace->endPos;
        stop.startPos = stoppingPlace->startPos;
        edge = myNet.getEdge(SUMOXMLDefinitions::getEdgeIDFromLane(stop.lane));
    } else {
        // no, the lane and the position should be given
        stop.lane = attrs.getOpt<std::string>(SUMO_ATTR_LANE, nullptr, ok, "");
        stop.edge = attrs.getOpt<std::string>(SUMO_ATTR_EDGE, nullptr, ok, "");
        if (ok && stop.edge != "") {
            edge = myNet.getEdge(stop.edge);
            if (edge == nullptr) {
                myErrorOutput->inform("The edge '" + stop.edge + "' for a stop is not known" + errorSuffix);
                return result;
            }
        } else if (ok && stop.lane != "") {
            edge = myNet.getEdge(SUMOXMLDefinitions::getEdgeIDFromLane(stop.lane));
            if (edge == nullptr) {
                myErrorOutput->inform("The lane '" + stop.lane + "' for a stop is not known" + errorSuffix);
                return result;
            }
        } else if (ok && ((attrs.hasAttribute(SUMO_ATTR_X) && attrs.hasAttribute(SUMO_ATTR_Y))
                          || (attrs.hasAttribute(SUMO_ATTR_LON) && attrs.hasAttribute(SUMO_ATTR_LAT)))) {
            Position pos;
            bool geo = false;
            if (attrs.hasAttribute(SUMO_ATTR_X) && attrs.hasAttribute(SUMO_ATTR_Y)) {
                pos = Position(attrs.get<double>(SUMO_ATTR_X, myVehicleParameter->id.c_str(), ok), attrs.get<double>(SUMO_ATTR_Y, myVehicleParameter->id.c_str(), ok));
            } else {
                pos = Position(attrs.get<double>(SUMO_ATTR_LON, myVehicleParameter->id.c_str(), ok), attrs.get<double>(SUMO_ATTR_LAT, myVehicleParameter->id.c_str(), ok));
                geo = true;
            }
            PositionVector positions;
            positions.push_back(pos);
            ConstROEdgeVector geoEdges;
            SUMOVehicleClass vClass = SVC_PASSENGER;
            if (!myNet.getVTypeDistribution(myVehicleParameter->vtypeid)) {
                SUMOVTypeParameter* type = myNet.getVehicleTypeSecure(myVehicleParameter->vtypeid);
                if (type != nullptr) {
                    vClass = type->vehicleClass;
                }
            }
            parseGeoEdges(positions, geo, vClass, geoEdges, myVehicleParameter->id, true, ok);
            if (ok) {
                edge = geoEdges.front();
                hasPos = true;
                if (geo) {
                    GeoConvHelper::getFinal().x2cartesian_const(pos);
                }
                stop.parametersSet |= STOP_END_SET;
                stop.endPos = edge->getLanes()[0]->getShape().nearest_offset_to_point2D(pos, false);
            } else {
                return result;
            }
        } else if (!ok || (stop.lane == "" && stop.edge == "")) {
            myErrorOutput->inform("A stop must be placed on a bus stop, a container stop, a parking area, an edge or a lane" + errorSuffix);
            return result;
        }
        if (!hasPos) {
            stop.endPos = attrs.getOpt<double>(SUMO_ATTR_ENDPOS, nullptr, ok, edge->getLength());
        }
        stop.startPos = attrs.getOpt<double>(SUMO_ATTR_STARTPOS, nullptr, ok, stop.endPos - 2 * POSITION_EPS);
        const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, nullptr, ok, !attrs.hasAttribute(SUMO_ATTR_STARTPOS) && !attrs.hasAttribute(SUMO_ATTR_ENDPOS));
        const double endPosOffset = edge->isInternal() ? edge->getNormalBefore()->getLength() : 0;
        if (!ok || (checkStopPos(stop.startPos, stop.endPos, edge->getLength() + endPosOffset, POSITION_EPS, friendlyPos) != SUMORouteHandler::StopPos::STOPPOS_VALID)) {
            myErrorOutput->inform("Invalid start or end position for stop" + errorSuffix);
            return result;
        }
    }
    stop.edge = edge->getID();
    if (myActivePlan != nullptr) {
        ROPerson::addStop(*myActivePlan, stop, edge);
        result = myActivePlan->back()->getStopParameters();
    } else if (myVehicleParameter != nullptr) {
        myVehicleParameter->stops.push_back(stop);
        result = &myVehicleParameter->stops.back();
    } else {
        myActiveRouteStops.push_back(stop);
        result = &myActiveRouteStops.back();
    }
    if (myInsertStopEdgesAt >= 0) {
        myActiveRoute.insert(myActiveRoute.begin() + myInsertStopEdgesAt, edge);
        myInsertStopEdgesAt++;
    }
    return result;
}


void
RORouteHandler::addRide(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    std::vector<ROPerson::PlanItem*>& plan = *myActivePlan;
    const std::string pid = myVehicleParameter->id;

    const ROEdge* from = nullptr;
    const ROEdge* to = nullptr;
    parseFromViaTo(SUMO_TAG_PERSON, attrs, ok);
    if (attrs.hasAttribute(SUMO_ATTR_FROM) || attrs.hasAttribute(SUMO_ATTR_FROM_JUNCTION) || attrs.hasAttribute(SUMO_ATTR_FROM_TAZ)
            || attrs.hasAttribute(SUMO_ATTR_FROMLONLAT) || attrs.hasAttribute(SUMO_ATTR_FROMXY)) {
        if (ok) {
            from = myActiveRoute.front();
        }
    } else if (plan.empty()) {
        myErrorOutput->inform("The start edge for person '" + pid + "' is not known.");
        return;
    }
    std::string stoppingPlaceID;
    const SUMOVehicleParameter::Stop* stop = retrieveStoppingPlace(attrs, " for ride of person '" + myVehicleParameter->id + "'", stoppingPlaceID);
    if (stop != nullptr) {
        to = myNet.getEdge(SUMOXMLDefinitions::getEdgeIDFromLane(stop->lane));
    } else {
        if (attrs.hasAttribute(SUMO_ATTR_TO) || attrs.hasAttribute(SUMO_ATTR_TO_JUNCTION) || attrs.hasAttribute(SUMO_ATTR_TO_TAZ)
                || attrs.hasAttribute(SUMO_ATTR_TOLONLAT) || attrs.hasAttribute(SUMO_ATTR_TOXY)) {
            to = myActiveRoute.back();
        } else {
            myErrorOutput->inform("The to edge is missing within a ride of '" + myVehicleParameter->id + "'.");
            return;
        }
    }
    double arrivalPos = attrs.getOpt<double>(SUMO_ATTR_ARRIVALPOS, myVehicleParameter->id.c_str(), ok,
                        stop == nullptr ? std::numeric_limits<double>::infinity() : stop->endPos);
    const std::string desc = attrs.get<std::string>(SUMO_ATTR_LINES, pid.c_str(), ok);
    const std::string group = attrs.getOpt<std::string>(SUMO_ATTR_GROUP, pid.c_str(), ok, "");

    if (plan.empty() && myVehicleParameter->departProcedure == DepartDefinition::TRIGGERED) {
        StringTokenizer st(desc);
        if (st.size() != 1) {
            myErrorOutput->inform("Triggered departure for person '" + pid + "' requires a unique lines value.");
            return;
        }
        const std::string vehID = st.front();
        if (!myNet.knowsVehicle(vehID)) {
            myErrorOutput->inform("Unknown vehicle '" + vehID + "' in triggered departure for person '" + pid + "'.");
            return;
        }
        SUMOTime vehDepart = myNet.getDeparture(vehID);
        if (vehDepart == -1) {
            myErrorOutput->inform("Cannot use triggered vehicle '" + vehID + "' in triggered departure for person '" + pid + "'.");
            return;
        }
        myVehicleParameter->depart = vehDepart + 1; // write person after vehicle
    }
    ROPerson::addRide(plan, from, to, desc, arrivalPos, stoppingPlaceID, group);
}


void
RORouteHandler::addTransport(const SUMOSAXAttributes& attrs) {
    if (myActiveContainerPlan != nullptr && myActiveContainerPlanSize == 0 && myVehicleParameter->departProcedure == DepartDefinition::TRIGGERED) {
        bool ok = true;
        const std::string pid = myVehicleParameter->id;
        const std::string desc = attrs.get<std::string>(SUMO_ATTR_LINES, pid.c_str(), ok);
        StringTokenizer st(desc);
        if (st.size() != 1) {
            throw ProcessError(TLF("Triggered departure for container '%' requires a unique lines value.", pid));
        }
        const std::string vehID = st.front();
        if (!myNet.knowsVehicle(vehID)) {
            throw ProcessError("Unknown vehicle '" + vehID + "' in triggered departure for container '" + pid + "'.");
        }
        SUMOTime vehDepart = myNet.getDeparture(vehID);
        if (vehDepart == -1) {
            throw ProcessError("Cannot use triggered vehicle '" + vehID + "' in triggered departure for container '" + pid + "'.");
        }
        myVehicleParameter->depart = vehDepart + 1; // write container after vehicle
    }
}


void
RORouteHandler::addTranship(const SUMOSAXAttributes& /*attrs*/) {
}


void
RORouteHandler::parseEdges(const std::string& desc, ConstROEdgeVector& into,
                           const std::string& rid, bool& ok) {
    for (StringTokenizer st(desc); st.hasNext();) {
        const std::string id = st.next();
        const ROEdge* edge = myNet.getEdge(id);
        if (edge == nullptr) {
            myErrorOutput->inform("The edge '" + id + "' within the route " + rid + " is not known.");
            ok = false;
        } else {
            into.push_back(edge);
        }
    }
}


void
RORouteHandler::parseWalkPositions(const SUMOSAXAttributes& attrs, const std::string& personID,
                                   const ROEdge* /*fromEdge*/, const ROEdge*& toEdge,
                                   double& departPos, double& arrivalPos, std::string& busStopID,
                                   const ROPerson::PlanItem* const lastStage, bool& ok) {
    const std::string description = "walk or personTrip of '" + personID + "'.";
    if (attrs.hasAttribute(SUMO_ATTR_DEPARTPOS)) {
        WRITE_WARNING(TL("The attribute departPos is no longer supported for walks, please use the person attribute, the arrivalPos of the previous step or explicit stops."));
    }
    departPos = myVehicleParameter->departPos;
    if (lastStage != nullptr) {
        departPos = lastStage->getDestinationPos();
    }

    busStopID = attrs.getOpt<std::string>(SUMO_ATTR_BUS_STOP, nullptr, ok, "");

    const SUMOVehicleParameter::Stop* bs = retrieveStoppingPlace(attrs, description, busStopID);
    if (bs != nullptr) {
        toEdge = myNet.getEdge(SUMOXMLDefinitions::getEdgeIDFromLane(bs->lane));
        arrivalPos = (bs->startPos + bs->endPos) / 2;
    }
    if (toEdge != nullptr) {
        if (attrs.hasAttribute(SUMO_ATTR_ARRIVALPOS)) {
            arrivalPos = SUMOVehicleParserHelper::parseWalkPos(SUMO_ATTR_ARRIVALPOS,
                         myHardFail, description, toEdge->getLength(),
                         attrs.get<std::string>(SUMO_ATTR_ARRIVALPOS, description.c_str(), ok));
        }
    } else {
        throw ProcessError(TLF("No destination edge for %.", description));
    }
}


void
RORouteHandler::addPersonTrip(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    const char* const id = myVehicleParameter->id.c_str();
    assert(!attrs.hasAttribute(SUMO_ATTR_EDGES));
    const ROEdge* from = nullptr;
    const ROEdge* to = nullptr;
    parseFromViaTo(SUMO_TAG_PERSON, attrs, ok);
    myInsertStopEdgesAt = -1;
    if (attrs.hasAttribute(SUMO_ATTR_FROM) || attrs.hasAttribute(SUMO_ATTR_FROM_JUNCTION) || attrs.hasAttribute(SUMO_ATTR_FROM_TAZ)
            || attrs.hasAttribute(SUMO_ATTR_FROMLONLAT) || attrs.hasAttribute(SUMO_ATTR_FROMXY)) {
        if (ok) {
            from = myActiveRoute.front();
        }
    } else if (myActivePlan->empty()) {
        throw ProcessError(TLF("Start edge not defined for person '%'.", myVehicleParameter->id));
    } else {
        from = myActivePlan->back()->getDestination();
    }
    if (attrs.hasAttribute(SUMO_ATTR_TO) || attrs.hasAttribute(SUMO_ATTR_TO_JUNCTION) || attrs.hasAttribute(SUMO_ATTR_TO_TAZ)
            || attrs.hasAttribute(SUMO_ATTR_TOLONLAT) || attrs.hasAttribute(SUMO_ATTR_TOXY)) {
        to = myActiveRoute.back();
    } // else, to may also be derived from stopping place

    const SUMOTime duration = attrs.getOptSUMOTimeReporting(SUMO_ATTR_DURATION, id, ok, -1);
    if (attrs.hasAttribute(SUMO_ATTR_DURATION) && duration <= 0) {
        throw ProcessError(TLF("Non-positive walking duration for '%'.", myVehicleParameter->id));
    }

    double departPos = 0;
    double arrivalPos = std::numeric_limits<double>::infinity();
    std::string busStopID;
    const ROPerson::PlanItem* const lastStage = myActivePlan->empty() ? nullptr : myActivePlan->back();
    parseWalkPositions(attrs, myVehicleParameter->id, from, to, departPos, arrivalPos, busStopID, lastStage, ok);

    const std::string modes = attrs.getOpt<std::string>(SUMO_ATTR_MODES, id, ok, "");
    const std::string group = attrs.getOpt<std::string>(SUMO_ATTR_GROUP, id, ok, "");
    SVCPermissions modeSet = 0;
    for (StringTokenizer st(modes); st.hasNext();) {
        const std::string mode = st.next();
        if (mode == "car") {
            modeSet |= SVC_PASSENGER;
        } else if (mode == "taxi") {
            modeSet |= SVC_TAXI;
        } else if (mode == "bicycle") {
            modeSet |= SVC_BICYCLE;
        } else if (mode == "public") {
            modeSet |= SVC_BUS;
        } else {
            throw InvalidArgument("Unknown person mode '" + mode + "'.");
        }
    }
    const std::string types = attrs.getOpt<std::string>(SUMO_ATTR_VTYPES, id, ok, "");
    double walkFactor = attrs.getOpt<double>(SUMO_ATTR_WALKFACTOR, id, ok, OptionsCont::getOptions().getFloat("persontrip.walkfactor"));
    if (ok) {
        const std::string originStopID = myActivePlan->empty() ?  "" : myActivePlan->back()->getStopDest();
        ROPerson::addTrip(*myActivePlan, myVehicleParameter->id, from, to, modeSet, types,
                          departPos, originStopID, arrivalPos, busStopID, walkFactor, group);
        myParamStack.push_back(myActivePlan->back());
    }
}


void
RORouteHandler::addWalk(const SUMOSAXAttributes& attrs) {
    // parse walks from->to as person trips
    if (attrs.hasAttribute(SUMO_ATTR_EDGES) || attrs.hasAttribute(SUMO_ATTR_ROUTE)) {
        // XXX allow --repair?
        bool ok = true;
        if (attrs.hasAttribute(SUMO_ATTR_ROUTE)) {
            const std::string routeID = attrs.get<std::string>(SUMO_ATTR_ROUTE, myVehicleParameter->id.c_str(), ok);
            RORouteDef* routeDef = myNet.getRouteDef(routeID);
            const RORoute* route = routeDef != nullptr ? routeDef->getFirstRoute() : nullptr;
            if (route == nullptr) {
                throw ProcessError("The route '" + routeID + "' for walk of person '" + myVehicleParameter->id + "' is not known.");
            }
            myActiveRoute = route->getEdgeVector();
        } else {
            myActiveRoute.clear();
            parseEdges(attrs.get<std::string>(SUMO_ATTR_EDGES, myVehicleParameter->id.c_str(), ok), myActiveRoute, " walk for person '" + myVehicleParameter->id + "'", ok);
        }
        const char* const objId = myVehicleParameter->id.c_str();
        const double duration = attrs.getOpt<double>(SUMO_ATTR_DURATION, objId, ok, -1);
        if (attrs.hasAttribute(SUMO_ATTR_DURATION) && duration <= 0) {
            throw ProcessError(TLF("Non-positive walking duration for '%'.", myVehicleParameter->id));
        }
        const double speed = attrs.getOpt<double>(SUMO_ATTR_SPEED, objId, ok, -1.);
        if (attrs.hasAttribute(SUMO_ATTR_SPEED) && speed <= 0) {
            throw ProcessError(TLF("Non-positive walking speed for '%'.", myVehicleParameter->id));
        }
        double departPos = 0.;
        double arrivalPos = std::numeric_limits<double>::infinity();
        if (attrs.hasAttribute(SUMO_ATTR_DEPARTPOS)) {
            WRITE_WARNING(TL("The attribute departPos is no longer supported for walks, please use the person attribute, the arrivalPos of the previous step or explicit stops."));
        }
        if (attrs.hasAttribute(SUMO_ATTR_ARRIVALPOS)) {
            arrivalPos = SUMOVehicleParserHelper::parseWalkPos(SUMO_ATTR_ARRIVALPOS, myHardFail, objId, myActiveRoute.back()->getLength(), attrs.get<std::string>(SUMO_ATTR_ARRIVALPOS, objId, ok));
        }
        std::string stoppingPlaceID;
        const std::string errorSuffix = " for walk of person '" + myVehicleParameter->id + "'";
        retrieveStoppingPlace(attrs, errorSuffix, stoppingPlaceID);
        if (ok) {
            ROPerson::addWalk(*myActivePlan, myActiveRoute, duration, speed, departPos, arrivalPos, stoppingPlaceID);
            myParamStack.push_back(myActivePlan->back());
        }
    } else {
        addPersonTrip(attrs);
    }
}


void
RORouteHandler::initLaneTree(NamedRTree* tree) {
    for (const auto& edgeItem : myNet.getEdgeMap()) {
        for (ROLane* lane : edgeItem.second->getLanes()) {
            Boundary b = lane->getShape().getBoxBoundary();
            const float cmin[2] = {(float) b.xmin(), (float) b.ymin()};
            const float cmax[2] = {(float) b.xmax(), (float) b.ymax()};
            tree->Insert(cmin, cmax, lane);
        }
    }
}


ROEdge*
RORouteHandler::retrieveEdge(const std::string& id) {
    return myNet.getEdge(id);
}

bool
RORouteHandler::checkLastDepart() {
    if (!myUnsortedInput) {
        return SUMORouteHandler::checkLastDepart();
    }
    return true;
}

/****************************************************************************/
