/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifdef HAVE_VERSION_H
#include <version.h>
#endif

#include <sstream>
#include <utils/common/TplConvert.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/xml/SUMOVehicleParserHelper.h>
#include <microsim/devices/MSDevice_Routing.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSNet.h>
#include <microsim/MSVehicleTransfer.h>
#include <microsim/MSInsertionControl.h>
#include <microsim/MSRoute.h>
#include <microsim/MSVehicleControl.h>
#include "MSStateHandler.h"

#include <mesosim/MESegment.h>
#include <mesosim/MELoop.h>


// ===========================================================================
// method definitions
// ===========================================================================
MSStateHandler::MSStateHandler(const std::string& file, const SUMOTime offset) :
    MSRouteHandler(file, true),
    myOffset(offset),
    mySegment(0),
    myEdgeAndLane(0, -1),
    myAttrs(0),
    myLastParameterised(0) {
    myAmLoadingState = true;
    const std::vector<std::string> vehIDs = OptionsCont::getOptions().getStringVector("load-state.remove-vehicles");
    myVehiclesToRemove.insert(vehIDs.begin(), vehIDs.end());
}


MSStateHandler::~MSStateHandler() {
}


void
MSStateHandler::saveState(const std::string& file, SUMOTime step) {
    OutputDevice& out = OutputDevice::getDevice(file);
    out.writeHeader<MSEdge>(SUMO_TAG_SNAPSHOT);
    out.writeAttr("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance").writeAttr("xsi:noNamespaceSchemaLocation", "http://sumo.dlr.de/xsd/state_file.xsd");
    out.writeAttr(SUMO_ATTR_VERSION, VERSION_STRING).writeAttr(SUMO_ATTR_TIME, time2string(step));
    MSRoute::dict_saveState(out);
    MSNet::getInstance()->getVehicleControl().saveState(out);
    MSVehicleTransfer::getInstance()->saveState(out);
    if (MSGlobals::gUseMesoSim) {
        for (int i = 0; i < MSEdge::dictSize(); i++) {
            for (MESegment* s = MSGlobals::gMesoNet->getSegmentForEdge(*MSEdge::getAllEdges()[i]); s != 0; s = s->getNextSegment()) {
                s->saveState(out);
            }
        }
    } else {
        for (int i = 0; i < MSEdge::dictSize(); i++) {
            const std::vector<MSLane*>& lanes = MSEdge::getAllEdges()[i]->getLanes();
            for (std::vector<MSLane*>::const_iterator it = lanes.begin(); it != lanes.end(); ++it) {
                (*it)->saveState(out);
            }
        }
    }
    out.close();
}


void
MSStateHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
    MSRouteHandler::myStartElement(element, attrs);
    MSVehicleControl& vc = MSNet::getInstance()->getVehicleControl();
    switch (element) {
        case SUMO_TAG_SNAPSHOT: {
            myTime = string2time(attrs.getString(SUMO_ATTR_TIME));
            const std::string& version = attrs.getString(SUMO_ATTR_VERSION);
            if (version != VERSION_STRING) {
                WRITE_WARNING("State was written with sumo version " + version + " (present: " + VERSION_STRING + ")!");
            }
            break;
        }
        case SUMO_TAG_DELAY: {
            vc.setState(attrs.getInt(SUMO_ATTR_NUMBER),
                        attrs.getInt(SUMO_ATTR_BEGIN),
                        attrs.getInt(SUMO_ATTR_END),
                        attrs.getFloat(SUMO_ATTR_DEPART),
                        attrs.getFloat(SUMO_ATTR_TIME));
            break;
        }
        case SUMO_TAG_VTYPE: {
            myLastParameterised = myCurrentVType;
            break;
        }
        case SUMO_TAG_VEHICLE: {
            myLastParameterised = myVehicleParameter;
            myAttrs = attrs.clone();
            break;
        }
        case SUMO_TAG_DEVICE: {
            myDeviceAttrs.push_back(attrs.clone());
            break;
        }
        case SUMO_TAG_VEHICLETRANSFER: {
            MSVehicleTransfer::getInstance()->loadState(attrs, myOffset, vc);
            break;
        }
        case SUMO_TAG_SEGMENT: {
            if (mySegment == 0) {
                mySegment = MSGlobals::gMesoNet->getSegmentForEdge(*MSEdge::getAllEdges()[0]);
            } else if (mySegment->getNextSegment() == 0) {
                mySegment = MSGlobals::gMesoNet->getSegmentForEdge(*MSEdge::getAllEdges()[mySegment->getEdge().getNumericalID() + 1]);
            } else {
                mySegment = mySegment->getNextSegment();
            }
            myQueIndex = 0;
            break;
        }
        case SUMO_TAG_LANE: {
            myEdgeAndLane.second++;
            if (myEdgeAndLane.second == (int)MSEdge::getAllEdges()[myEdgeAndLane.first]->getLanes().size()) {
                myEdgeAndLane.first++;
                myEdgeAndLane.second = 0;
            }
            break;
        }
        case SUMO_TAG_VIEWSETTINGS_VEHICLES: {
            std::vector<std::string> vehIDs;
            SUMOSAXAttributes::parseStringVector(attrs.getString(SUMO_ATTR_VALUE), vehIDs);
            if (MSGlobals::gUseMesoSim) {
                mySegment->loadState(vehIDs, MSNet::getInstance()->getVehicleControl(), TplConvert::_2long(attrs.getString(SUMO_ATTR_TIME).c_str()) - myOffset, myQueIndex++);
            } else {
                MSEdge::getAllEdges()[myEdgeAndLane.first]->getLanes()[myEdgeAndLane.second]->loadState(
                    vehIDs, MSNet::getInstance()->getVehicleControl());
            }
            break;
        }
        case SUMO_TAG_PARAM: {
            bool ok;
            const std::string key = attrs.get<std::string>(SUMO_ATTR_KEY, 0, ok);
            // circumventing empty string test
            const std::string val = attrs.hasAttribute(SUMO_ATTR_VALUE) ? attrs.getString(SUMO_ATTR_VALUE) : "";
            assert(myLastParameterised != 0);
            if (myLastParameterised != 0) {
                myLastParameterised->setParameter(key, val);
            }
            break;
        }
        default:
            break;
    }
}


void
MSStateHandler::myEndElement(int element) {
    MSRouteHandler::myEndElement(element);
    if (element != SUMO_TAG_PARAM && myVehicleParameter == 0 && myCurrentVType == 0) {
        myLastParameterised = 0;
    }
}


void
MSStateHandler::closeVehicle() {
    assert(myVehicleParameter != 0);
    myVehicleParameter->depart -= myOffset;
    // the vehicle was already counted in MSVehicleControl::setState
    MSVehicleControl& vc = MSNet::getInstance()->getVehicleControl();
    // make a copy because myVehicleParameter is reset in closeVehicle()
    const std::string vehID = myVehicleParameter->id;
    if (myVehiclesToRemove.count(vehID) == 0) {
        MSRouteHandler::closeVehicle();
        // reset depart
        vc.discountStateLoaded();
        SUMOVehicle* v = vc.getVehicle(vehID);
        v->loadState(*myAttrs, myOffset);
        if (v->hasDeparted()) {
            // vehicle already departed: disable pre-insertion rerouting and enable regular routing behavior
            MSDevice_Routing* routingDevice = static_cast<MSDevice_Routing*>(v->getDevice(typeid(MSDevice_Routing)));
            if (routingDevice != 0) {
                routingDevice->notifyEnter(*v, MSMoveReminder::NOTIFICATION_DEPARTED);
            }
            MSNet::getInstance()->getInsertionControl().alreadyDeparted(v);
        }
        while (!myDeviceAttrs.empty()) {
            const std::string attrID = myDeviceAttrs.back()->getString(SUMO_ATTR_ID);
            for (std::vector<MSDevice*>::const_iterator dev = v->getDevices().begin(); dev != v->getDevices().end(); ++dev) {
                if ((*dev)->getID() == attrID) {
                    (*dev)->loadState(*myDeviceAttrs.back());
                }
            }
            delete myDeviceAttrs.back();
            myDeviceAttrs.pop_back();
        }
    } else {
        vc.discountStateLoaded(true);
        delete myVehicleParameter;
        myVehicleParameter = 0;
    }
    delete myAttrs;
}



/****************************************************************************/
