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


void
GNEDemandElementFlow::setFlowParameters(SUMOVehicleParameter* vehicleParameters, const SumoXMLAttr attribute, const bool value) {
    // modify parameters depending of given Flow attribute
    if (value) {
        switch (attribute) {
            case SUMO_ATTR_END:
                vehicleParameters->parametersSet |= VEHPARS_END_SET;
                break;
            case SUMO_ATTR_NUMBER:
                vehicleParameters->parametersSet |= VEHPARS_NUMBER_SET;
                break;
            case SUMO_ATTR_VEHSPERHOUR:
            case SUMO_ATTR_PERSONSPERHOUR:
            case SUMO_ATTR_CONTAINERSPERHOUR:
                vehicleParameters->parametersSet |= VEHPARS_VPH_SET;
                break;
            case SUMO_ATTR_PERIOD:
                vehicleParameters->parametersSet |= VEHPARS_PERIOD_SET;
                break;
            case GNE_ATTR_POISSON:
                vehicleParameters->parametersSet |= VEHPARS_POISSON_SET;
                break;
            case SUMO_ATTR_PROB:
                vehicleParameters->parametersSet |= VEHPARS_PROB_SET;
                break;
            default:
                break;
        }
    } else {
        switch (attribute) {
            case SUMO_ATTR_END:
                vehicleParameters->parametersSet &= ~VEHPARS_END_SET;
                break;
            case SUMO_ATTR_NUMBER:
                vehicleParameters->parametersSet &= ~VEHPARS_NUMBER_SET;
                break;
            case SUMO_ATTR_VEHSPERHOUR:
            case SUMO_ATTR_PERSONSPERHOUR:
            case SUMO_ATTR_CONTAINERSPERHOUR:
                vehicleParameters->parametersSet &= ~VEHPARS_VPH_SET;
                break;
            case SUMO_ATTR_PERIOD:
                vehicleParameters->parametersSet &= ~VEHPARS_PERIOD_SET;
                break;
            case GNE_ATTR_POISSON:
                vehicleParameters->parametersSet &= ~VEHPARS_POISSON_SET;
                break;
            case SUMO_ATTR_PROB:
                vehicleParameters->parametersSet &= ~VEHPARS_PROB_SET;
                break;
            default:
                break;
        }
    }
}


void
GNEDemandElementFlow::adjustDefaultFlowAttributes(GNEDemandElement* flowElement, SUMOVehicleParameter* vehicleParameters) {
    // get tag property
    const auto &tagProperty = flowElement->getTagProperty();
    // first check that this demand element is a flow
    if (tagProperty.isFlow()) {
        // end
        if ((vehicleParameters->parametersSet & VEHPARS_END_SET) == 0) {
            flowElement->setAttribute(SUMO_ATTR_END, tagProperty.getDefaultValue(SUMO_ATTR_END));
        }
        // number
        if ((vehicleParameters->parametersSet & VEHPARS_NUMBER_SET) == 0) {
            flowElement->setAttribute(SUMO_ATTR_NUMBER, tagProperty.getDefaultValue(SUMO_ATTR_NUMBER));
        }
        // vehicles/person/container per hour
        if (((vehicleParameters->parametersSet & VEHPARS_PERIOD_SET) == 0) &&
                ((vehicleParameters->parametersSet & VEHPARS_POISSON_SET) == 0) &&
                ((vehicleParameters->parametersSet & VEHPARS_VPH_SET) == 0)) {
            flowElement->setAttribute(SUMO_ATTR_PERIOD, tagProperty.getDefaultValue(SUMO_ATTR_PERIOD));
        }
        // probability
        if ((vehicleParameters->parametersSet & VEHPARS_PROB_SET) == 0) {
            flowElement->setAttribute(SUMO_ATTR_PROB, tagProperty.getDefaultValue(SUMO_ATTR_PROB));
        }
        // poisson
        if (vehicleParameters->repetitionOffset < 0) {
            flowElement->toggleAttribute(SUMO_ATTR_PERIOD, false);
            flowElement->toggleAttribute(GNE_ATTR_POISSON, true);
            flowElement->setAttribute(GNE_ATTR_POISSON, time2string(vehicleParameters->repetitionOffset * -1));
        }
    }
}

/****************************************************************************/
