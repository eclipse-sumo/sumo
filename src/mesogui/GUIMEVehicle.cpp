/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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
#include <utils/gui/settings/GUIVisualizationSettings.h>
#include <microsim/logging/CastingFunctionBinding.h>
#include <microsim/logging/FunctionBinding.h>
#include <microsim/devices/MSDevice.h>
#include <guisim/GUILane.h>
#include "GUIMEVehicle.h"


// ===========================================================================
// method definitions
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4355)
#endif
GUIMEVehicle::GUIMEVehicle(SUMOVehicleParameter* pars, const MSRoute* route,
                           MSVehicleType* type, const double speedFactor) :
    MEVehicle(pars, route, type, speedFactor),
    GUIBaseVehicle((MSBaseVehicle&) * this) {
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif


GUIMEVehicle::~GUIMEVehicle() { }


GUIParameterTableWindow*
GUIMEVehicle::getParameterWindow(GUIMainWindow& app,
                                 GUISUMOAbstractView&) {
    GUIParameterTableWindow* ret =
        new GUIParameterTableWindow(app, *this, 20 + (int)getParameter().getMap().size());
    // add items
    ret->mkItem("edge [id]", false, getEdge()->getID());
    ret->mkItem("segment [#]", false, getSegment()->getIndex());
    ret->mkItem("position [m]", true, new FunctionBinding<GUIMEVehicle, double>(this, &MEVehicle::getPositionOnLane));
    ret->mkItem("speed [m/s]", true, new FunctionBinding<GUIMEVehicle, double>(this, &MEVehicle::getSpeed));
    ret->mkItem("angle [degree]", true, new FunctionBinding<GUIMEVehicle, double>(this, &GUIBaseVehicle::getNaviDegree));
    ret->mkItem("waiting time [s]", true,
                new FunctionBinding<GUIMEVehicle, double>(this, &MEVehicle::getWaitingSeconds));
    ret->mkItem("speed factor", false, getChosenSpeedFactor());
    //ret->mkItem("time gap [s]", true,
    //            new FunctionBinding<GUIMEVehicle, double>(this, &MSVehicle::getTimeGap));
    //ret->mkItem("waiting time [s]", true,
    //            new FunctionBinding<GUIMEVehicle, double>(this, &MSVehicle::getWaitingSeconds));
    //ret->mkItem("impatience", true,
    //            new FunctionBinding<GUIMEVehicle, double>(this, &MSVehicle::getImpatience));
    //ret->mkItem("last lane change [s]", true,
    //            new FunctionBinding<GUIMEVehicle, double>(this, &GUIMEVehicle::getLastLaneChangeOffset));
    ret->mkItem("desired depart [s]", false, time2string(getParameter().depart));
    ret->mkItem("depart delay [s]", false, time2string(getDepartDelay()));
    if (getParameter().repetitionNumber < std::numeric_limits<int>::max()) {
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
    //            new FunctionBinding<GUIMEVehicle, double>(this, &GUIMEVehicle::getCO2Emissions));
    //ret->mkItem("CO [mg/s]", true,
    //            new FunctionBinding<GUIMEVehicle, double>(this, &GUIMEVehicle::getCOEmissions));
    //ret->mkItem("HC [mg/s]", true,
    //            new FunctionBinding<GUIMEVehicle, double>(this, &GUIMEVehicle::getHCEmissions));
    //ret->mkItem("NOx [mg/s]", true,
    //            new FunctionBinding<GUIMEVehicle, double>(this, &GUIMEVehicle::getNOxEmissions));
    //ret->mkItem("PMx [mg/s]", true,
    //            new FunctionBinding<GUIMEVehicle, double>(this, &GUIMEVehicle::getPMxEmissions));
    //ret->mkItem("fuel [ml/s]", true,
    //            new FunctionBinding<GUIMEVehicle, double>(this, &GUIMEVehicle::getFuelConsumption));
    //ret->mkItem("noise (Harmonoise) [dB]", true,
    //            new FunctionBinding<GUIMEVehicle, double>(this, &GUIMEVehicle::getHarmonoise_NoiseEmissions));
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
    // meso specific values
    ret->mkItem("event time [s]", true, new FunctionBinding<GUIMEVehicle, double>(this, &MEVehicle::getEventTimeSeconds));
    ret->mkItem("entry time [s]", true, new FunctionBinding<GUIMEVehicle, double>(this, &MEVehicle::getLastEntryTimeSeconds));
    ret->mkItem("block time [s]", true, new FunctionBinding<GUIMEVehicle, double>(this, &MEVehicle::getBlockTimeSeconds));
    ret->mkItem("link penalty [s]", true, new FunctionBinding<GUIMEVehicle, double>(this, &MEVehicle::getCurrentLinkPenaltySeconds));
    // close building
    ret->closeBuilding(&getParameter());
    return ret;
}


GUIParameterTableWindow*
GUIMEVehicle::getTypeParameterWindow(GUIMainWindow& app,
                                     GUISUMOAbstractView&) {
    GUIParameterTableWindow* ret =
        new GUIParameterTableWindow(app, *this, 9 + (int)myType->getParameter().getMap().size());
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
    //ret->mkItem("desired headway (tau)", false, getCarFollowModel().getHeadwayTime());
    ret->mkItem("person capacity", false, myType->getPersonCapacity());
    ret->mkItem("container capacity", false, myType->getContainerCapacity());
    // close building
    ret->closeBuilding(&(myType->getParameter()));
    return ret;
}


bool
GUIMEVehicle::drawAction_drawCarriageClass(const GUIVisualizationSettings& /* s */, SUMOVehicleShape /* guiShape */, bool /* asImage */) const {
    drawAction_drawVehicleAsBoxPlus();
    return true;
}


double
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
GUIMEVehicle::drawRouteHelper(const GUIVisualizationSettings& s, const MSRoute& r) const {
    const double exaggeration = s.vehicleSize.getExaggeration(s);
    MSRouteIterator i = r.begin();
    for (; i != r.end(); ++i) {
        const GUILane* lane = static_cast<GUILane*>((*i)->getLanes()[0]);
        GLHelper::drawBoxLines(lane->getShape(), lane->getShapeRotations(), lane->getShapeLengths(), 1.0);
        GLHelper::drawBoxLines(lane->getShape(), lane->getShapeRotations(), lane->getShapeLengths(), exaggeration);
    }
}


double
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

