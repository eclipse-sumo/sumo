/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEVariableSpeedSignStep.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Apr 2017
/// @version $Id$
///
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
#include "GNEVariableSpeedSignDialog.h"
#include "GNEChange_Attribute.h"
#include "GNEUndoList.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEVariableSpeedSignStep::GNEVariableSpeedSignStep(GNEVariableSpeedSignDialog* variableSpeedSignDialog) :
    GNEAttributeCarrier(SUMO_TAG_STEP, ICON_EMPTY),
    myVariableSpeedSignParent(variableSpeedSignDialog->getEditedVariableSpeedSign()),
    myTime(0),
    mySpeed(getDefaultValue<double>(SUMO_TAG_STEP, SUMO_ATTR_SPEED)) {
    // set Time
    if (variableSpeedSignDialog->getEditedVariableSpeedSign()->getVariableSpeedSignSteps().size() > 0) {
        myTime = variableSpeedSignDialog->getEditedVariableSpeedSign()->getVariableSpeedSignSteps().back()->getTime() + 1;
    }
}


GNEVariableSpeedSignStep::GNEVariableSpeedSignStep(GNEVariableSpeedSign* variableSpeedSignParent, double time, double speed) :
    GNEAttributeCarrier(SUMO_TAG_STEP, ICON_EMPTY),
    myVariableSpeedSignParent(variableSpeedSignParent),
    myTime(time),
    mySpeed(speed) {
}


GNEVariableSpeedSignStep::~GNEVariableSpeedSignStep() {}


GNEVariableSpeedSign*
GNEVariableSpeedSignStep::getVariableSpeedSignParent() const {
    return myVariableSpeedSignParent;
}


void
GNEVariableSpeedSignStep::writeStep(OutputDevice& device) {
    // Open VSS tag
    device.openTag(SUMO_TAG_STEP);
    // Write TimeSTep
    device.writeAttr(SUMO_ATTR_TIME, myTime);
    // Write speed
    device.writeAttr(SUMO_ATTR_SPEED, mySpeed);
    // Close VSS tag
    device.closeTag();
}


double
GNEVariableSpeedSignStep::getTime() const {
    return myTime;
}


std::string
GNEVariableSpeedSignStep::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_TIME:
            return toString(myTime);
        case SUMO_ATTR_SPEED:
            return toString(mySpeed);
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEVariableSpeedSignStep::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_TIME:
        case SUMO_ATTR_SPEED:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEVariableSpeedSignStep::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_TIME:
            return canParse<double>(value) && (parse<double>(value) >= 0);
        case SUMO_ATTR_SPEED:
            return canParse<double>(value) && (parse<double>(value) >= 0);
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNEVariableSpeedSignStep::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_TIME: {
            myTime = parse<double>(value);
            break;
        }
        case SUMO_ATTR_SPEED: {
            mySpeed = parse<double>(value);
            break;
        }
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


/****************************************************************************/
