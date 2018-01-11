/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNECalibrator.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
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

#include <string>
#include <iostream>
#include <utility>
#include <utils/geom/PositionVector.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/common/ToString.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/xml/SUMOSAXHandler.h>

#include "GNECalibrator.h"
#include "GNEEdge.h"
#include "GNELane.h"
#include "GNEViewNet.h"
#include "GNEUndoList.h"
#include "GNENet.h"
#include "GNEChange_Attribute.h"
#include "GNERouteProbe.h"
#include "GNECalibratorDialog.h"
#include "GNECalibratorFlow.h"
#include "GNECalibratorVehicleType.h"
#include "GNECalibratorRoute.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNECalibrator::GNECalibrator(const std::string& id, GNEViewNet* viewNet, GNEEdge* edge, double pos, double frequency, const std::string& output) :
    GNEAdditional(id, viewNet, SUMO_TAG_CALIBRATOR, ICON_CALIBRATOR, true),
    myEdge(edge),
    myLane(NULL),
    myPositionOverLane(pos / edge->getLanes().at(0)->getLaneParametricLength()),
    myFrequency(frequency),
    myOutput(output),
    myRouteProbe(NULL) { /** change this in the future **/
}


GNECalibrator::GNECalibrator(const std::string& id, GNEViewNet* viewNet, GNELane* lane, double pos, double frequency, const std::string& output) :
    GNEAdditional(id, viewNet, SUMO_TAG_LANECALIBRATOR, ICON_CALIBRATOR, true),
    myEdge(NULL),
    myLane(lane),
    myPositionOverLane(pos / lane->getLaneParametricLength()),
    myFrequency(frequency),
    myOutput(output),
    myRouteProbe(NULL) { /** change this in the future **/
}


GNECalibrator::~GNECalibrator() {}


void
GNECalibrator::writeAdditional(OutputDevice& device) const {
    // Write parameters
    device.openTag(SUMO_TAG_CALIBRATOR);
    device.writeAttr(SUMO_ATTR_ID, getID());
    if (myLane) {
        device.writeAttr(SUMO_ATTR_LANE, myLane->getID());
        device.writeAttr(SUMO_ATTR_POSITION, myPositionOverLane * myLane->getLaneParametricLength());
    } else if (myEdge) {
        device.writeAttr(SUMO_ATTR_EDGE, myEdge->getID());
        device.writeAttr(SUMO_ATTR_POSITION, myPositionOverLane * myEdge->getLanes().at(0)->getLaneParametricLength());
    } else {
        throw ProcessError("Both myEdge and myLane aren't defined");
    }
    device.writeAttr(SUMO_ATTR_FREQUENCY, myFrequency);
    device.writeAttr(SUMO_ATTR_OUTPUT, myOutput);
    // write all routes of this calibrator
    for (auto i : myCalibratorRoutes) {
        i->writeRoute(device);
    }
    // write all vehicle types of this calibrator
    for (auto i : myCalibratorVehicleTypes) {
        i->writeVehicleType(device);
    }
    // Write all flows of this calibrator
    for (auto i : myCalibratorFlows) {
        i->writeFlow(device);
    }
    // Close tag
    device.closeTag();
}


void
GNECalibrator::moveGeometry(const Position&, const Position&) {
    // This additional cannot be moved
}


void
GNECalibrator::commitGeometryMoving(const Position&, GNEUndoList*) {
    // This additional cannot be moved
}


void
GNECalibrator::updateGeometry() {
    // Clear all containers
    myShapeRotations.clear();
    myShapeLengths.clear();
    // clear Shape
    myShape.clear();
    // get shape depending of we have a edge or a lane
    if (myLane) {
        // Get shape of lane parent
        myShape.push_back(myLane->getShape().positionAtOffset(myPositionOverLane * myLane->getShape().length()));
        // Save rotation (angle) of the vector constructed by points f and s
        myShapeRotations.push_back(myLane->getShape().rotationDegreeAtOffset(myPositionOverLane * myLane->getShape().length()) * -1);
    } else if (myEdge) {
        for (auto i : myEdge->getLanes()) {
            // Get shape of lane parent
            myShape.push_back(i->getShape().positionAtOffset(myPositionOverLane * i->getShape().length()));
            // Save rotation (angle) of the vector constructed by points f and s
            myShapeRotations.push_back(myEdge->getLanes().at(0)->getShape().rotationDegreeAtOffset(myPositionOverLane * i->getShape().length()) * -1);
        }
    } else {
        throw ProcessError("Both myEdge and myLane aren't defined");
    }
    // Refresh element (neccesary to avoid grabbing problems)
    myViewNet->getNet()->refreshElement(this);
}


Position
GNECalibrator::getPositionInView() const {
    // get position depending of we have a edge or a lane
    if (myLane) {
        return myLane->getShape().positionAtOffset(myPositionOverLane * myLane->getShape().length());
    } else if (myEdge) {
        return myEdge->getLanes().at(0)->getShape().positionAtOffset(myPositionOverLane * myEdge->getLanes().at(0)->getShape().length());
    } else {
        throw ProcessError("Both myEdge and myLane aren't defined");
    }
}


const std::string&
GNECalibrator::getParentName() const {
    // get parent name depending of we have a edge or a lane
    if (myLane) {
        return myLane->getMicrosimID();
    } else if (myEdge) {
        return myEdge->getLanes().at(0)->getMicrosimID();
    } else {
        throw ProcessError("Both myEdge and myLane aren't defined");
    }
}


void
GNECalibrator::drawGL(const GUIVisualizationSettings& s) const {
    // get values
    glPushName(getGlID());
    glLineWidth(1.0);
    const double exaggeration = s.addSize.getExaggeration(s);

    // iterate over every Calibrator symbol
    for (int i = 0; i < (int)myShape.size(); ++i) {
        const Position& pos = myShape[i];
        double rot = myShapeRotations[i];
        glPushMatrix();
        glTranslated(pos.x(), pos.y(), getType());
        glRotated(rot, 0, 0, 1);
        glTranslated(0, 0, getType());
        glScaled(exaggeration, exaggeration, 1);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        if (isAdditionalSelected()) {
            GLHelper::setColor(myViewNet->getNet()->selectedAdditionalColor);
        } else {
            GLHelper::setColor(RGBColor(255, 204, 0));
        }
        // base
        glBegin(GL_TRIANGLES);
        glVertex2d(0 - 1.4, 0);
        glVertex2d(0 - 1.4, 6);
        glVertex2d(0 + 1.4, 6);
        glVertex2d(0 + 1.4, 0);
        glVertex2d(0 - 1.4, 0);
        glVertex2d(0 + 1.4, 6);
        glEnd();

        // draw text
        if (s.scale * exaggeration >= 1.) {
            // set color depending of selection status
            RGBColor textColor = isAdditionalSelected() ? myViewNet->getNet()->selectionColor : RGBColor::BLACK;
            // draw "C"
            GLHelper::drawText("C", Position(0, 1.5), 0.1, 3, textColor, 180);
            // draw "edge" or "lane "
            if (myLane) {
                GLHelper::drawText("lane", Position(0, 3), .1, 1, textColor, 180);
            } else if (myEdge) {
                GLHelper::drawText("edge", Position(0, 3), .1, 1, textColor, 180);
            } else {
                throw ProcessError("Both myEdge and myLane aren't defined");
            }
        }
        glPopMatrix();
    }
    drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
    glPopName();
}


void
GNECalibrator::openAdditionalDialog() {
    // Open calibrator dialog
    GNECalibratorDialog calibratorDialog(this);
}





void
GNECalibrator::addCalibratorRoute(GNECalibratorRoute* route) {
    myCalibratorRoutes.push_back(route);
}


void
GNECalibrator::removeCalibratorRoute(GNECalibratorRoute* route) {
    myCalibratorRoutes.erase(std::find(myCalibratorRoutes.begin(), myCalibratorRoutes.end(), route));
}


const std::vector<GNECalibratorRoute*>&
GNECalibrator::getCalibratorRoutes() const {
    return myCalibratorRoutes;
}


void
GNECalibrator::addCalibratorFlow(GNECalibratorFlow* flow) {
    assert(flow);
    myCalibratorFlows.push_back(flow);
}


void
GNECalibrator::removeCalibratorFlow(GNECalibratorFlow* flow) {
    myCalibratorFlows.erase(std::find(myCalibratorFlows.begin(), myCalibratorFlows.end(), flow));
}


const std::vector<GNECalibratorFlow*>&
GNECalibrator::getCalibratorFlows() const {
    return myCalibratorFlows;
}


void
GNECalibrator::addCalibratorVehicleType(GNECalibratorVehicleType* vehicleType) {
    myCalibratorVehicleTypes.push_back(vehicleType);
}


void
GNECalibrator::removeCalibratorVehicleType(GNECalibratorVehicleType* vehicleType) {
    myCalibratorVehicleTypes.erase(std::find(myCalibratorVehicleTypes.begin(), myCalibratorVehicleTypes.end(), vehicleType));
}


const std::vector<GNECalibratorVehicleType*>&
GNECalibrator::getCalibratorVehicleTypes() const {
    return myCalibratorVehicleTypes;
}


std::string
GNECalibrator::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getAdditionalID();
        case SUMO_ATTR_EDGE:
            return myEdge->getID();
        case SUMO_ATTR_LANE:
            return myLane->getID();
        case SUMO_ATTR_POSITION:
            if (myEdge) {
                return toString(myPositionOverLane * myEdge->getLanes().at(0)->getLaneParametricLength());
            } else if (myLane) {
                return toString(myPositionOverLane * myLane->getLaneParametricLength());
            } else {
                throw ProcessError("Both myEdge and myLane aren't defined");
            }
        case SUMO_ATTR_FREQUENCY:
            return toString(myFrequency);
        case SUMO_ATTR_OUTPUT:
            return myOutput;
        case SUMO_ATTR_ROUTEPROBE:
            if (myRouteProbe) {
                return myRouteProbe->getID();
            } else {
                return "";
            }
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNECalibrator::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_EDGE:
        case SUMO_ATTR_LANE:
        case SUMO_ATTR_POSITION:
        case SUMO_ATTR_FREQUENCY:
        case SUMO_ATTR_OUTPUT:
        case SUMO_ATTR_ROUTEPROBE:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }

}


bool
GNECalibrator::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidAdditionalID(value);
        case SUMO_ATTR_EDGE:
            if (myViewNet->getNet()->retrieveEdge(value, false) != NULL) {
                return true;
            } else {
                return false;
            }
        case SUMO_ATTR_LANE:
            if (myViewNet->getNet()->retrieveLane(value, false) != NULL) {
                return true;
            } else {
                return false;
            }
        case SUMO_ATTR_POSITION:
            if (canParse<double>(value)) {
                // obtain relative new start position
                double newPosition;
                if (myEdge) {
                    newPosition = parse<double>(value) / myEdge->getLanes().at(0)->getLaneParametricLength();
                } else if (myLane) {
                    newPosition = parse<double>(value) / myLane->getLaneParametricLength();
                } else {
                    throw ProcessError("Both myEdge and myLane aren't defined");
                }
                if ((newPosition < 0) || (newPosition > 1)) {
                    return false;
                } else {
                    return true;
                }
            } else {
                return false;
            }
        case SUMO_ATTR_FREQUENCY:
            return (canParse<double>(value) && parse<double>(value) >= 0);
        case SUMO_ATTR_OUTPUT:
            return isValidFilename(value);
        case SUMO_ATTR_ROUTEPROBE:
            if (isValidID(value) && (myViewNet->getNet()->getAdditional(SUMO_TAG_ROUTEPROBE, value) != NULL)) {
                return true;
            } else {
                return false;
            }
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNECalibrator::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            changeAdditionalID(value);
            break;
        case SUMO_ATTR_EDGE:
            myEdge = changeEdge(myEdge, value);
            break;
        case SUMO_ATTR_LANE:
            myLane = changeLane(myLane, value);
            break;
        case SUMO_ATTR_POSITION:
            if (myEdge) {
                myPositionOverLane = parse<double>(value) / myEdge->getLanes().at(0)->getLaneParametricLength();
            } else if (myLane) {
                myPositionOverLane = parse<double>(value) / myLane->getLaneParametricLength();
            } else {
                throw ProcessError("Both myEdge and myLane aren't defined");
            }
            break;
        case SUMO_ATTR_FREQUENCY:
            myFrequency = parse<double>(value);
            break;
        case SUMO_ATTR_OUTPUT:
            myOutput = value;
            break;
        case SUMO_ATTR_ROUTEPROBE:
            myRouteProbe = dynamic_cast<GNERouteProbe*>(myViewNet->getNet()->getAdditional(SUMO_TAG_ROUTEPROBE, value));
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
    // After setting attribute always update Geometry
    updateGeometry();
}

/****************************************************************************/
