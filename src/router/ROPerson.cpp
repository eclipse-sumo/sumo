/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2002-2024 German Aerospace Center (DLR) and others.
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
/// @file    ROPerson.cpp
/// @author  Daniel Krajzewicz
/// @author  Axel Wegener
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Sept 2002
///
// A vehicle as used by router
/****************************************************************************/
#include <config.h>

#include <string>
#include <iostream>
#include <utils/common/StringTokenizer.h>
#include <utils/common/ToString.h>
#include <utils/common/StringUtils.h>
#include <utils/common/MsgHandler.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/vehicle/SUMOVTypeParameter.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include "RORouteDef.h"
#include "RORoute.h"
#include "ROVehicle.h"
#include "ROHelper.h"
#include "RONet.h"
#include "ROLane.h"
#include "ROPerson.h"

const std::string ROPerson::PlanItem::UNDEFINED_STOPPING_PLACE;

// ===========================================================================
// method definitions
// ===========================================================================
ROPerson::ROPerson(const SUMOVehicleParameter& pars, const SUMOVTypeParameter* type)
    : RORoutable(pars, type) {
}


ROPerson::~ROPerson() {
    for (PlanItem* const it : myPlan) {
        delete it;
    }
}


void
ROPerson::addTrip(std::vector<PlanItem*>& plan, const std::string& id,
                  const ROEdge* const from, const ROEdge* const to, const SVCPermissions modeSet,
                  const std::string& vTypes,
                  const double departPos, const std::string& stopOrigin,
                  const double arrivalPos, const std::string& busStop,
                  double walkFactor, const std::string& group) {
    PersonTrip* trip = new PersonTrip(from, to, modeSet, departPos, stopOrigin, arrivalPos, busStop, walkFactor, group);
    RONet* net = RONet::getInstance();
    SUMOVehicleParameter pars;
    pars.departProcedure = DepartDefinition::TRIGGERED;
    if (departPos != 0) {
        pars.departPosProcedure = DepartPosDefinition::GIVEN;
        pars.departPos = departPos;
        pars.parametersSet |= VEHPARS_DEPARTPOS_SET;
    }
    for (StringTokenizer st(vTypes); st.hasNext();) {
        pars.vtypeid = st.next();
        pars.parametersSet |= VEHPARS_VTYPE_SET;
        const SUMOVTypeParameter* type = net->getVehicleTypeSecure(pars.vtypeid);
        if (type == nullptr) {
            delete trip;
            throw InvalidArgument("The vehicle type '" + pars.vtypeid + "' in a trip for person '" + id + "' is not known.");
        }
        pars.id = id + "_" + toString(trip->getVehicles().size());
        trip->addVehicle(new ROVehicle(pars, new RORouteDef("!" + pars.id, 0, false, false), type, net));
        // update modeset with routing-category vClass
        if (type->vehicleClass == SVC_BICYCLE) {
            trip->updateModes(SVC_BICYCLE);
        } else {
            trip->updateModes(SVC_PASSENGER);
        }
    }
    if (trip->getVehicles().empty()) {
        if ((modeSet & SVC_PASSENGER) != 0) {
            pars.id = id + "_0";
            trip->addVehicle(new ROVehicle(pars, new RORouteDef("!" + pars.id, 0, false, false), net->getVehicleTypeSecure(DEFAULT_VTYPE_ID), net));
        }
        if ((modeSet & SVC_BICYCLE) != 0) {
            pars.id = id + "_b0";
            pars.vtypeid = DEFAULT_BIKETYPE_ID;
            pars.parametersSet |= VEHPARS_VTYPE_SET;
            trip->addVehicle(new ROVehicle(pars, new RORouteDef("!" + pars.id, 0, false, false), net->getVehicleTypeSecure(DEFAULT_BIKETYPE_ID), net));
        }
        if ((modeSet & SVC_TAXI) != 0) {
            // add dummy taxi for routing (never added to output)
            pars.id = "taxi"; // id is written as 'line'
            pars.vtypeid = DEFAULT_TAXITYPE_ID;
            trip->addVehicle(new ROVehicle(pars, new RORouteDef("!" + pars.id, 0, false, false), net->getVehicleTypeSecure(DEFAULT_TAXITYPE_ID), net));
        }
    }
    plan.push_back(trip);
}


void
ROPerson::addRide(std::vector<PlanItem*>& plan, const ROEdge* const from, const ROEdge* const to, const std::string& lines,
                  double arrivalPos, const std::string& destStop, const std::string& group) {
    plan.push_back(new PersonTrip(to, destStop));
    plan.back()->addTripItem(new Ride(-1, from, to, lines, group, -1., arrivalPos, -1., destStop));
}


void
ROPerson::addWalk(std::vector<PlanItem*>& plan, const ConstROEdgeVector& edges, const double duration, const double speed, const double departPos, const double arrivalPos, const std::string& busStop) {
    if (plan.empty() || plan.back()->isStop()) {
        plan.push_back(new PersonTrip(edges.back(), busStop));
    }
    plan.back()->addTripItem(new Walk(-1, edges, -1., duration, speed, departPos, arrivalPos, busStop));
}


void
ROPerson::addStop(std::vector<PlanItem*>& plan, const SUMOVehicleParameter::Stop& stopPar, const ROEdge* const stopEdge) {
    plan.push_back(new Stop(stopPar, stopEdge));
}


void
ROPerson::Ride::saveAsXML(OutputDevice& os, const bool extended, OptionsCont& options) const {
    os.openTag(SUMO_TAG_RIDE);
    std::string comment = "";
    if ((extended || options.getBool("write-costs")) && myCost >= 0.) {
        os.writeAttr(SUMO_ATTR_COST, myCost);
    }
    if (from != nullptr) {
        os.writeAttr(SUMO_ATTR_FROM, from->getID());
    }
    if (to != nullptr) {
        os.writeAttr(SUMO_ATTR_TO, to->getID());
    }
    if (destStop != "") {
        const std::string element = RONet::getInstance()->getStoppingPlaceElement(destStop);
        os.writeAttr(element, destStop);
        const std::string name = RONet::getInstance()->getStoppingPlaceName(destStop);
        if (name != "") {
            comment =  " <!-- " + name + " -->";
        }
    } else if (arrPos != 0 && arrPos != std::numeric_limits<double>::infinity()) {
        os.writeAttr(SUMO_ATTR_ARRIVALPOS, arrPos);
    }
    os.writeAttr(SUMO_ATTR_LINES, lines);
    if (group != "") {
        os.writeAttr(SUMO_ATTR_GROUP, group);
    }
    if (intended != "" && intended != lines) {
        os.writeAttr(SUMO_ATTR_INTENDED, intended);
    }
    if (depart >= 0) {
        os.writeAttr(SUMO_ATTR_DEPART, time2string(depart));
    }
    if (options.getBool("exit-times")) {
        os.writeAttr("started", time2string(getStart()));
        os.writeAttr("ended", time2string(getStart() + getDuration()));
    }
    if (options.getBool("route-length") && length != -1) {
        os.writeAttr("routeLength", length);
    }
    os.closeTag(comment);
}


void
ROPerson::Stop::saveAsXML(OutputDevice& os, const bool /*extended*/, const bool /*asTrip*/, OptionsCont& /*options*/) const {
    stopDesc.write(os, false);
    std::string comment = "";
    for (std::string sID : stopDesc.getStoppingPlaceIDs()) {
        const std::string name = RONet::getInstance()->getStoppingPlaceName(sID);
        if (name != "") {
            comment += name + " ";
        }
    }
    if (comment != "") {
        comment =  " <!-- " + comment + " -->";
    }
    stopDesc.writeParams(os);
    os.closeTag(comment);
}

void
ROPerson::Walk::saveAsXML(OutputDevice& os, const bool extended, OptionsCont& options) const {
    os.openTag(SUMO_TAG_WALK);
    std::string comment = "";
    if ((extended || options.getBool("write-costs")) && myCost >= 0.) {
        os.writeAttr(SUMO_ATTR_COST, myCost);
    }
    if (dur > 0) {
        os.writeAttr(SUMO_ATTR_DURATION, dur);
    }
    if (v > 0) {
        os.writeAttr(SUMO_ATTR_SPEED, v);
    }
    os.writeAttr(SUMO_ATTR_EDGES, edges);
    if (options.getBool("exit-times")) {
        os.writeAttr("started", time2string(getStart()));
        os.writeAttr("ended", time2string(getStart() + getDuration()));
        if (!exitTimes.empty()) {
            os.writeAttr("exitTimes", exitTimes);
        }
    }
    if (options.getBool("route-length")) {
        double length = 0;
        for (const ROEdge* roe : edges) {
            length += roe->getLength();
        }
        os.writeAttr("routeLength", length);
    }
    if (destStop != "") {
        const std::string element = RONet::getInstance()->getStoppingPlaceElement(destStop);
        os.writeAttr(element, destStop);
        const std::string name = RONet::getInstance()->getStoppingPlaceName(destStop);
        if (name != "") {
            comment =  " <!-- " + name + " -->";
        }
    } else if (arr != 0 && arr != std::numeric_limits<double>::infinity()) {
        os.writeAttr(SUMO_ATTR_ARRIVALPOS, arr);
    }
    os.closeTag(comment);
}

ROPerson::PlanItem*
ROPerson::PersonTrip::clone() const {
    PersonTrip* result = new PersonTrip(from, to, modes, dep, stopOrigin, arr, stopDest, walkFactor, group);
    for (auto* item : myTripItems) {
        result->myTripItems.push_back(item->clone());
    }
    return result;
}

void
ROPerson::PersonTrip::saveVehicles(OutputDevice& os, OutputDevice* const typeos, bool asAlternatives, OptionsCont& options) const {
    for (ROVehicle* veh : myVehicles) {
        if (!RONet::getInstance()->knowsVehicle(veh->getID())) {
            veh->saveAsXML(os, typeos, asAlternatives, options);
        }
    }
}

void
ROPerson::PersonTrip::saveAsXML(OutputDevice& os, const bool extended, const bool asTrip, OptionsCont& options) const {
    if ((asTrip || extended) && from != nullptr) {
        const bool writeGeoTrip = asTrip && options.getBool("write-trips.geo");
        os.openTag(SUMO_TAG_PERSONTRIP);
        if (writeGeoTrip) {
            Position fromPos = from->getLanes()[0]->getShape().positionAtOffset2D(getDepartPos());
            if (GeoConvHelper::getFinal().usingGeoProjection()) {
                os.setPrecision(gPrecisionGeo);
                GeoConvHelper::getFinal().cartesian2geo(fromPos);
                os.writeAttr(SUMO_ATTR_FROMLONLAT, fromPos);
                os.setPrecision(gPrecision);
            } else {
                os.writeAttr(SUMO_ATTR_FROMXY, fromPos);
            }
        } else {
            os.writeAttr(SUMO_ATTR_FROM, from->getID());
        }
        if (writeGeoTrip) {
            Position toPos = to->getLanes()[0]->getShape().positionAtOffset2D(MIN2(getArrivalPos(), to->getLanes()[0]->getShape().length2D()));
            if (GeoConvHelper::getFinal().usingGeoProjection()) {
                os.setPrecision(gPrecisionGeo);
                GeoConvHelper::getFinal().cartesian2geo(toPos);
                os.writeAttr(SUMO_ATTR_TOLONLAT, toPos);
                os.setPrecision(gPrecision);
            } else {
                os.writeAttr(SUMO_ATTR_TOXY, toPos);
            }
        } else {
            os.writeAttr(SUMO_ATTR_TO, to->getID());
        }
        std::vector<std::string> allowedModes;
        if ((modes & SVC_BUS) != 0) {
            allowedModes.push_back("public");
        }
        if ((modes & SVC_PASSENGER) != 0) {
            allowedModes.push_back("car");
        }
        if ((modes & SVC_TAXI) != 0) {
            allowedModes.push_back("taxi");
        }
        if ((modes & SVC_BICYCLE) != 0) {
            allowedModes.push_back("bicycle");
        }
        if (allowedModes.size() > 0) {
            os.writeAttr(SUMO_ATTR_MODES, toString(allowedModes));
        }
        if (!writeGeoTrip) {
            if (dep != 0 && dep != std::numeric_limits<double>::infinity()) {
                os.writeAttr(SUMO_ATTR_DEPARTPOS, dep);
            }
            if (arr != 0 && arr != std::numeric_limits<double>::infinity()) {
                os.writeAttr(SUMO_ATTR_ARRIVALPOS, arr);
            }
        }
        if (getStopDest() != "") {
            os.writeAttr(SUMO_ATTR_BUS_STOP, getStopDest());
        }
        if (walkFactor != 1) {
            os.writeAttr(SUMO_ATTR_WALKFACTOR, walkFactor);
        }
        if (extended && myTripItems.size() != 0) {
            std::vector<double> costs;
            for (const TripItem* const tripItem : myTripItems) {
                costs.push_back(tripItem->getCost());
            }
            os.writeAttr(SUMO_ATTR_COSTS, costs);
        }
        os.closeTag();
    } else {
        for (const TripItem* const it : myTripItems) {
            it->saveAsXML(os, extended, options);
        }
    }
}

SUMOTime
ROPerson::PersonTrip::getDuration() const {
    SUMOTime result = 0;
    for (TripItem* tItem : myTripItems) {
        result += tItem->getDuration();
    }
    return result;
}

bool
ROPerson::computeIntermodal(SUMOTime time, const RORouterProvider& provider,
                            const PersonTrip* const trip, const ROVehicle* const veh,
                            std::vector<TripItem*>& resultItems, MsgHandler* const errorHandler) {
    const double speed = getMaxSpeed() * trip->getWalkFactor();
    std::vector<ROIntermodalRouter::TripItem> result;
    provider.getIntermodalRouter().compute(trip->getOrigin(), trip->getDestination(),
                                           trip->getDepartPos(), trip->getStopOrigin(),
                                           trip->getArrivalPos(), trip->getStopDest(),
                                           speed, veh, trip->getModes(), time, result);
    bool carUsed = false;
    SUMOTime start = time;
    for (const ROIntermodalRouter::TripItem& item : result) {
        if (!item.edges.empty()) {
            if (item.line == "") {
                double depPos = trip->getDepartPos(false);
                double arrPos = trip->getArrivalPos(false);
                if (trip->getOrigin()->isTazConnector()) {
                    // walk the whole length of the first edge
                    const ROEdge* first = item.edges.front();
                    if (std::find(first->getPredecessors().begin(), first->getPredecessors().end(), trip->getOrigin()) != first->getPredecessors().end()) {
                        depPos = 0;
                    } else {
                        depPos = first->getLength();
                    }
                }
                if (trip->getDestination()->isTazConnector()) {
                    // walk the whole length of the last edge
                    const ROEdge* last = item.edges.back();
                    if (std::find(last->getSuccessors().begin(), last->getSuccessors().end(), trip->getDestination()) != last->getSuccessors().end()) {
                        arrPos = last->getLength();
                    } else {
                        arrPos = 0;
                    }
                }
                if (&item == &result.back() && trip->getStopDest() == "") {
                    resultItems.push_back(new Walk(start, item.edges, item.cost, item.exitTimes, depPos, arrPos));
                } else {
                    resultItems.push_back(new Walk(start, item.edges, item.cost, item.exitTimes, depPos, arrPos, item.destStop));
                }
            } else if (veh != nullptr && item.line == veh->getID()) {
                double cost = item.cost;
                if (veh->getVClass() != SVC_TAXI) {
                    RORoute* route = new RORoute(veh->getID() + "_RouteDef", item.edges);
                    route->setProbability(1);
                    veh->getRouteDefinition()->addLoadedAlternative(route);
                    carUsed = true;
                } else if (resultItems.empty()) {
                    // if this is the first plan item the initial taxi waiting time wasn't added yet
                    const double taxiWait = STEPS2TIME(string2time(OptionsCont::getOptions().getString("persontrip.taxi.waiting-time")));
                    cost += taxiWait;
                }
                resultItems.push_back(new Ride(start, item.edges.front(), item.edges.back(), veh->getID(), trip->getGroup(), cost, item.arrivalPos, item.length, item.destStop));
            } else {
                // write origin for first element of the plan
                const ROEdge* origin = trip == myPlan.front() && resultItems.empty() ? trip->getOrigin() : nullptr;
                resultItems.push_back(new Ride(start, origin, nullptr, item.line, trip->getGroup(), item.cost, item.arrivalPos, item.length, item.destStop, item.intended, TIME2STEPS(item.depart)));
            }
        }
        start += TIME2STEPS(item.cost);
    }
    if (result.empty()) {
        errorHandler->inform("No route for trip in person '" + getID() + "'.");
        myRoutingSuccess = false;
    }
    return carUsed;
}


void
ROPerson::computeRoute(const RORouterProvider& provider,
                       const bool /* removeLoops */, MsgHandler* errorHandler) {
    myRoutingSuccess = true;
    SUMOTime time = getParameter().depart;
    for (PlanItem* const it : myPlan) {
        if (it->needsRouting()) {
            PersonTrip* trip = static_cast<PersonTrip*>(it);
            const std::vector<ROVehicle*>& vehicles = trip->getVehicles();
            std::vector<TripItem*> resultItems;
            std::vector<TripItem*> best;
            const ROVehicle* bestVeh = nullptr;
            if (vehicles.empty()) {
                computeIntermodal(time, provider, trip, nullptr, best, errorHandler);
            } else {
                double bestCost = std::numeric_limits<double>::infinity();
                for (const ROVehicle* const v : vehicles) {
                    const bool carUsed = computeIntermodal(time, provider, trip, v, resultItems, errorHandler);
                    double cost = 0.;
                    for (const TripItem* const tripIt : resultItems) {
                        cost += tripIt->getCost();
                    }
                    if (cost < bestCost) {
                        bestCost = cost;
                        bestVeh = carUsed ? v : nullptr;
                        best.swap(resultItems);
                    }
                    for (const TripItem* const tripIt : resultItems) {
                        delete tripIt;
                    }
                    resultItems.clear();
                }
            }
            trip->setItems(best, bestVeh);
        }
        time += it->getDuration();
    }
}


void
ROPerson::saveAsXML(OutputDevice& os, OutputDevice* const typeos, bool asAlternatives, OptionsCont& options) const {
    // write the person's vehicles
    const bool writeTrip = options.exists("write-trips") && options.getBool("write-trips");
    if (!writeTrip) {
        for (const PlanItem* const it : myPlan) {
            it->saveVehicles(os, typeos, asAlternatives, options);
        }
    }

    if (typeos != nullptr && getType() != nullptr && !getType()->saved) {
        getType()->write(*typeos);
        getType()->saved = true;
    }
    if (getType() != nullptr && !getType()->saved) {
        getType()->write(os);
        getType()->saved = asAlternatives;
    }

    // write the person
    getParameter().write(os, options, SUMO_TAG_PERSON);

    for (const PlanItem* const it : myPlan) {
        it->saveAsXML(os, asAlternatives, writeTrip, options);
    }

    // write params
    getParameter().writeParams(os);
    os.closeTag();
}


/****************************************************************************/
