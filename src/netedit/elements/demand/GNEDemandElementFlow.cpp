/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNEDemandElementFlow.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2023
///
// An auxiliar, asbtract class for flow elements (vehicles, person and containers)
/****************************************************************************/

#include <netedit/GNETagProperties.h>
#include <netedit/GNEUndoList.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/changes/GNEChange_ToggleAttribute.h>
#include <utils/gui/div/GLHelper.h>

#include "GNEDemandElementFlow.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEDemandElementFlow::GNEDemandElementFlow(GNEDemandElement* flowElement) {
    // set default flow attributes
    setDefaultFlowAttributes(flowElement);
}


GNEDemandElementFlow::GNEDemandElementFlow(GNEDemandElement* flowElement, const SUMOVehicleParameter& vehicleParameters) :
    SUMOVehicleParameter(vehicleParameters) {
    // set default flow attributes
    setDefaultFlowAttributes(flowElement);
}


GNEDemandElementFlow::~GNEDemandElementFlow() {}


void
GNEDemandElementFlow::drawFlowLabel(const Position& position, const double rotation, const double width,
                                    const double length, const double exaggeration) const {
    // declare contour width
    const double contourWidth = (0.05 * exaggeration);
    // Push matrix
    GLHelper::pushMatrix();
    // Traslate to  bot
    glTranslated(position.x(), position.y(), GLO_VEHICLELABELS);
    // glTranslated(position.x(), position.y(), GLO_ROUTE + getType() + 0.1 + GLO_PERSONFLOW + 0.1);
    glRotated(rotation, 0, 0, -1);
    glTranslated(-1 * ((width * 0.5 * exaggeration) + (0.35 * exaggeration)) - 0.05, 0, 0);
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
GNEDemandElementFlow::getFlowAttribute(const GNEDemandElement* flowElement, SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_DEPART:
        case SUMO_ATTR_BEGIN:
            if (departProcedure == DepartDefinition::TRIGGERED) {
                return "triggered";
            } else if (departProcedure == DepartDefinition::CONTAINER_TRIGGERED) {
                return "containerTriggered";
            } else if (departProcedure == DepartDefinition::NOW) {
                return "now";
            } else if (departProcedure == DepartDefinition::SPLIT) {
                return "split";
            } else if (departProcedure == DepartDefinition::BEGIN) {
                return "begin";
            } else {
                return time2string(depart);
            }
        case SUMO_ATTR_END:
            return time2string(repetitionEnd);
        case SUMO_ATTR_VEHSPERHOUR:
        case SUMO_ATTR_PERSONSPERHOUR:
        case SUMO_ATTR_CONTAINERSPERHOUR:
            return StringUtils::adjustDecimalValue(3600 / STEPS2TIME(repetitionOffset), 2);
        case SUMO_ATTR_PERIOD:
            return time2string(repetitionOffset);
        case GNE_ATTR_POISSON:
            return StringUtils::adjustDecimalValue(poissonRate, 10);
        case SUMO_ATTR_PROB:
            return StringUtils::adjustDecimalValue(repetitionProbability, 20);
        case SUMO_ATTR_NUMBER:
            return toString(repetitionNumber);
        case GNE_ATTR_FLOW_TERMINATE:
            if (isFlowAttributeEnabled(SUMO_ATTR_END)) {
                if (isFlowAttributeEnabled(SUMO_ATTR_NUMBER)) {
                    return toString(SUMO_ATTR_END) + "-" + toString(SUMO_ATTR_NUMBER);
                } else {
                    return toString(SUMO_ATTR_END);
                }
            } else if (isFlowAttributeEnabled(SUMO_ATTR_NUMBER)) {
                return toString(SUMO_ATTR_NUMBER);
            } else {
                return "invalid terminate";
            }
        case GNE_ATTR_FLOW_SPACING:
            if (flowElement->getTagProperty()->hasAttribute(SUMO_ATTR_VEHSPERHOUR) && isFlowAttributeEnabled(SUMO_ATTR_VEHSPERHOUR)) {
                return toString(SUMO_ATTR_VEHSPERHOUR);
            } else if (flowElement->getTagProperty()->hasAttribute(SUMO_ATTR_PERSONSPERHOUR) && isFlowAttributeEnabled(SUMO_ATTR_PERSONSPERHOUR)) {
                return toString(SUMO_ATTR_PERSONSPERHOUR);
            } else if (flowElement->getTagProperty()->hasAttribute(SUMO_ATTR_CONTAINERSPERHOUR) && isFlowAttributeEnabled(SUMO_ATTR_CONTAINERSPERHOUR)) {
                return toString(SUMO_ATTR_CONTAINERSPERHOUR);
            } else if (isFlowAttributeEnabled(SUMO_ATTR_PERIOD)) {
                return toString(SUMO_ATTR_PERIOD);
            } else if (isFlowAttributeEnabled(SUMO_ATTR_PROB)) {
                return toString(SUMO_ATTR_PROB);
            } else if (isFlowAttributeEnabled(GNE_ATTR_POISSON)) {
                return toString(GNE_ATTR_POISSON);
            } else {
                return "invalid flow spacing";
            }
        default:
            return flowElement->getCommonAttribute(key);
    }
}


double
GNEDemandElementFlow::getFlowAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_DEPART:
        case SUMO_ATTR_BEGIN:
            return STEPS2TIME(depart);
        default:
            throw InvalidArgument("Flow doesn't have a double attribute of type '" + toString(key) + "'");
    }
}


void
GNEDemandElementFlow::setFlowAttribute(GNEDemandElement* flowElement, SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_DEPART:
        case SUMO_ATTR_BEGIN:
        case SUMO_ATTR_END:
        case SUMO_ATTR_NUMBER:
        case SUMO_ATTR_VEHSPERHOUR:
        case SUMO_ATTR_PERSONSPERHOUR:
        case SUMO_ATTR_CONTAINERSPERHOUR:
        case SUMO_ATTR_PERIOD:
        case GNE_ATTR_POISSON:
        case SUMO_ATTR_PROB:
        case GNE_ATTR_FLOW_TERMINATE:
        case GNE_ATTR_FLOW_SPACING:
            GNEChange_Attribute::changeAttribute(flowElement, key, value, undoList);
            break;
        default:
            return flowElement->setCommonAttribute(key, value, undoList);
            break;
    }
}


bool
GNEDemandElementFlow::isValidFlowAttribute(GNEDemandElement* flowElement, SumoXMLAttr key, const std::string& value) {
    // declare string error
    std::string error;
    switch (key) {
        case SUMO_ATTR_DEPART:
        case SUMO_ATTR_BEGIN: {
            SUMOTime dummyDepart;
            DepartDefinition dummyDepartProcedure;
            parseDepart(value, flowElement->getTagProperty()->getTagStr(), id, dummyDepart, dummyDepartProcedure, error);
            // if error is empty, given value is valid
            return error.empty();
        }
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
            if (GNEAttributeCarrier::canParse<double>(value)) {
                return (GNEAttributeCarrier::parse<double>(value) > 0);
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
        case GNE_ATTR_FLOW_TERMINATE:
        case GNE_ATTR_FLOW_SPACING: {
            const auto& flowValues = flowElement->getTagProperty()->getAttributeProperties(key)->getDiscreteValues();
            if (std::find(flowValues.begin(), flowValues.end(), value) != flowValues.end()) {
                return true;
            } else {
                return false;
            }
        }
        default:
            return flowElement->isCommonAttributeValid(key, value);
    }
}


void
GNEDemandElementFlow::enableFlowAttribute(GNEDemandElement* flowElement, SumoXMLAttr key, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_END:
        case SUMO_ATTR_NUMBER:
        case SUMO_ATTR_VEHSPERHOUR:
        case SUMO_ATTR_PERSONSPERHOUR:
        case SUMO_ATTR_CONTAINERSPERHOUR:
        case SUMO_ATTR_PERIOD:
        case GNE_ATTR_POISSON:
        case SUMO_ATTR_PROB:
            undoList->add(new GNEChange_ToggleAttribute(flowElement, key, true), true);
            return;
        default:
            throw InvalidArgument(flowElement->getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEDemandElementFlow::disableFlowAttribute(GNEDemandElement* flowElement, SumoXMLAttr key, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_END:
        case SUMO_ATTR_NUMBER:
        case SUMO_ATTR_VEHSPERHOUR:
        case SUMO_ATTR_PERSONSPERHOUR:
        case SUMO_ATTR_CONTAINERSPERHOUR:
        case SUMO_ATTR_PERIOD:
        case GNE_ATTR_POISSON:
        case SUMO_ATTR_PROB:
            undoList->add(new GNEChange_ToggleAttribute(flowElement, key, false), true);
            return;
        default:
            throw InvalidArgument(flowElement->getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEDemandElementFlow::isFlowAttributeEnabled(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_END:
            return (parametersSet & VEHPARS_END_SET) != 0;
        case SUMO_ATTR_NUMBER:
            return (parametersSet & VEHPARS_NUMBER_SET) != 0;
        case SUMO_ATTR_VEHSPERHOUR:
        case SUMO_ATTR_PERSONSPERHOUR:
        case SUMO_ATTR_CONTAINERSPERHOUR:
            return (parametersSet & VEHPARS_VPH_SET) != 0;
        case SUMO_ATTR_PERIOD:
            return (parametersSet & VEHPARS_PERIOD_SET) != 0;
        case GNE_ATTR_POISSON:
            return (parametersSet & VEHPARS_POISSON_SET) != 0;
        case SUMO_ATTR_PROB:
            return (parametersSet & VEHPARS_PROB_SET) != 0;
        case GNE_ATTR_FLOW_SPACING:
            return !isFlowAttributeEnabled(SUMO_ATTR_END) || !isFlowAttributeEnabled(SUMO_ATTR_NUMBER);
        default:
            return true;
    }
}


void
GNEDemandElementFlow::setFlowAttribute(GNEDemandElement* flowElement, SumoXMLAttr key, const std::string& value) {
    // declare string error
    std::string error;
    switch (key) {
        case SUMO_ATTR_DEPART:
        case SUMO_ATTR_BEGIN: {
            parseDepart(value, flowElement->getTagProperty()->getTagStr(), id, depart, departProcedure, error);
            break;
        }
        case SUMO_ATTR_END:
            repetitionEnd = string2time(value);
            break;
        case SUMO_ATTR_VEHSPERHOUR:
        case SUMO_ATTR_PERSONSPERHOUR:
        case SUMO_ATTR_CONTAINERSPERHOUR:
            repetitionOffset = TIME2STEPS(3600 / GNEAttributeCarrier::parse<double>(value));
            poissonRate = GNEAttributeCarrier::parse<double>(value) / 3600;
            break;
        case SUMO_ATTR_PERIOD:
            repetitionOffset = string2time(value);
            poissonRate = 1 / STEPS2TIME(repetitionOffset);
            break;
        case GNE_ATTR_POISSON:
            poissonRate = GNEAttributeCarrier::parse<double>(value);
            repetitionOffset = TIME2STEPS(1 / poissonRate);
            break;
        case SUMO_ATTR_PROB:
            repetitionProbability = GNEAttributeCarrier::parse<double>(value);
            break;
        case SUMO_ATTR_NUMBER:
            repetitionNumber = GNEAttributeCarrier::parse<int>(value);
            break;
        case GNE_ATTR_FLOW_TERMINATE:
            if (value == (toString(SUMO_ATTR_END) + "-" + toString(SUMO_ATTR_NUMBER))) {
                toggleFlowAttribute(SUMO_ATTR_END, true);
                toggleFlowAttribute(SUMO_ATTR_NUMBER, true);
                // in this special case, disable other spacing
                toggleFlowAttribute(SUMO_ATTR_VEHSPERHOUR, false);
                toggleFlowAttribute(SUMO_ATTR_PERIOD, false);
                toggleFlowAttribute(GNE_ATTR_POISSON, false);
                toggleFlowAttribute(SUMO_ATTR_PROB, false);
            } else {
                // if previously end-number was enabled, enable perHour
                if (isFlowAttributeEnabled(SUMO_ATTR_END) && isFlowAttributeEnabled(SUMO_ATTR_NUMBER)) {
                    toggleFlowAttribute(SUMO_ATTR_VEHSPERHOUR, true);
                }
                if (value == toString(SUMO_ATTR_END)) {
                    toggleFlowAttribute(SUMO_ATTR_END, true);
                    toggleFlowAttribute(SUMO_ATTR_NUMBER, false);
                } else if (value == toString(SUMO_ATTR_NUMBER)) {
                    toggleFlowAttribute(SUMO_ATTR_END, false);
                    toggleFlowAttribute(SUMO_ATTR_NUMBER, true);
                }
            }
            break;
        case GNE_ATTR_FLOW_SPACING:
            if ((value == toString(SUMO_ATTR_VEHSPERHOUR)) ||
                    (value == toString(SUMO_ATTR_PERSONSPERHOUR)) ||
                    (value == toString(SUMO_ATTR_CONTAINERSPERHOUR))) {
                toggleFlowAttribute(SUMO_ATTR_VEHSPERHOUR, true);
                toggleFlowAttribute(SUMO_ATTR_PERIOD, false);
                toggleFlowAttribute(GNE_ATTR_POISSON, false);
                toggleFlowAttribute(SUMO_ATTR_PROB, false);
            } else if (value == toString(SUMO_ATTR_PERIOD)) {
                toggleFlowAttribute(SUMO_ATTR_VEHSPERHOUR, false);
                toggleFlowAttribute(SUMO_ATTR_PERIOD, true);
                toggleFlowAttribute(GNE_ATTR_POISSON, false);
                toggleFlowAttribute(SUMO_ATTR_PROB, false);
            } else if (value == toString(GNE_ATTR_POISSON)) {
                toggleFlowAttribute(SUMO_ATTR_VEHSPERHOUR, false);
                toggleFlowAttribute(SUMO_ATTR_PERIOD, false);
                toggleFlowAttribute(GNE_ATTR_POISSON, true);
                toggleFlowAttribute(SUMO_ATTR_PROB, false);
            } else if (value == toString(SUMO_ATTR_PROB)) {
                toggleFlowAttribute(SUMO_ATTR_VEHSPERHOUR, false);
                toggleFlowAttribute(SUMO_ATTR_PERIOD, false);
                toggleFlowAttribute(GNE_ATTR_POISSON, false);
                toggleFlowAttribute(SUMO_ATTR_PROB, true);
            }
            break;
        default:
            flowElement->setCommonAttribute(key, value);
            break;
    }
}


void
GNEDemandElementFlow::toggleFlowAttribute(const SumoXMLAttr attribute, const bool value) {
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
GNEDemandElementFlow::setDefaultFlowAttributes(GNEDemandElement* flowElement) {
    // first check that this demand element is a flow
    if (flowElement->getTagProperty()->isFlow()) {
        // end
        if ((parametersSet & VEHPARS_END_SET) == 0) {
            setFlowAttribute(flowElement, SUMO_ATTR_END, flowElement->getTagProperty()->getDefaultStringValue(SUMO_ATTR_END));
        }
        // number
        if ((parametersSet & VEHPARS_NUMBER_SET) == 0) {
            setFlowAttribute(flowElement, SUMO_ATTR_NUMBER, flowElement->getTagProperty()->getDefaultStringValue(SUMO_ATTR_NUMBER));
        }
        // vehicles/person/container per hour
        if (((parametersSet & VEHPARS_PERIOD_SET) == 0) &&
                ((parametersSet & VEHPARS_POISSON_SET) == 0) &&
                ((parametersSet & VEHPARS_VPH_SET) == 0)) {
            setFlowAttribute(flowElement, SUMO_ATTR_PERIOD, flowElement->getTagProperty()->getDefaultStringValue(SUMO_ATTR_PERIOD));
        }
        // probability
        if ((parametersSet & VEHPARS_PROB_SET) == 0) {
            setFlowAttribute(flowElement, SUMO_ATTR_PROB, flowElement->getTagProperty()->getDefaultStringValue(SUMO_ATTR_PROB));
        }
        // poisson
        if (repetitionOffset < 0) {
            toggleFlowAttribute(SUMO_ATTR_PERIOD, false);
            toggleFlowAttribute(GNE_ATTR_POISSON, true);
            setFlowAttribute(flowElement, GNE_ATTR_POISSON, toString(poissonRate));
        } else {
            setFlowAttribute(flowElement, SUMO_ATTR_PERIOD, time2string(repetitionOffset));
        }
    }
}

/****************************************************************************/
