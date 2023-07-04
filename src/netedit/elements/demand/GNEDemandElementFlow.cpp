/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    GNEDemandElementFlow.h
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2023
///
// An auxiliar, asbtract class for flow elements (vehicles, person and containers)
/****************************************************************************/
#include <config.h>

#include <netedit/GNEUndoList.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/div/GLHelper.h>

#include "GNEDemandElementFlow.h"
#include "GNEDemandElement.h"

// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEDemandElement - methods
// ---------------------------------------------------------------------------

GNEDemandElementFlow::GNEDemandElementFlow() {}


GNEDemandElementFlow::GNEDemandElementFlow(const SUMOVehicleParameter& vehicleParameters) :
    SUMOVehicleParameter(vehicleParameters) {
}


GNEDemandElementFlow::~GNEDemandElementFlow() {}


void
GNEDemandElementFlow::drawFlowLabel(const Position& position, const double rotation, const double width, const double length, const double exaggeration) const {
    // declare contour width
    const double contourWidth = (0.05 * exaggeration);
    // Push matrix
    GLHelper::pushMatrix();
    // Traslate to  bot
    glTranslated(position.x(), position.y(), GLO_PERSONFLOW + 0.1);
    // glTranslated(position.x(), position.y(), GLO_ROUTE + getType() + 0.1 + GLO_PERSONFLOW + 0.1);
    glRotated(rotation, 0, 0, -1);
    glTranslated(-1 * ((width * 0.5 * exaggeration) + (0.35 * exaggeration)), 0, 0);
    // draw external box
    GLHelper::setColor(RGBColor::GREY);
    GLHelper::drawBoxLine(Position(), Position(), 0, (length * exaggeration), 0.3 * exaggeration);
    // draw internal box
    glTranslated(0, 0, 0.1);
    GLHelper::setColor(RGBColor::CYAN);
    GLHelper::drawBoxLine(Position(0, -contourWidth), Position(0, -contourWidth), 0, (length * exaggeration) - (contourWidth * 2), (0.3 * exaggeration) - contourWidth);
    // draw stack label
    GLHelper::drawText("Flow", Position(0, length * exaggeration * -0.5), (.1 * exaggeration), (0.6 * exaggeration), RGBColor::BLACK, 90, 0, -1);
    // pop draw matrix
    GLHelper::popMatrix();
}


std::string
GNEDemandElementFlow::getFlowAttribute(GNEDemandElement* flowElement, SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_END:
            return time2string(repetitionEnd);
        case SUMO_ATTR_VEHSPERHOUR:
        case SUMO_ATTR_PERSONSPERHOUR:
        case SUMO_ATTR_CONTAINERSPERHOUR:
            return toString(3600 / STEPS2TIME(repetitionOffset));
        case SUMO_ATTR_PERIOD:
            return time2string(repetitionOffset);
        case GNE_ATTR_POISSON:
            return toString(1 / STEPS2TIME(repetitionOffset));
        case SUMO_ATTR_PROB:
            return toString(repetitionProbability, 10);
        case SUMO_ATTR_NUMBER:
            return toString(repetitionNumber);
        default:
            throw InvalidArgument(flowElement->getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEDemandElementFlow::setFlowAttribute(GNEDemandElement* flowElement, SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_END:
        case SUMO_ATTR_NUMBER:
        case SUMO_ATTR_VEHSPERHOUR:
        case SUMO_ATTR_PERSONSPERHOUR:
        case SUMO_ATTR_CONTAINERSPERHOUR:
        case SUMO_ATTR_PERIOD:
        case GNE_ATTR_POISSON:
        case SUMO_ATTR_PROB:
            undoList->changeAttribute(new GNEChange_Attribute(flowElement, key, value));
            break;
        default:
            throw InvalidArgument(flowElement->getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEDemandElementFlow::isValidFlowAttribute(GNEDemandElement* flowElement, SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_END:
            if (GNEAttributeCarrier::canParse<SUMOTime>(value)) {
                return (GNEAttributeCarrier::parse<SUMOTime>(value) >= 0);
            } else {
                return false;
            }
        case SUMO_ATTR_VEHSPERHOUR:
        case SUMO_ATTR_PERSONSPERHOUR:
        case SUMO_ATTR_CONTAINERSPERHOUR:
        case SUMO_ATTR_PERIOD:
        case GNE_ATTR_POISSON:
            if (GNEAttributeCarrier::canParse<SUMOTime>(value)) {
                return (GNEAttributeCarrier::parse<SUMOTime>(value) > 0);
            } else {
                return false;
            }
        case SUMO_ATTR_PROB:
            if (GNEAttributeCarrier::canParse<double>(value)) {
                const double prob = GNEAttributeCarrier::parse<double>(value);
                return ((prob >= 0) && (prob <= 1));
            } else {
                return false;
            }
        case SUMO_ATTR_NUMBER:
            if (GNEAttributeCarrier::canParse<int>(value)) {
                return (GNEAttributeCarrier::parse<int>(value) >= 0);
            } else {
                return false;
            }
        default:
            throw InvalidArgument(flowElement->getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEDemandElementFlow::setFlowAttribute(GNEDemandElement* flowElement, SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_END:
            repetitionEnd = string2time(value);
            break;
        case SUMO_ATTR_VEHSPERHOUR:
        case SUMO_ATTR_PERSONSPERHOUR:
        case SUMO_ATTR_CONTAINERSPERHOUR:
            repetitionOffset = TIME2STEPS(3600 / string2time(value));
            break;
        case SUMO_ATTR_PERIOD:
            repetitionOffset = string2time(value);
            break;
        case GNE_ATTR_POISSON:
            repetitionOffset = TIME2STEPS(1 / string2time(value));
            break;
        case SUMO_ATTR_PROB:
            repetitionProbability = GNEAttributeCarrier::parse<double>(value);
            break;
        case SUMO_ATTR_NUMBER:
            repetitionNumber = GNEAttributeCarrier::parse<int>(value);
            break;
        default:
            throw InvalidArgument(flowElement->getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEDemandElementFlow::setFlowParameters(const SumoXMLAttr attribute, const bool value) {
    // modify parameters depending of given Flow attribute
    if (value) {
        switch (attribute) {
            case SUMO_ATTR_END:
                parametersSet |= VEHPARS_END_SET;
                break;
            case SUMO_ATTR_NUMBER:
                parametersSet |= VEHPARS_NUMBER_SET;
                break;
            case SUMO_ATTR_VEHSPERHOUR:
            case SUMO_ATTR_PERSONSPERHOUR:
            case SUMO_ATTR_CONTAINERSPERHOUR:
                parametersSet |= VEHPARS_VPH_SET;
                break;
            case SUMO_ATTR_PERIOD:
                parametersSet |= VEHPARS_PERIOD_SET;
                break;
            case GNE_ATTR_POISSON:
                parametersSet |= VEHPARS_POISSON_SET;
                break;
            case SUMO_ATTR_PROB:
                parametersSet |= VEHPARS_PROB_SET;
                break;
            default:
                break;
        }
    } else {
        switch (attribute) {
            case SUMO_ATTR_END:
                parametersSet &= ~VEHPARS_END_SET;
                break;
            case SUMO_ATTR_NUMBER:
                parametersSet &= ~VEHPARS_NUMBER_SET;
                break;
            case SUMO_ATTR_VEHSPERHOUR:
            case SUMO_ATTR_PERSONSPERHOUR:
            case SUMO_ATTR_CONTAINERSPERHOUR:
                parametersSet &= ~VEHPARS_VPH_SET;
                break;
            case SUMO_ATTR_PERIOD:
                parametersSet &= ~VEHPARS_PERIOD_SET;
                break;
            case GNE_ATTR_POISSON:
                parametersSet &= ~VEHPARS_POISSON_SET;
                break;
            case SUMO_ATTR_PROB:
                parametersSet &= ~VEHPARS_PROB_SET;
                break;
            default:
                break;
        }
    }
}


void
GNEDemandElementFlow::adjustDefaultFlowAttributes(GNEDemandElement* flowElement) {
    // get tag property
    const auto &tagProperty = flowElement->getTagProperty();
    // first check that this demand element is a flow
    if (tagProperty.isFlow()) {
        // end
        if ((parametersSet & VEHPARS_END_SET) == 0) {
            flowElement->setAttribute(SUMO_ATTR_END, tagProperty.getDefaultValue(SUMO_ATTR_END));
        }
        // number
        if ((parametersSet & VEHPARS_NUMBER_SET) == 0) {
            flowElement->setAttribute(SUMO_ATTR_NUMBER, tagProperty.getDefaultValue(SUMO_ATTR_NUMBER));
        }
        // vehicles/person/container per hour
        if (((parametersSet & VEHPARS_PERIOD_SET) == 0) &&
                ((parametersSet & VEHPARS_POISSON_SET) == 0) &&
                ((parametersSet & VEHPARS_VPH_SET) == 0)) {
            flowElement->setAttribute(SUMO_ATTR_PERIOD, tagProperty.getDefaultValue(SUMO_ATTR_PERIOD));
        }
        // probability
        if ((parametersSet & VEHPARS_PROB_SET) == 0) {
            flowElement->setAttribute(SUMO_ATTR_PROB, tagProperty.getDefaultValue(SUMO_ATTR_PROB));
        }
        // poisson
        if (repetitionOffset < 0) {
            flowElement->toggleAttribute(SUMO_ATTR_PERIOD, false);
            flowElement->toggleAttribute(GNE_ATTR_POISSON, true);
            flowElement->setAttribute(GNE_ATTR_POISSON, time2string(repetitionOffset * -1));
        }
    }
}

/****************************************************************************/
