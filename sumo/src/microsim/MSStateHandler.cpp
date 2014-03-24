/****************************************************************************/
/// @file    MSStateHandler.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Thu, 13 Dec 2012
/// @version $Id$
///
// Parser and output filter for routes and vehicles state saving and loading
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 200122014 DLR (http://www.dlr.de/) and contributors
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

#include <sstream>
#include <utils/iodevices/OutputDevice.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/xml/SUMOVehicleParserHelper.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSNet.h>
#include <microsim/MSInsertionControl.h>
#include <microsim/MSRoute.h>
#include "MSStateHandler.h"

#ifdef HAVE_INTERNAL
#include <mesosim/MESegment.h>
#include <mesosim/MELoop.h>
#endif


#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
MSStateHandler::MSStateHandler(const std::string& file, const SUMOTime offset) :
    SUMOSAXHandler(file), myOffset(offset),
#ifdef HAVE_INTERNAL
    mySegment(0),
#endif
    myEdgeAndLane(0, -1),
    myCurrentVType(0) {
}


MSStateHandler::~MSStateHandler() {
}


void
MSStateHandler::saveState(const std::string& file, SUMOTime step) {
    OutputDevice& out = OutputDevice::getDevice(file);
    out.writeHeader<MSEdge>(SUMO_TAG_SNAPSHOT);
    out.writeAttr(SUMO_ATTR_VERSION, VERSION_STRING).writeAttr(SUMO_ATTR_TIME, step);
    MSRoute::dict_saveState(out);
    MSNet::getInstance()->getVehicleControl().saveState(out);
    if (MSGlobals::gUseMesoSim) {
#ifdef HAVE_INTERNAL
        for (size_t i = 0; i < MSEdge::dictSize(); i++) {
            for (MESegment* s = MSGlobals::gMesoNet->getSegmentForEdge(*MSEdge::dictionary(i)); s != 0; s = s->getNextSegment()) {
                s->saveState(out);
            }
        }
#endif
    } else {
        for (size_t i = 0; i < MSEdge::dictSize(); i++) {
            const std::vector<MSLane*>& lanes = MSEdge::dictionary(i)->getLanes();
            for (std::vector<MSLane*>::const_iterator it = lanes.begin(); it != lanes.end(); ++it) {
                (*it)->saveState(out);
            }
        }
    }
}


void
MSStateHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
    MSVehicleControl& vc = MSNet::getInstance()->getVehicleControl();
    switch (element) {
        case SUMO_TAG_SNAPSHOT: {
            myTime = attrs.getInt(SUMO_ATTR_TIME);
            const std::string& version = attrs.getString(SUMO_ATTR_VERSION);
            if (version != VERSION_STRING) {
                WRITE_WARNING("State was written with sumo version " + version + " (present: " + VERSION_STRING + ")!");
            }
            break;
        }
        case SUMO_TAG_DELAY: {
            vc.setState(attrs.getInt(SUMO_ATTR_NUMBER), attrs.getInt(SUMO_ATTR_END),
                        attrs.getFloat(SUMO_ATTR_DEPART), attrs.getFloat(SUMO_ATTR_TIME));
            break;
        }
        case SUMO_TAG_ROUTE: {
            const std::string id = attrs.getString(SUMO_ATTR_ID);
            if (MSRoute::dictionary(id) == 0) {
                MSEdgeVector edges;
                MSEdge::parseEdgesList(attrs.getString(SUMO_ATTR_EDGES), edges, id);
                MSRoute* r = new MSRoute(id, edges, attrs.getBool(SUMO_ATTR_STATE),
                                         0, std::vector<SUMOVehicleParameter::Stop>());
                MSRoute::dictionary(id, r);
            }
            break;
        }
        case SUMO_TAG_ROUTE_DISTRIBUTION: {
            const std::string id = attrs.getString(SUMO_ATTR_ID);
            if (MSRoute::dictionary(id) == 0) {
                RandomDistributor<const MSRoute*>* dist = new RandomDistributor<const MSRoute*>();
                std::vector<std::string> routeIDs;
                std::istringstream iss(attrs.getString(SUMO_ATTR_PROBS));
                SUMOSAXAttributes::parseStringVector(attrs.getString(SUMO_ATTR_ROUTES), routeIDs);
                for (std::vector<std::string>::const_iterator it = routeIDs.begin(); it != routeIDs.end(); ++it) {
                    SUMOReal prob;
                    iss >> prob;
                    const MSRoute* r = MSRoute::dictionary(*it);
                    assert(r != 0);
                    dist->add(prob, r, false);
                    r->addReference();
                }
                MSRoute::dictionary(id, dist, attrs.getBool(SUMO_ATTR_STATE));
            }
            break;
        }
        case SUMO_TAG_VTYPE: {
            myCurrentVType = SUMOVehicleParserHelper::beginVTypeParsing(attrs, getFileName());
            break;
        }
        case SUMO_TAG_VTYPE_DISTRIBUTION: {
            const std::string id = attrs.getString(SUMO_ATTR_ID);
            if (vc.getVType(id) == 0) {
                RandomDistributor<MSVehicleType*>* dist = new RandomDistributor<MSVehicleType*>();
                std::vector<std::string> typeIDs;
                std::istringstream iss(attrs.getString(SUMO_ATTR_PROBS));
                SUMOSAXAttributes::parseStringVector(attrs.getString(SUMO_ATTR_VTYPES), typeIDs);
                for (std::vector<std::string>::const_iterator it = typeIDs.begin(); it != typeIDs.end(); ++it) {
                    SUMOReal prob;
                    iss >> prob;
                    MSVehicleType* t = vc.getVType(*it);
                    assert(t != 0);
                    dist->add(prob, t, false);
                }
                vc.addVTypeDistribution(id, dist);
            }
            break;
        }
        case SUMO_TAG_VEHICLE: {
            SUMOVehicleParameter* p = new SUMOVehicleParameter();
            p->id = attrs.getString(SUMO_ATTR_ID);
            p->depart = attrs.getInt(SUMO_ATTR_DEPART) - myOffset;
            p->routeid = attrs.getString(SUMO_ATTR_ROUTE);
            p->vtypeid = attrs.getString(SUMO_ATTR_TYPE);
            const MSRoute* route = MSRoute::dictionary(p->routeid);
            const MSVehicleType* type = vc.getVType(p->vtypeid);
            assert(route != 0);
            assert(type != 0);
            assert(vc.getVehicle(p->id) == 0);

            SUMOVehicle* v = vc.buildVehicle(p, route, type);
            v->loadState(attrs, myOffset);
            if (!vc.addVehicle(p->id, v)) {
                throw ProcessError("Error: Could not build vehicle " + p->id + "!");
            }
            if (!v->hasDeparted()) {
                // !!! the save did not keep the order in which the vehicles are checked for insertion
                MSNet::getInstance()->getInsertionControl().add(v);
            }
            break;
        }
#ifdef HAVE_INTERNAL
        case SUMO_TAG_SEGMENT: {
            if (mySegment == 0) {
                mySegment = MSGlobals::gMesoNet->getSegmentForEdge(*MSEdge::dictionary(0));
            } else if (mySegment->getNextSegment() == 0) {
                mySegment = MSGlobals::gMesoNet->getSegmentForEdge(*MSEdge::dictionary(mySegment->getEdge().getNumericalID() + 1));
            } else {
                mySegment = mySegment->getNextSegment();
            }
            myQueIndex = 0;
            break;
        }
#endif
        case SUMO_TAG_LANE: {
            myEdgeAndLane.second++;
            if (myEdgeAndLane.second == (int)MSEdge::dictionary(myEdgeAndLane.first)->getLanes().size()) {
                myEdgeAndLane.first++;
                myEdgeAndLane.second = 0;
            }
            break;
        }
        case SUMO_TAG_VIEWSETTINGS_VEHICLES: {
            std::vector<std::string> vehIDs;
            SUMOSAXAttributes::parseStringVector(attrs.getString(SUMO_ATTR_VALUE), vehIDs);
            if (MSGlobals::gUseMesoSim) {
#ifdef HAVE_INTERNAL
                mySegment->loadState(vehIDs, MSNet::getInstance()->getVehicleControl(), attrs.getInt(SUMO_ATTR_TIME) - myOffset, myQueIndex++);
#endif
            } else {
                MSEdge::dictionary(myEdgeAndLane.first)->getLanes()[myEdgeAndLane.second]->loadState(
                    vehIDs, MSNet::getInstance()->getVehicleControl());
            }
            break;
        }
        default:
            // parse embedded vtype information
            if (myCurrentVType != 0) {
                SUMOVehicleParserHelper::parseVTypeEmbedded(*myCurrentVType, element, attrs);
            }
            break;
    }
}


void
MSStateHandler::myEndElement(int element) {
    switch (element) {
        case SUMO_TAG_VTYPE:
            MSNet::getInstance()->getVehicleControl().addVType(MSVehicleType::build(*myCurrentVType));
            delete myCurrentVType;
            myCurrentVType = 0;
            break;
        default:
            break;
    }
}


/****************************************************************************/
