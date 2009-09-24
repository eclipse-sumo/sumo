/****************************************************************************/
/// @file    MSRouteHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// Parser and container for routes during their loading
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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

#include <string>
#include <map>
#include <vector>
#include <microsim/MSRoute.h>
#include <microsim/MSEdge.h>
#include <microsim/MSVehicleType.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSEdge.h>
#include <microsim/MSEmitControl.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSLane.h>
#include "MSRouteHandler.h"
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/UtilExceptions.h>
#include <utils/options/OptionsCont.h>
#include "MSNet.h"

#include <microsim/trigger/MSBusStop.h>
#include <microsim/MSGlobals.h>
#include <utils/xml/SUMOVehicleParserHelper.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
MSRouteHandler::MSRouteHandler(const std::string &file,
                               bool addVehiclesDirectly)
        : SUMOSAXHandler(file), myVehicleParameter(0),
        myLastDepart(0), myLastReadVehicle(0),
        myAddVehiclesDirectly(addVehiclesDirectly),
        myRunningVehicleNumber(0),
        myCurrentVTypeDistribution(0),
        myCurrentRouteDistribution(0),
        myHaveWarned(false), myCurrentVType(0) {
    myIncrementalBase = OptionsCont::getOptions().getInt("incremental-dua-base");
    myIncrementalStage = OptionsCont::getOptions().getInt("incremental-dua-step");
    myAmUsingIncrementalDUA = (myIncrementalStage>0);
    myActiveRoute.reserve(100);
}


MSRouteHandler::~MSRouteHandler() throw() {
    delete myVehicleParameter;
}


SUMOTime
MSRouteHandler::getLastDepart() const {
    return myLastDepart;
}


MSVehicle *
MSRouteHandler::retrieveLastReadVehicle() {
    MSVehicle *v = myLastReadVehicle;
    myLastReadVehicle = 0;
    return v;
}


void
MSRouteHandler::myStartElement(SumoXMLTag element,
                               const SUMOSAXAttributes &attrs) throw(ProcessError) {
    switch (element) {
    case SUMO_TAG_VEHICLE:
        delete myVehicleParameter;
        myVehicleParameter = 0;
        myVehicleParameter = SUMOVehicleParserHelper::parseVehicleAttributes(attrs);
        break;
    case SUMO_TAG_VTYPE:
        myCurrentVType = SUMOVehicleParserHelper::beginVTypeParsing(attrs);
        break;
    case SUMO_TAG_VTYPE_DISTRIBUTION:
        openVehicleTypeDistribution(attrs);
        break;
    case SUMO_TAG_ROUTE:
        openRoute(attrs);
        break;
    case SUMO_TAG_ROUTE_DISTRIBUTION:
        openRouteDistribution(attrs);
        break;
    case SUMO_TAG_TRIPDEF:
        myVehicleParameter = SUMOVehicleParserHelper::parseVehicleAttributes(attrs);
        myActiveRouteID = "!" + myVehicleParameter->id;
        MSEdge::parseEdgesList(attrs.getString(SUMO_ATTR_FROM), myActiveRoute, myActiveRouteID);
        MSEdge::parseEdgesList(attrs.getString(SUMO_ATTR_TO), myActiveRoute, myActiveRouteID);
        closeRoute();
        closeVehicle();
        break;
    default:
        break;
    }
    // parse embedded vtype information
    if(myCurrentVType!=0&&element!=SUMO_TAG_VTYPE) {
        SUMOVehicleParserHelper::parseVTypeEmbedded(*myCurrentVType, element, attrs);
        return;
    }

    if (element==SUMO_TAG_STOP) {
        MSVehicle::Stop stop;
        stop.lane = 0;
        stop.busstop = 0;
        // try to parse the assigne bus stop
        string bus_stop = attrs.getStringSecure(SUMO_ATTR_BUS_STOP, "");
        if (bus_stop!="") {
            // ok, we have obviously a bus stop
            MSBusStop *bs = MSNet::getInstance()->getBusStop(bus_stop);
            if (bs!=0) {
                const MSLane &l = bs->getLane();
                stop.lane = &((MSLane &) l);
                stop.busstop = bs;
                stop.pos = bs->getEndLanePosition();
            } else {
                MsgHandler::getErrorInstance()->inform("The bus stop '" + bus_stop + "' is not known.");
                return;
            }
        } else {
            // no, the lane and the position should be given
            // get the lane
            string laneS = attrs.getStringSecure(SUMO_ATTR_LANE, "");
            if (laneS!="") {
                MSLane *l = MSLane::dictionary(laneS);
                if (l==0) {
                    MsgHandler::getErrorInstance()->inform("The lane '" + laneS + "' for a stop is not known.");
                    return;
                }
                stop.lane = l;
            } else {
                MsgHandler::getErrorInstance()->inform("A stop must be placed on a bus stop or a lane.");
                return;
            }
            // get the position
            try {
                stop.pos = attrs.getFloat(SUMO_ATTR_POSITION);
            } catch (EmptyData&) {
                MsgHandler::getErrorInstance()->inform("The position of a stop is not defined.");
                return;
            } catch (NumberFormatException&) {
                MsgHandler::getErrorInstance()->inform("The position of a stop is not numeric.");
                return;
            }
        }

        // get the standing duration
        stop.until = -1;
        stop.duration = -1;
        if (!attrs.hasAttribute(SUMO_ATTR_DURATION) && !attrs.hasAttribute(SUMO_ATTR_UNTIL)) {
            MsgHandler::getErrorInstance()->inform("The duration of a stop is not defined.");
            return;
        } else {
            try {
                stop.duration = (SUMOTime) attrs.getFloatSecure(SUMO_ATTR_DURATION, -1); // time-parser
            } catch (EmptyData&) {
                MsgHandler::getErrorInstance()->inform("The duration of a stop is empty.");
                return;
            } catch (NumberFormatException&) {
                MsgHandler::getErrorInstance()->inform("The duration of a stop is not numeric.");
                return;
            }
            try {
                stop.until = (SUMOTime) attrs.getFloatSecure(SUMO_ATTR_UNTIL, -1); // time-parser
            } catch (EmptyData&) {
                MsgHandler::getErrorInstance()->inform("The end time of a stop is empty.");
                return;
            } catch (NumberFormatException&) {
                MsgHandler::getErrorInstance()->inform("The end time of a stop is not numeric.");
                return;
            }
            if (stop.duration<0&&stop.until<0) {
                MsgHandler::getErrorInstance()->inform("Neither the duration nor the end time is given for a stop.");
                return;
            }
        }
        stop.reached = false;
        myVehicleStops.push_back(stop);
    }
}


void
MSRouteHandler::openVehicleTypeDistribution(const SUMOSAXAttributes &attrs) {
    if (attrs.setIDFromAttributes("vtypeDistribution", myCurrentVTypeDistributionID)) {
        myCurrentVTypeDistribution = new RandomDistributor<MSVehicleType*>();
        if (attrs.hasAttribute(SUMO_ATTR_VTYPES)) {
            StringTokenizer st(attrs.getString(SUMO_ATTR_VTYPES));
            while (st.hasNext()) {
                string vtypeID = st.next();
                MSVehicleType *type = MSNet::getInstance()->getVehicleControl().getVType(vtypeID);
                if (type==0) {
                    throw ProcessError("Unknown vtype '" + vtypeID + "' in distribution '" + myCurrentVTypeDistributionID
                                       + "'.");
                }
                myCurrentVTypeDistribution->add(type->getDefaultProbability(), type);
            }
        }
    }
}


void
MSRouteHandler::closeVehicleTypeDistribution() {
    if (myCurrentVTypeDistribution != 0) {
        if (myCurrentVTypeDistribution->getOverallProb() == 0) {
            delete myCurrentVTypeDistribution;
            MsgHandler::getErrorInstance()->inform("Vehicle type distribution '" + myCurrentVTypeDistributionID + "' is empty.");
        } else if (!MSNet::getInstance()->getVehicleControl().addVTypeDistribution(myCurrentVTypeDistributionID, myCurrentVTypeDistribution)) {
            delete myCurrentVTypeDistribution;
            MsgHandler::getErrorInstance()->inform("Another vehicle type (or distribution) with the id '" + myCurrentVTypeDistributionID + "' exists.");
        }
        myCurrentVTypeDistribution = 0;
    }
}


void
MSRouteHandler::openRoute(const SUMOSAXAttributes &attrs) {
    // get the id
    try {
        // check whether the id is really necessary
        if (myVehicleParameter!=0) {
            // ok, a vehicle is wrapping the route,
            //  we may use this vehicle's id as default
            myActiveRouteID = "!" + myVehicleParameter->id; // !!! document this
        } else {
            myActiveRouteID = attrs.getString(SUMO_ATTR_ID);
        }
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Missing id of a route-object.");
        return;
    }
    if (attrs.hasAttribute(SUMO_ATTR_EDGES)) {
        MSEdge::parseEdgesList(attrs.getString(SUMO_ATTR_EDGES), myActiveRoute, myActiveRouteID);
    }
    myActiveRouteProbability = attrs.getFloatSecure(SUMO_ATTR_PROB, DEFAULT_VEH_PROB);
}


// ----------------------------------


void
MSRouteHandler::myCharacters(SumoXMLTag element,
                             const std::string &chars) throw(ProcessError) {
    switch (element) {
    case SUMO_TAG_ROUTE:
        if (!myHaveWarned) {
            MsgHandler::getWarningInstance()->inform("Defining routes as a nested string is deprecated, use the edges attribute instead.");
            myHaveWarned = true;
        }
        MSEdge::parseEdgesList(chars, myActiveRoute, myActiveRouteID);
        break;
    default:
        break;
    }
}


// ----------------------------------

void
MSRouteHandler::myEndElement(SumoXMLTag element) throw(ProcessError) {
    switch (element) {
    case SUMO_TAG_ROUTE:
        closeRoute();
        break;
    case SUMO_TAG_VEHICLE:
        closeVehicle();
        delete myVehicleParameter;
        myVehicleParameter = 0;
        break;
    case SUMO_TAG_VTYPE_DISTRIBUTION:
        closeVehicleTypeDistribution();
        break;
    case SUMO_TAG_ROUTE_DISTRIBUTION:
        closeRouteDistribution();
        break;
    case SUMO_TAG_VTYPE: {
        SUMOVehicleParserHelper::closeVTypeParsing(*myCurrentVType);
        MSVehicleType *vehType = MSVehicleType::build(*myCurrentVType);
        delete myCurrentVType;
        myCurrentVType = 0;
        if (!MSNet::getInstance()->getVehicleControl().addVType(vehType)) {
            std::string id = vehType->getID();
            delete vehType;
#ifdef HAVE_MESOSIM
            if (!MSGlobals::gStateLoaded) {
#endif
                throw ProcessError("Another vehicle type (or distribution) with the id '" + id + "' exists.");
#ifdef HAVE_MESOSIM
            }
#endif
        } else {
            if (myCurrentVTypeDistribution != 0) {
                myCurrentVTypeDistribution->add(vehType->getDefaultProbability(), vehType);
            }
        }
    }
    break;
    default:
        break;
    }
}


MSRoute*
MSRouteHandler::buildRoute() throw() {
    return new MSRoute(myActiveRouteID, myActiveRoute, myVehicleParameter==0||myVehicleParameter->repetitionNumber>=1);
}

void
MSRouteHandler::closeRoute() throw(ProcessError) {
    if (myActiveRoute.size()==0) {
        if (myVehicleParameter!=0) {
            throw ProcessError("Vehicle's '" + myVehicleParameter->id + "' route has no edges.");
        } else {
            throw ProcessError("Route '" + myActiveRouteID + "' has no edges.");
        }
    }
    // check whether the route is long enough
    MSRoute *route = buildRoute();
    myActiveRoute.clear();
    if (!MSRoute::dictionary(myActiveRouteID, route)) {
        delete route;
#ifdef HAVE_MESOSIM
        if (!MSGlobals::gStateLoaded) {
#endif
            if (myVehicleParameter!=0) {
                if (MSNet::getInstance()->getVehicleControl().getVehicle(myVehicleParameter->id)==0) {
                    throw ProcessError("Another route for vehicle '" + myVehicleParameter->id + "' exists.");
                } else {
                    throw ProcessError("A vehicle with id '" + myVehicleParameter->id + "' already exists.");
                }
            } else {
                throw ProcessError("Another route (or distribution) with the id '" + myActiveRouteID + "' exists.");
            }
#ifdef HAVE_MESOSIM
        }
#endif
    } else {
        if (myCurrentRouteDistribution != 0) {
            myCurrentRouteDistribution->add(myActiveRouteProbability, route);
        }
    }
    myActiveRouteID = "";
}


void
MSRouteHandler::openRouteDistribution(const SUMOSAXAttributes &attrs) {
    if (attrs.setIDFromAttributes("routeDistribution", myCurrentRouteDistributionID)) {
        myCurrentRouteDistribution = new RandomDistributor<const MSRoute*>();
        if (attrs.hasAttribute(SUMO_ATTR_ROUTES)) {
            StringTokenizer st(attrs.getString(SUMO_ATTR_ROUTES));
            while (st.hasNext()) {
                string routeID = st.next();
                const MSRoute *route = MSRoute::dictionary(routeID);
                if (route==0) {
                    throw ProcessError("Unknown route '" + routeID + "' in distribution '" + myCurrentRouteDistributionID
                                       + "'.");
                }
                myCurrentRouteDistribution->add(1., route, false);
            }
        }
    }
}


void
MSRouteHandler::closeRouteDistribution() {
    if (myCurrentRouteDistribution != 0) {
        if (myCurrentRouteDistribution->getOverallProb() == 0) {
            delete myCurrentRouteDistribution;
            MsgHandler::getErrorInstance()->inform("Route distribution '" + myCurrentRouteDistributionID + "' is empty.");
        } else if (!MSRoute::dictionary(myCurrentRouteDistributionID, myCurrentRouteDistribution)) {
            delete myCurrentRouteDistribution;
            MsgHandler::getErrorInstance()->inform("Another route (or distribution) with the id '" + myCurrentRouteDistributionID + "' exists.");
        }
        myCurrentRouteDistribution = 0;
    }
}


bool
MSRouteHandler::closeVehicle() throw(ProcessError) {
    myLastDepart = myVehicleParameter->depart;
    // let's check whether this vehicle had to be emitted before the simulation starts
    if (myVehicleParameter->depart<OptionsCont::getOptions().getInt("begin")) {
        // yes, but maybe it's a repeating vehicle...
        if (myVehicleParameter->repetitionNumber>=0&&myVehicleParameter->repetitionOffset>=0) {
            while (myVehicleParameter->depart<OptionsCont::getOptions().getInt("begin") && myVehicleParameter->repetitionNumber>=0) {
                --myVehicleParameter->repetitionNumber;
                myVehicleParameter->depart += myVehicleParameter->repetitionOffset;
            }
        }
        if (myVehicleParameter->depart<OptionsCont::getOptions().getInt("begin")) {
            myVehicleStops.clear();
            return false;
        }
    }
    // get the vehicle's type
    MSVehicleType *vtype = 0;
    if (myVehicleParameter->vtypeid!="") {
        vtype = MSNet::getInstance()->getVehicleControl().getVType(myVehicleParameter->vtypeid);
        if (vtype==0) {
            throw ProcessError("The vehicle type '" + myVehicleParameter->vtypeid + "' for vehicle '" + myVehicleParameter->id + "' is not known.");
        }
    } else {
        // there should be one (at least the default one)
        vtype = MSNet::getInstance()->getVehicleControl().getVType();
    }
    // get the vehicle's route
    //  maybe it was explicitely assigned to the vehicle
    const MSRoute *route = MSRoute::dictionary("!" + myVehicleParameter->id);
    if (route==0) {
        // if not, try via the (hopefully) given route-id
        route = MSRoute::dictionary(myVehicleParameter->routeid);
    }
    if (route==0) {
        // nothing found? -> error
        if (myVehicleParameter->routeid!="") {
            throw ProcessError("The route '" + myVehicleParameter->routeid + "' for vehicle '" + myVehicleParameter->id + "' is not known.");
        } else {
            throw ProcessError("Vehicle '" + myVehicleParameter->id + "' has no route.");
        }
    }
    myActiveRouteID = "";

    // try to build the vehicle
    MSVehicle *vehicle = 0;
    if (MSNet::getInstance()->getVehicleControl().getVehicle(myVehicleParameter->id)==0) {
        // ok there was no other vehicle with the same id, yet
        // maybe we do not want this vehicle to be emitted due to using incremental dua
        bool add = true;
        if (myAmUsingIncrementalDUA) {
            if ((int)(myRunningVehicleNumber%myIncrementalBase)>=(int) myIncrementalStage) {
                add = false;
            }
            myRunningVehicleNumber++;
        }
        if (add) {
            vehicle =
                MSNet::getInstance()->getVehicleControl().buildVehicle(myVehicleParameter, route, vtype);
            // add the vehicle to the vehicle control
            MSNet::getInstance()->getVehicleControl().addVehicle(myVehicleParameter->id, vehicle);
            myVehicleParameter = 0;
        }
    } else {
        // strange: another vehicle with the same id already exists
#ifdef HAVE_MESOSIM
        if (!MSGlobals::gStateLoaded) {
#endif
            // and was not loaded while loading a simulation state
            // -> error
            throw ProcessError("Another vehicle with the id '" + myVehicleParameter->id + "' exists.");
#ifdef HAVE_MESOSIM
        } else {
            // ok, it seems to be loaded previously while loading a simulation state
            vehicle = 0;
        }
#endif
    }
    // check whether the vehicle shall be added directly to the network or
    //  shall stay in the internal buffer
    if (myAddVehiclesDirectly&&vehicle!=0) {
        MSNet::getInstance()->myEmitter->add(vehicle);
    } else {
        myLastReadVehicle = vehicle;
    }
    if (vehicle!=0) {
        for (std::vector<MSVehicle::Stop>::iterator i=myVehicleStops.begin(); i!=myVehicleStops.end(); ++i) {
            if (!vehicle->addStop(*i)) {
                throw ProcessError("Stop for vehicle '" + myVehicleParameter->id +
                                   "' on lane '" + i->lane->getID() + "' is not downstream the current route.");
            }
        }
    }
    myVehicleStops.clear();
    return true;
}



/****************************************************************************/
