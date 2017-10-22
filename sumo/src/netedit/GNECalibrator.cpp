/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2017 German Aerospace Center (DLR) and others.
/****************************************************************************/
//
//   This program and the accompanying materials
//   are made available under the terms of the Eclipse Public License v2.0
//   which accompanies this distribution, and is available at
//   http://www.eclipse.org/legal/epl-v20.html
//
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


// ===========================================================================
// member method definitions
// ===========================================================================

GNECalibrator::GNECalibrator(const std::string& id, GNEViewNet* viewNet, SumoXMLTag tag, double relativePos, double frequency, const std::string& output, 
                             const std::vector<GNECalibratorRoute>& calibratorRoutes, const std::vector<GNECalibratorFlow>& calibratorFlows, 
                             const std::vector<GNECalibratorVehicleType>& calibratorVehicleTypes, GNEEdge *edge, GNELane *lane) :
    GNEAdditional(id, viewNet, tag, ICON_CALIBRATOR),
    myPositionOverLane(relativePos),
    myFrequency(frequency),
    myOutput(output),
    myRouteProbe(NULL), /** change this in the future **/
    myCalibratorRoutes(calibratorRoutes),
    myCalibratorFlows(calibratorFlows),
    myCalibratorVehicleTypes(calibratorVehicleTypes),
    myEdge(edge),
    myLane(lane) {
    assert((myEdge == NULL) ^ (myLane == NULL));
    // this additional ISN'T movable
    myMovable = false;
}


GNECalibrator::~GNECalibrator() {}


void
GNECalibrator::writeAdditional(OutputDevice& device) const {
    // Write parameters
    device.openTag(SUMO_TAG_CALIBRATOR);
    device.writeAttr(SUMO_ATTR_ID, getID());
    if(myLane) {
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
        // Open route tag
        device.openTag(i.getTag());
        // Write route ID
        device.writeAttr(SUMO_ATTR_BEGIN, i.getRouteID());
        // Write edge IDs
        device.writeAttr(SUMO_ATTR_BEGIN, i.getEdgesIDs());
        // Write Color
        device.writeAttr(SUMO_ATTR_BEGIN, i.getColor());
        // Close flow tag
        device.closeTag();
    }
    // write all vehicle types of this calibrator
    for (auto i : myCalibratorVehicleTypes) {
        // Open vehicle type tag
        device.openTag(i.getTag());
        // write id
        device.writeAttr(SUMO_ATTR_ID, i.getVehicleTypeID());
        //write accel
        device.writeAttr(SUMO_ATTR_ACCEL, i.getAccel());
        // write decel
        device.writeAttr(SUMO_ATTR_DECEL, i.getDecel());
        // write sigma
        device.writeAttr(SUMO_ATTR_SIGMA, i.getSigma());
        // write tau
        device.writeAttr(SUMO_ATTR_TAU, i.getTau());
        // write lenght
        device.writeAttr(SUMO_ATTR_LENGTH, i.getLength());
        // write min gap
        device.writeAttr(SUMO_ATTR_MINGAP, i.getMinGap());
        // write max speed
        device.writeAttr(SUMO_ATTR_MAXSPEED, i.getMaxSpeed());
        // write speed factor
        device.writeAttr(SUMO_ATTR_SPEEDFACTOR, i.getSpeedFactor());
        // write speed dev
        device.writeAttr(SUMO_ATTR_SPEEDDEV, i.getSpeedDev());
        // write color
        device.writeAttr(SUMO_ATTR_COLOR, i.getColor());
        // write vehicle class
        device.writeAttr(SUMO_ATTR_VCLASS, i.getVClass());
        // write emission class
        device.writeAttr(SUMO_ATTR_EMISSIONCLASS, i.getEmissionClass());
        // write shape
        device.writeAttr(SUMO_ATTR_SHAPE, i.getShape());
        // write width
        device.writeAttr(SUMO_ATTR_WIDTH, i.getWidth());
        // write filename
        device.writeAttr(SUMO_ATTR_FILE, i.getFilename());
        // write impatience
        device.writeAttr(SUMO_ATTR_IMPATIENCE, i.getImpatience());
        // write lane change model
        device.writeAttr(SUMO_ATTR_LANE_CHANGE_MODEL, i.getLaneChangeModel());
        // write car follow model
        device.writeAttr(SUMO_ATTR_CAR_FOLLOW_MODEL, i.getCarFollowModel());
        // write person capacity
        device.writeAttr(SUMO_ATTR_PERSON_CAPACITY, i.getPersonCapacity());
        // write container capacity
        device.writeAttr(SUMO_ATTR_CONTAINER_CAPACITY, i.getContainerCapacity());
        // write boarding duration
        device.writeAttr(SUMO_ATTR_BOARDING_DURATION, i.getBoardingDuration());
        // write loading duration
        device.writeAttr(SUMO_ATTR_LOADING_DURATION, i.getLoadingDuration());
        // write get lat alignment
        device.writeAttr(SUMO_ATTR_LATALIGNMENT, i.getLatAlignment());
        // write min gap lat
        device.writeAttr(SUMO_ATTR_MINGAP_LAT, i.getMinGapLat());
        // write max speed lat
        device.writeAttr(SUMO_ATTR_MAXSPEED_LAT, i.getMaxSpeedLat());
        // Close vehicle type tag
        device.closeTag();
    }
    // Write all flows of this calibrator
    for (auto i : myCalibratorFlows) {
        // Open flow tag
        device.openTag(i.getTag());
        // Write begin
        device.writeAttr(SUMO_ATTR_BEGIN, i.getBegin());
        // Write end
        device.writeAttr(SUMO_ATTR_END, i.getEnd());
        // Write type
        device.writeAttr(SUMO_ATTR_TYPE, i.getVehicleType());
        // Write route
        device.writeAttr(SUMO_ATTR_ROUTE, i.getRoute());
        // Write color
        device.writeAttr(SUMO_ATTR_COLOR, i.getColor());
        // Write depart lane
        device.writeAttr(SUMO_ATTR_DEPARTLANE, i.getDepartLane());
        // Write depart pos
        device.writeAttr(SUMO_ATTR_DEPARTPOS, i.getDepartPos());
        // Write depart speed
        device.writeAttr(SUMO_ATTR_DEPARTSPEED, i.getDepartSpeed());
        // Write arrival lane
        device.writeAttr(SUMO_ATTR_ARRIVALLANE, i.getArrivalLane());
        // Write arrival pos
        device.writeAttr(SUMO_ATTR_ARRIVALPOS, i.getArrivalPos());
        // Write arrival speed
        device.writeAttr(SUMO_ATTR_ARRIVALSPEED, i.getArrivalSpeed());
        // Write line
        device.writeAttr(SUMO_ATTR_LINE, i.getLine());
        // Write person number
        device.writeAttr(SUMO_ATTR_PERSON_NUMBER, i.getPersonNumber());
        // Write container number
        device.writeAttr(SUMO_ATTR_CONTAINER_NUMBER, i.getContainerNumber());
        // Write reroute
        device.writeAttr(SUMO_ATTR_REROUTE, i.getReroute());
        // Write departPosLat
        device.writeAttr(SUMO_ATTR_DEPARTPOS_LAT, i.getDepartPosLat());
        // Write arrivalPosLat
        device.writeAttr(SUMO_ATTR_ARRIVALPOS_LAT, i.getArrivalPosLat());
        // Write number
        device.writeAttr(SUMO_ATTR_NUMBER, i.getNumber());
        // Write type of flow
        if (i.getFlowType() == GNECalibratorFlow::GNE_CALIBRATORFLOW_PERIOD) {
            // write period
            device.writeAttr(SUMO_ATTR_PERIOD, i.getPeriod());
        } else if (i.getFlowType() == GNECalibratorFlow::GNE_CALIBRATORFLOW_VEHSPERHOUR) {
            // write vehs per hour
            device.writeAttr(SUMO_ATTR_VEHSPERHOUR, i.getVehsPerHour());
        } else if (i.getFlowType() == GNECalibratorFlow::GNE_CALIBRATORFLOW_PROBABILITY) {
            // write probability
            device.writeAttr(SUMO_ATTR_PROB, i.getProbability());
        }
        // Close flow tag
        device.closeTag();
    }
    // Close tag
    device.closeTag();
}


void
GNECalibrator::moveGeometry(const Position&, const Position&) {
    // This additional cannot be moved currently
}


void
GNECalibrator::commitGeometryMoving(const Position&, GNEUndoList*) {
    // This additional cannot be moved currently
}


void
GNECalibrator::updateGeometry() {
    // Clear all containers
    myShapeRotations.clear();
    myShapeLengths.clear();
    // clear Shape
    myShape.clear();
    
    if(myLane != NULL) {
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
    if(myLane) {
        return myLane->getShape().positionAtOffset(myPositionOverLane * myLane->getShape().length());
    } else if(myEdge) {
        return myEdge->getLanes().at(0)->getShape().positionAtOffset(myPositionOverLane * myEdge->getLanes().at(0)->getShape().length());
    } else {
        throw ProcessError("Both myEdge and myLane aren't defined");
    }
}


const std::string&
GNECalibrator::getParentName() const {
    if(myLane) {
        return myLane->getMicrosimID();
    } else if(myEdge) {
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
            RGBColor textColor = isAdditionalSelected()? myViewNet->getNet()->selectionColor : RGBColor::BLACK;
            // draw "C"
            GLHelper::drawText("C", Position(0, 1.5), 0.1, 3, textColor, 180);
            // draw "edge" or "lane "
            if(myLane) {
                GLHelper::drawText("lane", Position(0, 3), .1, 1, textColor, 180);
            } else if(myEdge) {
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
GNECalibrator::addCalibratorVehicleType(const GNECalibratorVehicleType& vehicleType) {
    myCalibratorVehicleTypes.push_back(vehicleType);
}


void
GNECalibrator::addCalibratorFlow(const GNECalibratorFlow& flow) {
    myCalibratorFlows.push_back(flow);
}


void
GNECalibrator::addCalibratorRoute(const GNECalibratorRoute& route) {
    myCalibratorRoutes.push_back(route);
}


const std::vector<GNECalibratorVehicleType>&
GNECalibrator::getCalibratorVehicleTypes() const {
    return myCalibratorVehicleTypes;
}


const std::vector<GNECalibratorFlow>&
GNECalibrator::getCalibratorFlows() const {
    return myCalibratorFlows;
}


const std::vector<GNECalibratorRoute>&
GNECalibrator::getCalibratorRoutes() const {
    return myCalibratorRoutes;
}


bool
GNECalibrator::vehicleTypeExists(std::string vehicleTypeID) const {
    for (auto i : myCalibratorVehicleTypes) {
        if (i.getVehicleTypeID() == vehicleTypeID) {
            return true;
        }
    }
    return false;
}


bool
GNECalibrator::flowExists(std::string flowID) const {
    for (auto i : myCalibratorFlows) {
        if (i.getFlowID() == flowID) {
            return true;
        }
    }
    return false;
}


bool
GNECalibrator::routeExists(std::string routeID) const {
    for (auto i : myCalibratorRoutes) {
        if (i.getRouteID() == routeID) {
            return true;
        }
    }
    return false;
}


const GNECalibratorVehicleType&
GNECalibrator::getCalibratorVehicleType(const std::string& vehicleTypeID) {
    for (auto &i : myCalibratorVehicleTypes) {
        if (i.getVehicleTypeID() == vehicleTypeID) {
            return i;
        }
    }
    throw InvalidArgument(toString(getTag()) + " " + getID() + " doesn't have a " + toString(SUMO_TAG_VTYPE) + " with id = '" + vehicleTypeID + "'");
}


const GNECalibratorFlow&
GNECalibrator::getCalibratorFlow(const std::string& flowID) {
    for (auto &i : myCalibratorFlows) {
        if (i.getFlowID() == flowID) {
            return i;
        }
    }
    throw InvalidArgument(toString(getTag()) + " " + getID() + " doesn't have a " + toString(SUMO_TAG_FLOW) + " with id = '" + flowID + "'");
}


const GNECalibratorRoute&
GNECalibrator::getCalibratorRoute(const std::string& routeID) {
    for (auto  &i : myCalibratorRoutes) {
        if (i.getRouteID() == routeID) {
            return i;
        }
    }
    throw InvalidArgument(toString(getTag()) + " " + getID() + " doesn't have a " + toString(SUMO_TAG_ROUTE) + " with id = '" + routeID + "'");
}


std::string
GNECalibrator::getAttribute(SumoXMLAttr key) const {
    switch (key) {
    case SUMO_ATTR_ID:
        return getAdditionalID();
    case SUMO_ATTR_EDGE:    // Only called by subClass GNECalibratorEdge
        return myEdge->getID();
    case SUMO_ATTR_LANE:    // Only called by subClass GNECalibratorLane
        return myLane->getID();
    case SUMO_ATTR_POSITION:
        if(myEdge) {
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
    case SUMO_ATTR_EDGE:    // Only called by subClass GNECalibratorEdge
    case SUMO_ATTR_LANE:    // Only called by subClass GNECalibratorLane
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
    case SUMO_ATTR_EDGE:    // Only called by subClass GNECalibratorEdge
        if (myViewNet->getNet()->retrieveEdge(value, false) != NULL) {
            return true;
        } else {
            return false;
        }
    case SUMO_ATTR_LANE:    // Only called by subClass GNECalibratorLane
        if (myViewNet->getNet()->retrieveLane(value, false) != NULL) {
            return true;
        } else {
            return false;
        }
    case SUMO_ATTR_POSITION:
        if (canParse<double>(value)) {
            // obtain relative new start position
            double newStartPos;
            if(myEdge) {
                newStartPos = parse<double>(value) / myEdge->getLanes().at(0)->getLaneParametricLength();
            } else if (myLane) {
                newStartPos = parse<double>(value) / myLane->getLaneParametricLength();
            } else {
                throw ProcessError("Both myEdge and myLane aren't defined");
            }
            if ((newStartPos < 0) || (newStartPos > 1)) {
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

void
GNECalibrator::setCalibratorVehicleTypes(const std::vector<GNECalibratorVehicleType>& calibratorVehicleTypes) {
    myCalibratorVehicleTypes = calibratorVehicleTypes;
}


void
GNECalibrator::setCalibratorFlows(const std::vector<GNECalibratorFlow>& calibratorFlows) {
    myCalibratorFlows = calibratorFlows;
}


void
GNECalibrator::setCalibratorRoutes(const std::vector<GNECalibratorRoute>& calibratorRoutes) {
    myCalibratorRoutes = calibratorRoutes;
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
    case SUMO_ATTR_EDGE:    // Only called by subClass GNECalibratorEdge
        myEdge = changeEdge(myEdge, value);
        break;
    case SUMO_ATTR_LANE:    // Only called by subClass GNECalibratorLane
        myLane = changeLane(myLane, value);
        break;
    case SUMO_ATTR_POSITION:
        if(myEdge) {
            myPositionOverLane = parse<double>(value) / myEdge->getLanes().at(0)->getShape().length();
        } else if (myLane) {
            myPositionOverLane = parse<double>(value) / myLane->getShape().length();
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
