/****************************************************************************/
/// @file    GUIMEVehicle.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// A MSVehicle extended by some values for usage within the gui
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

#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/emissions/PollutantsInterface.h>
#include <microsim/logging/CastingFunctionBinding.h>
#include <microsim/logging/FunctionBinding.h>
#include <microsim/devices/MSDevice.h>
#include <guisim/GUILane.h>
#include "GUIMEVehicle.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4355)
#endif
GUIMEVehicle::GUIMEVehicle(SUMOVehicleParameter* pars, const MSRoute* route,
                           const MSVehicleType* type, const SUMOReal speedFactor) :
    MEVehicle(pars, route, type, speedFactor),
    GUIBaseVehicle((MSBaseVehicle&) * this) {
}
#ifdef _MSC_VER
#pragma warning(default: 4355)
#endif


GUIMEVehicle::~GUIMEVehicle() { }


GUIParameterTableWindow*
GUIMEVehicle::getParameterWindow(GUIMainWindow& app,
                                 GUISUMOAbstractView&) {
    GUIParameterTableWindow* ret =
        new GUIParameterTableWindow(app, *this, 21);
    // add items
    ret->mkItem("edge [id]", false, getEdge()->getID());
    ret->mkItem("segment [#]", false, getSegment()->getIndex());
    ret->mkItem("position [m]", true, new FunctionBinding<GUIMEVehicle, SUMOReal>(this, &MEVehicle::getPositionOnLane));
    ret->mkItem("speed [m/s]", true, new FunctionBinding<GUIMEVehicle, SUMOReal>(this, &MEVehicle::getSpeed));
    ret->mkItem("angle [degree]", true, new FunctionBinding<GUIMEVehicle, SUMOReal>(this, &GUIBaseVehicle::getNaviDegree));
    ret->mkItem("waiting time [s]", true,
                new FunctionBinding<GUIMEVehicle, SUMOReal>(this, &MEVehicle::getWaitingSeconds));
    if (getChosenSpeedFactor() != 1) {
        ret->mkItem("speed factor", false, getChosenSpeedFactor());
    }
    //ret->mkItem("time gap [s]", true,
    //            new FunctionBinding<GUIMEVehicle, SUMOReal>(this, &MSVehicle::getTimeGap));
    //ret->mkItem("waiting time [s]", true,
    //            new FunctionBinding<GUIMEVehicle, SUMOReal>(this, &MSVehicle::getWaitingSeconds));
    //ret->mkItem("impatience", true,
    //            new FunctionBinding<GUIMEVehicle, SUMOReal>(this, &MSVehicle::getImpatience));
    //ret->mkItem("last lane change [s]", true,
    //            new FunctionBinding<GUIMEVehicle, SUMOReal>(this, &GUIMEVehicle::getLastLaneChangeOffset));
    ret->mkItem("desired depart [s]", false, time2string(getParameter().depart));
    ret->mkItem("depart delay [s]", false, time2string(getDepartDelay()));
    if (getParameter().repetitionNumber < INT_MAX) {
        ret->mkItem("remaining [#]", false, (int) getParameter().repetitionNumber - getParameter().repetitionsDone);
    }
    if (getParameter().repetitionOffset > 0) {
        ret->mkItem("insertion period [s]", false, time2string(getParameter().repetitionOffset));
    }
    if (getParameter().repetitionProbability > 0) {
        ret->mkItem("insertion probability", false, getParameter().repetitionProbability);
    }
    //ret->mkItem("stop info", false, getStopInfo());
    ret->mkItem("line", false, myParameter->line);
    //ret->mkItem("CO2 [mg/s]", true,
    //            new FunctionBinding<GUIMEVehicle, SUMOReal>(this, &GUIMEVehicle::getCO2Emissions));
    //ret->mkItem("CO [mg/s]", true,
    //            new FunctionBinding<GUIMEVehicle, SUMOReal>(this, &GUIMEVehicle::getCOEmissions));
    //ret->mkItem("HC [mg/s]", true,
    //            new FunctionBinding<GUIMEVehicle, SUMOReal>(this, &GUIMEVehicle::getHCEmissions));
    //ret->mkItem("NOx [mg/s]", true,
    //            new FunctionBinding<GUIMEVehicle, SUMOReal>(this, &GUIMEVehicle::getNOxEmissions));
    //ret->mkItem("PMx [mg/s]", true,
    //            new FunctionBinding<GUIMEVehicle, SUMOReal>(this, &GUIMEVehicle::getPMxEmissions));
    //ret->mkItem("fuel [ml/s]", true,
    //            new FunctionBinding<GUIMEVehicle, SUMOReal>(this, &GUIMEVehicle::getFuelConsumption));
    //ret->mkItem("noise (Harmonoise) [dB]", true,
    //            new FunctionBinding<GUIMEVehicle, SUMOReal>(this, &GUIMEVehicle::getHarmonoise_NoiseEmissions));
    std::ostringstream str;
    for (std::vector<MSDevice*>::const_iterator i = myDevices.begin(); i != myDevices.end(); ++i) {
        if (i != myDevices.begin()) {
            str << ' ';
        }
        str << (*i)->getID().substr(0, (*i)->getID().find(getID()));
    }
    ret->mkItem("devices", false, str.str());
    //ret->mkItem("persons", true,
    //            new FunctionBinding<GUIMEVehicle, int>(this, &GUIMEVehicle::getPersonNumber));
    //ret->mkItem("containers", true,
    //            new FunctionBinding<GUIMEVehicle, int>(this, &GUIMEVehicle::getContainerNumber));
    ret->mkItem("parameters [key:val]", false, toString(getParameter().getMap()));

    // meso specific values
    ret->mkItem("event time [s]", true, new FunctionBinding<GUIMEVehicle, SUMOReal>(this, &MEVehicle::getEventTimeSeconds));
    ret->mkItem("entry time [s]", true, new FunctionBinding<GUIMEVehicle, SUMOReal>(this, &MEVehicle::getLastEntryTimeSeconds));
    ret->mkItem("block time [s]", true, new FunctionBinding<GUIMEVehicle, SUMOReal>(this, &MEVehicle::getBlockTimeSeconds));
    ret->mkItem("link penalty [s]", true, new FunctionBinding<GUIMEVehicle, SUMOReal>(this, &MEVehicle::getCurrentLinkPenaltySeconds));
    // close building
    ret->closeBuilding();
    return ret;
}


GUIParameterTableWindow*
GUIMEVehicle::getTypeParameterWindow(GUIMainWindow& app,
                                     GUISUMOAbstractView&) {
    GUIParameterTableWindow* ret =
        new GUIParameterTableWindow(app, *this, 10);
    // add items
    ret->mkItem("Type Information:", false, "");
    ret->mkItem("type [id]", false, myType->getID());
    ret->mkItem("length", false, myType->getLength());
    ret->mkItem("minGap", false, myType->getMinGap());
    ret->mkItem("vehicle class", false, SumoVehicleClassStrings.getString(myType->getVehicleClass()));
    ret->mkItem("emission class", false, PollutantsInterface::getName(myType->getEmissionClass()));
    ret->mkItem("maximum speed [m/s]", false, getMaxSpeed());
    //ret->mkItem("maximum acceleration [m/s^2]", false, getCarFollowModel().getMaxAccel());
    //ret->mkItem("maximum deceleration [m/s^2]", false, getCarFollowModel().getMaxDecel());
    //ret->mkItem("imperfection (sigma)", false, getCarFollowModel().getImperfection());
    //ret->mkItem("reaction time (tau)", false, getCarFollowModel().getHeadwayTime());
    ret->mkItem("person capacity", false, myType->getPersonCapacity());
    ret->mkItem("container capacity", false, myType->getContainerCapacity());

    ret->mkItem("type parameters [key:val]", false, toString(myType->getParameter().getMap()));
    // close building
    ret->closeBuilding();
    return ret;
}


bool
GUIMEVehicle::drawAction_drawCarriageClass(const GUIVisualizationSettings& /* s */, SUMOVehicleShape /* guiShape */, bool /* asImage */) const {
    drawAction_drawVehicleAsBoxPlus();
    return true;
}


SUMOReal
GUIMEVehicle::getColorValue(int activeScheme) const {
    switch (activeScheme) {
        case 8:
            return getSpeed();
        case 9:
            return getWaitingSeconds();
        case 11:
            return 0; // invalid getLastLaneChangeOffset();
        case 12:
            return getSegment()->getEdge().getVehicleMaxSpeed(this);
        case 13:
            return 0; // invalid getCO2Emissions();
        case 14:
            return 0; // invalid getCOEmissions();
        case 15:
            return 0; // invalid getPMxEmissions();
        case 16:
            return 0; // invalid  getNOxEmissions();
        case 17:
            return 0; // invalid getHCEmissions();
        case 18:
            return 0; // invalid getFuelConsumption();
        case 19:
            return 0; // invalid getHarmonoise_NoiseEmissions();
        case 20: // !!! unused!?
            if (getNumberReroutes() == 0) {
                return -1;
            }
            return getNumberReroutes();
        case 21:
            return gSelected.isSelected(GLO_VEHICLE, getGlID());
        case 22:
            return 0; // invalid getBestLaneOffset();
        case 23:
            return 0; // invalid getAcceleration();
        case 24:
            return 0; // invalid getTimeGapOnLane();
        case 25:
            return STEPS2TIME(getDepartDelay());
    }
    return 0;
}



void
GUIMEVehicle::drawRouteHelper(const MSRoute& r, SUMOReal exaggeration) const {
    MSRouteIterator i = r.begin();
    for (; i != r.end(); ++i) {
        const GUILane* lane = static_cast<GUILane*>((*i)->getLanes()[0]);
        GLHelper::drawBoxLines(lane->getShape(), lane->getShapeRotations(), lane->getShapeLengths(), 1.0);
        GLHelper::drawBoxLines(lane->getShape(), lane->getShapeRotations(), lane->getShapeLengths(), exaggeration);
    }
}


SUMOReal
GUIMEVehicle::getLastLaneChangeOffset() const {
    // @todo possibly we could compute something reasonable here
    return 0;
}


std::string
GUIMEVehicle::getStopInfo() const {
    std::string result = "";
    if (isParking()) {
        result += "parking";
    } else if (isStopped()) {
        result += "stopped";
    } else {
        return "";
    }
    return result;
}


void
GUIMEVehicle::selectBlockingFoes() const {
    // @todo possibly we could compute something reasonable here
}
/****************************************************************************/

