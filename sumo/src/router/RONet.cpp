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
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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
#include "ROEdge.h"
#include "RONode.h"
#include "RONet.h"
#include "RORoute.h"
#include "RORouteDef.h"
#include "ROVehicle.h"
#include <utils/common/SUMOVTypeParameter.h>
#include <utils/common/SUMOAbstractRouter.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/iodevices/OutputDevice.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
RONet::RONet()
    : myVehicleTypes(), myDefaultVTypeMayBeDeleted(true),
      myRoutesOutput(0), myRouteAlternativesOutput(0), myTypesOutput(0),
      myReadRouteNo(0), myDiscardedRouteNo(0), myWrittenRouteNo(0),
      myHaveRestrictions(false),
      myNumInternalEdges(0) {
    SUMOVTypeParameter* type = new SUMOVTypeParameter(DEFAULT_VTYPE_ID, SVC_IGNORING);
    type->onlyReferenced = true;
    myVehicleTypes.add(type->id, type);
}


RONet::~RONet() {
    myNodes.clear();
    myEdges.clear();
    myVehicleTypes.clear();
    myRoutes.clear();
    myVehicles.clear();
}


bool
RONet::addEdge(ROEdge* edge) {
    if (!myEdges.add(edge->getID(), edge)) {
        WRITE_ERROR("The edge '" + edge->getID() + "' occurs at least twice.");
        delete edge;
        return false;
    }
    if (edge->getType() == ROEdge::ET_INTERNAL) {
        myNumInternalEdges += 1;
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


bool
RONet::addRouteDef(RORouteDef* def) {
    return myRoutes.add(def->getID(), def);
}


void
RONet::openOutput(const std::string& filename, const std::string altFilename, const std::string typeFilename) {
    if (filename != "") {
        myRoutesOutput = &OutputDevice::getDevice(filename);
        myRoutesOutput->writeHeader<ROEdge>(SUMO_TAG_ROUTES);
        myRoutesOutput->writeAttr("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance").writeAttr("xsi:noNamespaceSchemaLocation", "http://sumo-sim.org/xsd/routes_file.xsd");
    }
    if (altFilename != "") {
        myRouteAlternativesOutput = &OutputDevice::getDevice(altFilename);
        myRouteAlternativesOutput->writeHeader<ROEdge>(SUMO_TAG_ROUTES);
        myRouteAlternativesOutput->writeAttr("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance").writeAttr("xsi:noNamespaceSchemaLocation", "http://sumo-sim.org/xsd/routes_file.xsd");
    }
    if (typeFilename != "") {
        myTypesOutput = &OutputDevice::getDevice(typeFilename);
        myTypesOutput->writeXMLHeader("routes", "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"http://sumo-sim.org/xsd/routes_file.xsd\"");
    }
}


void
RONet::closeOutput() {
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
}



SUMOVTypeParameter*
RONet::getVehicleTypeSecure(const std::string& id) {
    // check whether the type was already known
    SUMOVTypeParameter* type = myVehicleTypes.get(id);
    if (id == DEFAULT_VTYPE_ID) {
        myDefaultVTypeMayBeDeleted = false;
    }
    if (type != 0) {
        return type;
    }
    VTypeDistDictType::iterator it2 = myVTypeDistDict.find(id);
    if (it2 != myVTypeDistDict.end()) {
        return it2->second->get();
    }
    if (id == "") {
        // ok, no vehicle type was given within the user input
        //  return the default type
        myDefaultVTypeMayBeDeleted = false;
        return myVehicleTypes.get(DEFAULT_VTYPE_ID);
    }
    // Assume, the user will define the type somewhere else
    //  return a type which contains the id only
    type = new SUMOVTypeParameter(id, SVC_IGNORING);
    type->onlyReferenced = true;
    addVehicleType(type);
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
    if (myVehIDs.find(id) == myVehIDs.end() && myVehicles.add(id, veh)) {
        myVehIDs.insert(id);
        myReadRouteNo++;
        return true;
    }
    WRITE_ERROR("The vehicle '" + id + "' occurs at least twice.");
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


void
RONet::addPerson(const SUMOTime depart, const std::string desc) {
    myPersons.insert(std::pair<const SUMOTime, const std::string>(depart, desc));
}


bool
RONet::computeRoute(OptionsCont& options, SUMOAbstractRouter<ROEdge, ROVehicle>& router,
                    const ROVehicle* const veh) {
    MsgHandler* mh = (OptionsCont::getOptions().getBool("ignore-errors") ?
                      MsgHandler::getWarningInstance() : MsgHandler::getErrorInstance());
    std::string noRouteMsg = "The vehicle '" + veh->getID() + "' has no valid route.";
    RORouteDef* const routeDef = veh->getRouteDefinition();
    // check if the route definition is valid
    if (routeDef == 0) {
        mh->inform(noRouteMsg);
        return false;
    }
    // check whether the route was already saved
    if (routeDef->isSaved()) {
        return true;
    }
    //
    RORoute* current = routeDef->buildCurrentRoute(router, veh->getDepartureTime(), *veh);
    if (current == 0 || current->size() == 0) {
        delete current;
        mh->inform(noRouteMsg);
        return false;
    }
    // check whether we have to evaluate the route for not containing loops
    if (options.getBool("remove-loops")) {
        current->recheckForLoops();
        // check whether the route is still valid
        if (current->size() == 0) {
            delete current;
            mh->inform(noRouteMsg + " (after removing loops)");
            return false;
        }
    }
    // add built route
    routeDef->addAlternative(router, veh, current, veh->getDepartureTime());
    return true;
}


void
RONet::checkFlows(SUMOTime time) {
    std::vector<std::string> toRemove;
    for (NamedObjectCont<SUMOVehicleParameter*>::IDMap::const_iterator i = myFlows.getMyMap().begin(); i != myFlows.getMyMap().end(); ++i) {
        SUMOVehicleParameter* pars = i->second;
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
            pars->repetitionsDone++;
            // try to build the vehicle
            SUMOVTypeParameter* type = getVehicleTypeSecure(pars->vtypeid);
            RORouteDef* route = getRouteDef(pars->routeid)->copy("!" + newPars->id);
            ROVehicle* veh = new ROVehicle(*newPars, route, type, this);
            addVehicle(newPars->id, veh);
            delete newPars;
        }
        if (pars->repetitionsDone == pars->repetitionNumber) {
            toRemove.push_back(i->first);
        }
    }
    for (std::vector<std::string>::const_iterator i = toRemove.begin(); i != toRemove.end(); ++i) {
        myFlows.erase(*i);
    }
}


SUMOTime
RONet::saveAndRemoveRoutesUntil(OptionsCont& options, SUMOAbstractRouter<ROEdge, ROVehicle>& router,
                                SUMOTime time) {
    checkFlows(time);
    SUMOTime lastTime = -1;
    // write all vehicles (and additional structures)
    while (myVehicles.size() != 0 || myPersons.size() != 0) {
        // get the next vehicle and person
        const ROVehicle* const veh = myVehicles.getTopVehicle();
        const SUMOTime vehicleTime = veh == 0 ? SUMOTime_MAX : veh->getDepartureTime();
        PersonMap::iterator person = myPersons.begin();
        const SUMOTime personTime = person == myPersons.end() ? SUMOTime_MAX : person->first;
        // check whether it shall not yet be computed
        if (vehicleTime > time && personTime > time) {
            lastTime = MIN2(vehicleTime, personTime);
            break;
        }
        if (vehicleTime < personTime) {
            // check whether to print the output
            if (lastTime != vehicleTime && lastTime != -1) {
                // report writing progress
                if (options.getInt("stats-period") >= 0 && ((int) vehicleTime % options.getInt("stats-period")) == 0) {
                    WRITE_MESSAGE("Read: " + toString(myReadRouteNo) + ",  Discarded: " + toString(myDiscardedRouteNo) + ",  Written: " + toString(myWrittenRouteNo));
                }
            }
            lastTime = vehicleTime;

            // ok, compute the route (try it)
            if (computeRoute(options, router, veh)) {
                // write the route
                veh->saveAllAsXML(*myRoutesOutput, myRouteAlternativesOutput, myTypesOutput, options.getBool("exit-times"));
                myWrittenRouteNo++;
            } else {
                myDiscardedRouteNo++;
            }
            // delete routes and the vehicle
            if (veh->getRouteDefinition()->getID()[0] == '!') {
                if (!myRoutes.erase(veh->getRouteDefinition()->getID())) {
                    delete veh->getRouteDefinition();
                }
            }
            myVehicles.erase(veh->getID());
        } else {
            myRoutesOutput->writePreformattedTag(person->second);
            if (myRouteAlternativesOutput != 0) {
                myRouteAlternativesOutput->writePreformattedTag(person->second);
            }
            myPersons.erase(person);
        }
    }
    return lastTime;
}


bool
RONet::furtherStored() {
    return myVehicles.size() > 0 || myFlows.size() > 0 || myPersons.size() > 0;
}


unsigned int
RONet::getEdgeNo() const {
    return (unsigned int) myEdges.size();
}


unsigned int
RONet::getEdgeNoWithoutInternal() const {
    return (unsigned int)(myEdges.size() - myNumInternalEdges);
}


const std::map<std::string, ROEdge*>&
RONet::getEdgeMap() const {
    return myEdges.getMyMap();
}


bool
RONet::hasRestrictions() const {
    return myHaveRestrictions;
}


void
RONet::setRestrictionFound() {
    myHaveRestrictions = true;
}



/****************************************************************************/

