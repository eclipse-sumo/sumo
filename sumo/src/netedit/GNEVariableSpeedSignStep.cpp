/****************************************************************************/
/// @file    GNEVariableSpeedSignStep.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Apr 2017
/// @version $Id$
///
///
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
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

#include <utils/common/ToString.h>
#include <utils/common/MsgHandler.h>

#include "GNEVariableSpeedSignStep.h"
#include "GNEVariableSpeedSign.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEVariableSpeedSignStep::GNEVariableSpeedSignStep(GNEVariableSpeedSign* variableSpeedSignParent) :
    myVariableSpeedSignParent(variableSpeedSignParent),
    myTime(0),
    mySpeed(50),
    myTag(SUMO_TAG_STEP) {
}


GNEVariableSpeedSignStep::GNEVariableSpeedSignStep(GNEVariableSpeedSign* variableSpeedSignParent, double time, double speed) :
    myVariableSpeedSignParent(variableSpeedSignParent),
    myTime(0),
    mySpeed(50),
    myTag(SUMO_TAG_STEP) {
    // use set functions to avid non valid intervals
    setTime(time);
    setSpeed(speed);
}


GNEVariableSpeedSignStep::~GNEVariableSpeedSignStep() {}


GNEVariableSpeedSign*
GNEVariableSpeedSignStep::getVariableSpeedSignParent() const {
    return myVariableSpeedSignParent;
}


SumoXMLTag
GNEVariableSpeedSignStep::getTag() const {
    return myTag;
}


double
GNEVariableSpeedSignStep::getTime() const {
    return myTime;
}


double
GNEVariableSpeedSignStep::getSpeed() const {
    return mySpeed;
}


bool
GNEVariableSpeedSignStep::setTime(double time) {
    if (time >= 0) {
        myTime = time;
        return true;
    } else {
        return false;
    }
}


bool
GNEVariableSpeedSignStep::setSpeed(double speed) {
    if (speed >= 0) {
        mySpeed = speed;
        return true;
    } else {
        return false;
    }
}


bool
GNEVariableSpeedSignStep::operator==(const GNEVariableSpeedSignStep& variableSpeedSignStep) const {
    return (myTime == variableSpeedSignStep.myTime);
}


bool
GNEVariableSpeedSignStep::operator!=(const GNEVariableSpeedSignStep& variableSpeedSignStep) const {
    return (myTime != variableSpeedSignStep.myTime);
}


bool
GNEVariableSpeedSignStep::operator>(const GNEVariableSpeedSignStep& variableSpeedSignStep) const {
    return (myTime > variableSpeedSignStep.myTime);
}


bool
GNEVariableSpeedSignStep::operator<(const GNEVariableSpeedSignStep& variableSpeedSignStep) const {
    return (myTime < variableSpeedSignStep.myTime);
}


bool
GNEVariableSpeedSignStep::operator>=(const GNEVariableSpeedSignStep& variableSpeedSignStep) const {
    return (myTime >= variableSpeedSignStep.myTime);
}


bool
GNEVariableSpeedSignStep::operator<=(const GNEVariableSpeedSignStep& variableSpeedSignStep) const {
    return (myTime <= variableSpeedSignStep.myTime);
}
/****************************************************************************/
