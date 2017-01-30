/****************************************************************************/
/// @file    RONet.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// The router's network representation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <algorithm>
#include <utils/vehicle/RouteCostCalculator.h>
#include <utils/vehicle/SUMOVTypeParameter.h>
#include <utils/vehicle/SUMOAbstractRouter.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/ToString.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/iodevices/OutputDevice.h>
#include "ROEdge.h"
#include "RONode.h"
#include "ROPerson.h"
#include "RORoute.h"
#include "RORouteDef.h"
#include "ROVehicle.h"
#include "RONet.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static member definitions
// ===========================================================================
RONet* RONet::myInstance = 0;


// ===========================================================================
// method definitions
// ===========================================================================
RONet*
RONet::getInstance(void) {
    if (myInstance != 0) {
        return myInstance;
    }
    throw ProcessError("A network was not yet constructed.");
}


RONet::RONet()
    : myVehicleTypes(), myDefaultVTypeMayBeDeleted(true), myDefaultPedTypeMayBeDeleted(true),
      myRoutesOutput(0), myRouteAlternativesOutput(0), myTypesOutput(0),
      myReadRouteNo(0), myDiscardedRouteNo(0), myWrittenRouteNo(0),
      myHavePermissions(false),
      myNumInternalEdges(0),
      myErrorHandler(OptionsCont::getOptions().exists("ignore-errors")
                     && OptionsCont::getOptions().getBool("ignore-errors") ? MsgHandler::getWarningInstance() : MsgHandler::getErrorInstance()) {
    if (myInstance != 0) {
        throw ProcessError("A network was already constructed.");
    }
    SUMOVTypeParameter* type = new SUMOVTypeParameter(DEFAULT_VTYPE_ID, SVC_PASSENGER);
    type->onlyReferenced = true;
    myVehicleTypes.add(type->id, type);
    SUMOVTypeParameter* defPedType = new SUMOVTypeParameter(DEFAULT_PEDTYPE_ID, SVC_PEDESTRIAN);
    defPedType->onlyReferenced = true;
    defPedType->setParameter |= VTYPEPARS_VEHICLECLASS_SET;
    myVehicleTypes.add(defPedType->id, defPedType);
    myInstance = this;
}


RONet::~RONet() {
    for (RoutablesMap::iterator routables = myRoutables.begin(); routables != myRoutables.end(); ++routables) {
        for (std::deque<RORoutable*>::iterator r = routables->second.begin(); r != routables->second.end(); ++r) {
            const ROVehicle* const veh = dynamic_cast<const ROVehicle*>(*r);
            // delete routes and the vehicle
            if (veh != 0 && veh->getRouteDefinition()->getID()[0] == '!') {
                if (!myRoutes.erase(veh->getRouteDefinition()->getID())) {
                    delete veh->getRouteDefinition();
                }
            }
            delete *r;
        }
    }
    for (std::map<std::string, SUMOVehicleParameter::Stop*>::iterator it = myBusStops.begin(); it != myBusStops.end(); ++it) {
        delete it->second;
    }
    for (std::map<std::string, SUMOVehicleParameter::Stop*>::iterator it = myContainerStops.begin(); it != myContainerStops.end(); ++it) {
        delete it->second;
    }
    myNodes.clear();
    myEdges.clear();
    myVehicleTypes.clear();
    myRoutes.clear();
    myRoutables.clear();
}


void
RONet::addRestriction(const std::string& id, const SUMOVehicleClass svc, const SUMOReal speed) {
    myRestrictions[id][svc] = speed;
}


const std::map<SUMOVehicleClass, SUMOReal>*
RONet::getRestrictions(const std::string& id) const {
    std::map<std::string, std::map<SUMOVehicleClass, SUMOReal> >::const_iterator i = myRestrictions.find(id);
    if (i == myRestrictions.end()) {
        return 0;
    }
    return &i->second;
}


bool
RONet::addEdge(ROEdge* edge) {
    if (!myEdges.add(edge->getID(), edge)) {
        WRITE_ERROR("The edge '" + edge->getID() + "' occurs at least twice.");
        delete edge;
        return false;
    }
    if (edge->getFunc() == ROEdge::ET_INTERNAL) {
        myNumInternalEdges += 1;
    }
    return true;
}


bool
RONet::addDistrict(const std::string id, ROEdge* source, ROEdge* sink) {
    if (myDistricts.count(id) > 0) {
        WRITE_ERROR("The TAZ '" + id + "' occurs at least twice.");
        delete source;
        delete sink;
        return false;
    }
    sink->setFunc(ROEdge::ET_DISTRICT);
    addEdge(sink);
    source->setFunc(ROEdge::ET_DISTRICT);
    addEdge(source);
    myDistricts[id] = std::make_pair(std::vector<std::string>(), std::vector<std::string>());
    return true;
}


bool
RONet::addDistrictEdge(const std::string tazID, const std::string edgeID, const bool isSource) {
    if (myDistricts.count(tazID) == 0) {
        WRITE_ERROR("The TAZ '" + tazID + "' is unknown.");
        return false;
    }
    ROEdge* edge = getEdge(edgeID);
    if (edge == 0) {
        WRITE_ERROR("The edge '" + edgeID + "' for TAZ '" + tazID + "' is unknown.");
        return false;
    }
    if (isSource) {
        getEdge(tazID + "-source")->addSuccessor(edge);
        myDistricts[tazID].first.push_back(edgeID);
    } else {
        edge->addSuccessor(getEdge(tazID + "-sink"));
        myDistricts[tazID].second.push_back(edgeID);
    }
    return true;
}


void
RONet::addNode(RONode* node) {
    if (!myNodes.add(node->getID(), node)) {
        WRITE_ERROR("The node '" + node->getID() + "' occurs at least twice.");
        delete node;
    }
}


void
RONet::addBusStop(const std::string& id, SUMOVehicleParameter::Stop* stop) {
    std::map<std::string, SUMOVehicleParameter::Stop*>::const_iterator it = myBusStops.find(id);
    if (it != myBusStops.end()) {
        WRITE_ERROR("The bus stop '" + id + "' occurs at least twice.");
        delete stop;
    }
    myBusStops[id] = stop;
}


void
RONet::addContainerStop(const std::string& id, SUMOVehicleParameter::Stop* stop) {
    std::map<std::string, SUMOVehicleParameter::Stop*>::const_iterator it = myContainerStops.find(id);
    if (it != myContainerStops.end()) {
        WRITE_ERROR("The container stop '" + id + "' occurs at least twice.");
        delete stop;
    }
    myContainerStops[id] = stop;
}


void
RONet::addParkingArea(const std::string& id, SUMOVehicleParameter::Stop* stop) {
    std::map<std::string, SUMOVehicleParameter::Stop*>::const_iterator it = myParkingAreas.find(id);
    if (it != myParkingAreas.end()) {
        WRITE_ERROR("The parking area '" + id + "' occurs at least twice.");
        delete stop;
    }
    myParkingAreas[id] = stop;
}


bool
RONet::addRouteDef(RORouteDef* def) {
    return myRoutes.add(def->getID(), def);
}


void
RONet::openOutput(const OptionsCont& options, const std::string altFilename) {
    if (options.isSet("output-file") && options.getString("output-file") != "") {
        myRoutesOutput = &OutputDevice::getDevice(options.getString("output-file"));
        myRoutesOutput->writeHeader<ROEdge>(SUMO_TAG_ROUTES);
        myRoutesOutput->writeAttr("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance").writeAttr("xsi:noNamespaceSchemaLocation", "http://sumo.dlr.de/xsd/routes_file.xsd");
    }
    if (altFilename != "") {
        myRouteAlternativesOutput = &OutputDevice::getDevice(altFilename);
        myRouteAlternativesOutput->writeHeader<ROEdge>(SUMO_TAG_ROUTES);
        myRouteAlternativesOutput->writeAttr("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance").writeAttr("xsi:noNamespaceSchemaLocation", "http://sumo.dlr.de/xsd/routes_file.xsd");
    }
    if (options.isSet("vtype-output") && options.getString("vtype-output") != "") {
        myTypesOutput = &OutputDevice::getDevice(options.getString("vtype-output"));
        myTypesOutput->writeHeader<ROEdge>(SUMO_TAG_ROUTES);
        myTypesOutput->writeAttr("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance").writeAttr("xsi:noNamespaceSchemaLocation", "http://sumo.dlr.de/xsd/routes_file.xsd");
    }
}


void
RONet::cleanup() {
    // end writing
    if (myRoutesOutput != 0) {
        myRoutesOutput->close();
    }
    // only if opened
    if (myRouteAlternativesOutput != 0) {
        myRouteAlternativesOutput->close();
    }
    // only if opened
    if (myTypesOutput != 0) {
        myTypesOutput->close();
    }
    RouteCostCalculator<RORoute, ROEdge, ROVehicle>::cleanup();
#ifdef HAVE_FOX
    if (myThreadPool.size() > 0) {
        myThreadPool.clear();
    }
#endif
}



SUMOVTypeParameter*
RONet::getVehicleTypeSecure(const std::string& id) {
    // check whether the type was already known
    SUMOVTypeParameter* type = myVehicleTypes.get(id);
    if (id == DEFAULT_VTYPE_ID) {
        myDefaultVTypeMayBeDeleted = false;
    }
    if (id == DEFAULT_PEDTYPE_ID) {
        myDefaultPedTypeMayBeDeleted = false;
    }
    if (type != 0) {
        return type;
    }
    VTypeDistDictType::iterator it2 = myVTypeDistDict.find(id);
    if (it2 != myVTypeDistDict.end()) {
        return it2->second->get();
    }
    if (id == "") {
        // ok, no vehicle type or an unknown type was given within the user input
        //  return the default type
        myDefaultVTypeMayBeDeleted = false;
        return myVehicleTypes.get(DEFAULT_VTYPE_ID);
    }
    return type;
}


bool
RONet::checkVType(const std::string& id) {
    if (id == DEFAULT_VTYPE_ID) {
        if (myDefaultVTypeMayBeDeleted) {
            myVehicleTypes.remove(id);
            myDefaultVTypeMayBeDeleted = false;
        } else {
            return false;
        }
    } else if (id == DEFAULT_PEDTYPE_ID) {
        if (myDefaultPedTypeMayBeDeleted) {
            myVehicleTypes.remove(id);
            myDefaultPedTypeMayBeDeleted = false;
        } else {
            return false;
        }
    } else {
        if (myVehicleTypes.get(id) != 0 || myVTypeDistDict.find(id) != myVTypeDistDict.end()) {
            return false;
        }
    }
    return true;
}


bool
RONet::addVehicleType(SUMOVTypeParameter* type) {
    if (checkVType(type->id)) {
        myVehicleTypes.add(type->id, type);
    } else {
        WRITE_ERROR("The vehicle type '" + type->id + "' occurs at least twice.");
        delete type;
        return false;
    }
    return true;
}


bool
RONet::addVTypeDistribution(const std::string& id, RandomDistributor<SUMOVTypeParameter*>* vehTypeDistribution) {
    if (checkVType(id)) {
        myVTypeDistDict[id] = vehTypeDistribution;
        return true;
    }
    return false;
}


bool
RONet::addVehicle(const std::string& id, ROVehicle* veh) {
    if (myVehIDs.find(id) == myVehIDs.end()) {
        myVehIDs.insert(id);
        myRoutables[veh->getDepart()].push_back(veh);
        myReadRouteNo++;
        return true;
    }
    WRITE_ERROR("Another vehicle with the id '" + id + "' exists.");
    return false;
}


bool
RONet::addFlow(SUMOVehicleParameter* flow, const bool randomize) {
    if (randomize) {
        myDepartures[flow->id].reserve(flow->repetitionNumber);
        for (int i = 0; i < flow->repetitionNumber; ++i) {
            myDepartures[flow->id].push_back(flow->depart + RandHelper::rand(flow->repetitionNumber * flow->repetitionOffset));
        }
        std::sort(myDepartures[flow->id].begin(), myDepartures[flow->id].end());
        std::reverse(myDepartures[flow->id].begin(), myDepartures[flow->id].end());
    }
    return myFlows.add(flow->id, flow);
}


bool
RONet::addPerson(ROPerson* person) {
    if (myPersonIDs.count(person->getID()) == 0) {
        myPersonIDs.insert(person->getID());
        myRoutables[person->getDepart()].push_back(person);
        return true;
    }
    WRITE_ERROR("Another person with the id '" + person->getID() + "' exists.");
    return false;
}


void
RONet::addContainer(const SUMOTime depart, const std::string desc) {
    myContainers.insert(std::pair<const SUMOTime, const std::string>(depart, desc));
}


void
RONet::checkFlows(SUMOTime time, MsgHandler* errorHandler) {
    std::vector<std::string> toRemove;
    for (NamedObjectCont<SUMOVehicleParameter*>::IDMap::const_iterator i = myFlows.getMyMap().begin(); i != myFlows.getMyMap().end(); ++i) {
        SUMOVehicleParameter* pars = i->second;
        if (pars->repetitionProbability > 0) {
            const SUMOTime origDepart = pars->depart;
            while (pars->depart < time) {
                if (pars->repetitionEnd <= pars->depart) {
                    toRemove.push_back(i->first);
                    break;
                }
                // only call rand if all other conditions are met
                if (RandHelper::rand() < (pars->repetitionProbability * TS)) {
                    SUMOVehicleParameter* newPars = new SUMOVehicleParameter(*pars);
                    newPars->id = pars->id + "." + toString(pars->repetitionsDone);
                    newPars->depart = pars->depart;
                    for (std::vector<SUMOVehicleParameter::Stop>::iterator stop = newPars->stops.begin(); stop != newPars->stops.end(); ++stop) {
                        if (stop->until >= 0) {
                            stop->until += pars->depart - origDepart;
                        }
                    }
                    pars->repetitionsDone++;
                    // try to build the vehicle
                    SUMOVTypeParameter* type = getVehicleTypeSecure(pars->vtypeid);
                    if (type == 0) {
                        type = getVehicleTypeSecure(DEFAULT_VTYPE_ID);
                    } else {
                        // fix the type id in case we used a distribution
                        newPars->vtypeid = type->id;
                    }
                    const SUMOTime stopOffset = pars->routeid[0] == '!' ? pars->depart - origDepart : pars->depart;
                    RORouteDef* route = getRouteDef(pars->routeid)->copy("!" + newPars->id, stopOffset);
                    ROVehicle* veh = new ROVehicle(*newPars, route, type, this, errorHandler);
                    addVehicle(newPars->id, veh);
                    delete newPars;
                }
                pars->depart += DELTA_T;
            }
        } else {
            while (pars->repetitionsDone < pars->repetitionNumber) {
                SUMOTime depart = static_cast<SUMOTime>(pars->depart + pars->repetitionsDone * pars->repetitionOffset);
                if (myDepartures.find(pars->id) != myDepartures.end()) {
                    depart = myDepartures[pars->id].back();
                }
                if (depart >= time + DELTA_T) {
                    break;
                }
                if (myDepartures.find(pars->id) != myDepartures.end()) {
                    myDepartures[pars->id].pop_back();
                }
                SUMOVehicleParameter* newPars = new SUMOVehicleParameter(*pars);
                newPars->id = pars->id + "." + toString(pars->repetitionsDone);
                newPars->depart = depart;
                for (std::vector<SUMOVehicleParameter::Stop>::iterator stop = newPars->stops.begin(); stop != newPars->stops.end(); ++stop) {
                    if (stop->until >= 0) {
                        stop->until += depart - pars->depart;
                    }
                }
                pars->repetitionsDone++;
                // try to build the vehicle
                SUMOVTypeParameter* type = getVehicleTypeSecure(pars->vtypeid);
                if (type == 0) {
                    type = getVehicleTypeSecure(DEFAULT_VTYPE_ID);
                } else {
                    // fix the type id in case we used a distribution
                    newPars->vtypeid = type->id;
                }
                const SUMOTime stopOffset = pars->routeid[0] == '!' ? depart - pars->depart : depart;
                RORouteDef* route = getRouteDef(pars->routeid)->copy("!" + newPars->id, stopOffset);
                ROVehicle* veh = new ROVehicle(*newPars, route, type, this, errorHandler);
                addVehicle(newPars->id, veh);
                delete newPars;
            }
            if (pars->repetitionsDone == pars->repetitionNumber) {
                toRemove.push_back(i->first);
            }
        }
    }
    for (std::vector<std::string>::const_iterator i = toRemove.begin(); i != toRemove.end(); ++i) {
        myFlows.erase(*i);
    }
}


void
RONet::createBulkRouteRequests(const RORouterProvider& provider, const SUMOTime time, const bool removeLoops) {
    std::map<const int, std::vector<RORoutable*> > bulkVehs;
    for (RoutablesMap::const_iterator i = myRoutables.begin(); i != myRoutables.end(); ++i) {
        if (i->first >= time) {
            break;
        }
        for (std::deque<RORoutable*>::const_iterator r = i->second.begin(); r != i->second.end(); ++r) {
            RORoutable* const routable = *r;
            const ROEdge* const depEdge = routable->getDepartEdge();
            bulkVehs[depEdge->getNumericalID()].push_back(routable);
            RORoutable* const first = bulkVehs[depEdge->getNumericalID()].front();
            if (first->getMaxSpeed() != routable->getMaxSpeed()) {
                WRITE_WARNING("Bulking different maximum speeds ('" + first->getID() + "' and '" + routable->getID() + "') may lead to suboptimal routes.");
            }
            if (first->getVClass() != routable->getVClass()) {
                WRITE_WARNING("Bulking different vehicle classes ('" + first->getID() + "' and '" + routable->getID() + "') may lead to invalid routes.");
            }
        }
    }
    int workerIndex = 0;
    for (std::map<const int, std::vector<RORoutable*> >::const_iterator i = bulkVehs.begin(); i != bulkVehs.end(); ++i) {
#ifdef HAVE_FOX
        if (myThreadPool.size() > 0) {
            RORoutable* const first = i->second.front();
            myThreadPool.add(new RoutingTask(first, removeLoops, myErrorHandler), workerIndex);
            myThreadPool.add(new BulkmodeTask(true), workerIndex);
            for (std::vector<RORoutable*>::const_iterator j = i->second.begin() + 1; j != i->second.end(); ++j) {
                myThreadPool.add(new RoutingTask(*j, removeLoops, myErrorHandler), workerIndex);
            }
            myThreadPool.add(new BulkmodeTask(false), workerIndex);
            workerIndex++;
            if (workerIndex == (int)myThreadPool.size()) {
                workerIndex = 0;
            }
            continue;
        }
#endif
        for (std::vector<RORoutable*>::const_iterator j = i->second.begin(); j != i->second.end(); ++j) {
            (*j)->computeRoute(provider, removeLoops, myErrorHandler);
            provider.getVehicleRouter().setBulkMode(true);
        }
        provider.getVehicleRouter().setBulkMode(false);
    }
}


SUMOTime
RONet::saveAndRemoveRoutesUntil(OptionsCont& options, const RORouterProvider& provider,
                                SUMOTime time) {
    MsgHandler* mh = (options.getBool("ignore-errors") ?
                      MsgHandler::getWarningInstance() : MsgHandler::getErrorInstance());
    checkFlows(time, mh);
    SUMOTime lastTime = -1;
    const bool removeLoops = options.getBool("remove-loops");
    const int maxNumThreads = options.getInt("routing-threads");
    if (myRoutables.size() != 0) {
        if (options.getBool("bulk-routing")) {
#ifdef HAVE_FOX
            while ((int)myThreadPool.size() < maxNumThreads) {
                new WorkerThread(myThreadPool, provider);
            }
#endif
            createBulkRouteRequests(provider, time, removeLoops);
        } else {
            for (RoutablesMap::const_iterator i = myRoutables.begin(); i != myRoutables.end(); ++i) {
                if (i->first >= time) {
                    break;
                }
                for (std::deque<RORoutable*>::const_iterator r = i->second.begin(); r != i->second.end(); ++r) {
                    RORoutable* const routable = *r;
#ifdef HAVE_FOX
                    // add task
                    if (maxNumThreads > 0) {
                        const int numThreads = (int)myThreadPool.size();
                        if (numThreads == 0) {
                            // This is the very first routing. Since at least the CHRouter needs initialization
                            // before it gets cloned, we do not do this in parallel
                            routable->computeRoute(provider, removeLoops, myErrorHandler);
                            new WorkerThread(myThreadPool, provider);
                        } else {
                            // add thread if necessary
                            if (numThreads < maxNumThreads && myThreadPool.isFull()) {
                                new WorkerThread(myThreadPool, provider);
                            }
                            myThreadPool.add(new RoutingTask(routable, removeLoops, myErrorHandler));
                        }
                        continue;
                    }
#endif
                    routable->computeRoute(provider, removeLoops, myErrorHandler);
                }
            }
        }
#ifdef HAVE_FOX
        myThreadPool.waitAll();
#endif
    }
    // write all vehicles (and additional structures)
    while (myRoutables.size() != 0 || myContainers.size() != 0) {
        // get the next vehicle, person or container
        RoutablesMap::iterator routables = myRoutables.begin();
        const SUMOTime routableTime = routables == myRoutables.end() ? SUMOTime_MAX : routables->first;
        ContainerMap::iterator container = myContainers.begin();
        const SUMOTime containerTime = container == myContainers.end() ? SUMOTime_MAX : container->first;
        // check whether it shall not yet be computed
        if (routableTime >= time && containerTime >= time) {
            lastTime = MIN2(routableTime, containerTime);
            break;
        }
        const SUMOTime minTime = MIN2(routableTime, containerTime);
        if (routableTime == minTime) {
            const RORoutable* const r = routables->second.front();
            // check whether to print the output
            if (lastTime != routableTime && lastTime != -1) {
                // report writing progress
                if (options.getInt("stats-period") >= 0 && ((int) routableTime % options.getInt("stats-period")) == 0) {
                    WRITE_MESSAGE("Read: " + toString(myReadRouteNo) + ",  Discarded: " + toString(myDiscardedRouteNo) + ",  Written: " + toString(myWrittenRouteNo));
                }
            }
            lastTime = routableTime;

            // ok, compute the route (try it)
            if (r->getRoutingSuccess()) {
                // write the route
                r->write(*myRoutesOutput, myRouteAlternativesOutput, myTypesOutput, options);
                myWrittenRouteNo++;
            } else {
                myDiscardedRouteNo++;
            }
            const ROVehicle* const veh = dynamic_cast<const ROVehicle*>(r);
            // delete routes and the vehicle
            if (veh != 0 && veh->getRouteDefinition()->getID()[0] == '!') {
                if (!myRoutes.erase(veh->getRouteDefinition()->getID())) {
                    delete veh->getRouteDefinition();
                }
            }
            routables->second.pop_front();
            if (routables->second.empty()) {
                myRoutables.erase(routables);
            }
            delete r;
        }
        if (containerTime == minTime) {
            myRoutesOutput->writePreformattedTag(container->second);
            if (myRouteAlternativesOutput != 0) {
                myRouteAlternativesOutput->writePreformattedTag(container->second);
            }
            myContainers.erase(container);
        }
    }
    return lastTime;
}


bool
RONet::furtherStored() {
    return myRoutables.size() > 0 || myFlows.size() > 0 || myContainers.size() > 0;
}


int
RONet::getEdgeNo() const {
    return myEdges.size();
}


int
RONet::getInternalEdgeNumber() const {
    return myNumInternalEdges;
}


const std::map<std::string, ROEdge*>&
RONet::getEdgeMap() const {
    return myEdges.getMyMap();
}


void
RONet::adaptIntermodalRouter(ROIntermodalRouter& router) {
    // add access to all public transport stops
    for (std::map<std::string, SUMOVehicleParameter::Stop*>::const_iterator i = myInstance->myBusStops.begin(); i != myInstance->myBusStops.end(); ++i) {
        router.addAccess(i->first, myInstance->getEdgeForLaneID(i->second->lane), i->second->endPos);
        for (std::multimap<std::string, SUMOReal>::const_iterator a = i->second->accessPos.begin(); a != i->second->accessPos.end(); ++a) {
            router.addAccess(i->first, myInstance->getEdgeForLaneID(a->first), a->second);
        }
    }
    // fill the public transport router with pre-parsed public transport lines
    for (std::map<std::string, SUMOVehicleParameter*>::const_iterator i = myInstance->myFlows.getMyMap().begin(); i != myInstance->myFlows.getMyMap().end(); ++i) {
        if (i->second->line != "") {
            RORouteDef* route = myInstance->getRouteDef(i->second->routeid);
            const std::vector<SUMOVehicleParameter::Stop>* addStops = 0;
            if (route != 0 && route->getFirstRoute() != 0) {
                addStops = &route->getFirstRoute()->getStops();
            }
            router.addSchedule(*i->second, addStops);
        }
    }
    for (RoutablesMap::const_iterator i = myInstance->myRoutables.begin(); i != myInstance->myRoutables.end(); ++i) {
        for (std::deque<RORoutable*>::const_iterator r = i->second.begin(); r != i->second.end(); ++r) {
            const ROVehicle* const veh = dynamic_cast<ROVehicle*>(*r);
            // add single vehicles with line attribute which are not part of a flow
            if (veh != 0 && veh->getParameter().line != "" && veh->getParameter().repetitionNumber < 0) {
                router.addSchedule(veh->getParameter());
            }
        }
    }
}


bool
RONet::hasPermissions() const {
    return myHavePermissions;
}


void
RONet::setPermissionsFound() {
    myHavePermissions = true;
}


#ifdef HAVE_FOX
// ---------------------------------------------------------------------------
// RONet::RoutingTask-methods
// ---------------------------------------------------------------------------
void
RONet::RoutingTask::run(FXWorkerThread* context) {
    myRoutable->computeRoute(*static_cast<WorkerThread*>(context), myRemoveLoops, myErrorHandler);
}
#endif


/****************************************************************************/

