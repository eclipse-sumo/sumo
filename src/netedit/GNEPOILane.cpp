/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEPOILane.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2017
/// @version $Id$
///
// A class for visualizing and editing POIS in netedit (adapted from
// GUIPointOfInterest and NLHandler)
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
#include <utility>
#include <utils/foxtools/MFXImageHelper.h>
#include <utils/geom/Position.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/common/MsgHandler.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <netimport/NIImporter_SUMO.h>
#include <netwrite/NWWriter_SUMO.h>
#include "GNENet.h"
#include "GNEEdge.h"
#include "GNEUndoList.h"
#include "GNEViewNet.h"
#include "GNEChange_Attribute.h"
#include "GNEPOILane.h"
#include "GNELane.h"


// ===========================================================================
// method definitions
// ===========================================================================

GNEPOILane::GNEPOILane(GNENet* net, const std::string& id, const std::string& type, const RGBColor& color,
                       double layer, double angle, const std::string& imgFile, GNELane* lane, double posOverLane, double posLat,
                       double width, double height, bool movementBlocked) :
    GUIPointOfInterest(id, type, color, Position(), false, lane->getID(), posOverLane / lane->getLaneParametricLength(), posLat, layer, angle, imgFile, width, height),
    GNEShape(net, SUMO_TAG_POILANE, ICON_LOCATEPOI, movementBlocked, false),
    myGNELane(lane) {
}


GNEPOILane::~GNEPOILane() {}


void
GNEPOILane::writeShape(OutputDevice& device) {
    // obtain fixed position over lane
    double fixedPositionOverLane = myPosOverLane > 1 ? 1 : myPosOverLane < 0 ? 0 : myPosOverLane;
    // write POILane using POI::writeXML
    writeXML(device, false, 0, myGNELane->getID(), fixedPositionOverLane * myGNELane->getShape().length(), myPosLat);
}


void
GNEPOILane::moveGeometry(const Position& oldPos, const Position& offset) {
    // Calculate new position using old position
    Position newPosition = oldPos;
    newPosition.add(offset);
    myPosOverLane = myGNELane->getShape().nearest_offset_to_point2D(newPosition, false) / myGNELane->getLaneShapeLength();
    // Update geometry
    updateGeometry();
}


void
GNEPOILane::commitGeometryMoving(const Position& oldPos, GNEUndoList* undoList) {
    if (!myBlockMovement) {
        // restore old position before commit new position
        double originalPosOverLane = myGNELane->getShape().nearest_offset_to_point2D(oldPos, false);
        undoList->p_begin("position of " + toString(getTag()));
        undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_POSITION, toString(myPosOverLane * myGNELane->getLaneParametricLength()), true, toString(originalPosOverLane)));
        undoList->p_end();
    }
}


GNELane*
GNEPOILane::getLane() const {
    return myGNELane;
}


void
GNEPOILane::updateGeometry() {
    // obtain fixed position over lane
    double fixedPositionOverLane = myPosOverLane > 1 ? 1 : myPosOverLane < 0 ? 0 : myPosOverLane;
    // set new position regarding to lane
    set(myGNELane->getShape().positionAtOffset(fixedPositionOverLane * myGNELane->getLaneShapeLength(), myPosLat));
    // refresh element to avoid grabbings problem
    myNet->refreshElement(this);
}


Position
GNEPOILane::getPositionInView() const {
    return Position(x(), y());
}


GUIGlID
GNEPOILane::getGlID() const {
    return GUIPointOfInterest::getGlID();
}


const std::string&
GNEPOILane::getParentName() const {
    return myNet->getMicrosimID();
}


GUIGLObjectPopupMenu*
GNEPOILane::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    // build shape header
    buildShapePopupOptions(app, ret, myType);
    // add option for convert to GNEPOI
    new FXMenuCommand(ret, ("Release from " + toString(SUMO_TAG_LANE)).c_str(), GUIIconSubSys::getIcon(ICON_LANE), &parent, MID_GNE_POI_TRANSFORM);
    return ret;
}


GUIParameterTableWindow*
GNEPOILane::getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    return GUIPointOfInterest::getParameterWindow(app, parent);
}


Boundary
GNEPOILane::getCenteringBoundary() const {
    return GUIPointOfInterest::getCenteringBoundary();
}


void
GNEPOILane::drawGL(const GUIVisualizationSettings& s) const {
    GUIPointOfInterest::drawGL(s);
    // draw a label with the type of POI
    GLHelper::drawText("POI Lane", *this, myLayer + .1, 0.6, myColor.invertedColor());
    // draw lock icon
    drawLockIcon(*this + Position(0, -0.5), GLO_POI, 0.2);
}


std::string
GNEPOILane::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return myID;
        case SUMO_ATTR_COLOR:
            return toString(myColor);
        case SUMO_ATTR_LANE:
            return myLane;
        case SUMO_ATTR_POSITION:
            return toString(myPosOverLane * myGNELane->getLaneParametricLength());
        case SUMO_ATTR_POSITION_LAT:
            return toString(myPosLat);
        case SUMO_ATTR_TYPE:
            return myType;
        case SUMO_ATTR_LAYER:
            return toString(myLayer);
        case SUMO_ATTR_IMGFILE:
            return myImgFile;
        case SUMO_ATTR_WIDTH:
            return toString(getWidth());
        case SUMO_ATTR_HEIGHT:
            return toString(getHeight());
        case SUMO_ATTR_ANGLE:
            return toString(getNaviDegree());
        case GNE_ATTR_BLOCK_MOVEMENT:
            return toString(myBlockMovement);
        default:
            throw InvalidArgument(toString(getTag()) + " attribute '" + toString(key) + "' not allowed");
    }
}


void
GNEPOILane::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_COLOR:
        case SUMO_ATTR_LANE:
        case SUMO_ATTR_POSITION:
        case SUMO_ATTR_POSITION_LAT:
        case SUMO_ATTR_TYPE:
        case SUMO_ATTR_LAYER:
        case SUMO_ATTR_IMGFILE:
        case SUMO_ATTR_WIDTH:
        case SUMO_ATTR_HEIGHT:
        case SUMO_ATTR_ANGLE:
        case GNE_ATTR_BLOCK_MOVEMENT:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEPOILane::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidID(value) && (myNet->retrievePOI(value, false) == 0);
        case SUMO_ATTR_COLOR:
            return canParse<RGBColor>(value);
        case SUMO_ATTR_LANE:
            return (myNet->retrieveLane(value, false) != NULL);
        case SUMO_ATTR_POSITION:
            return canParse<double>(value);
        case SUMO_ATTR_POSITION_LAT:
            return canParse<double>(value);
        case SUMO_ATTR_TYPE:
            return true;
        case SUMO_ATTR_LAYER:
            return canParse<double>(value);
        case SUMO_ATTR_IMGFILE:
            if (value == "") {
                return true;
            } else if (isValidFilename(value)) {
                // check that image can be loaded
                return GUITexturesHelper::getTextureID(value) != -1;
            } else {
                return false;
            }
        case SUMO_ATTR_WIDTH:
            return canParse<double>(value);
        case SUMO_ATTR_HEIGHT:
            return canParse<double>(value);
        case SUMO_ATTR_ANGLE:
            return canParse<double>(value);
        case GNE_ATTR_BLOCK_MOVEMENT:
            return canParse<bool>(value);
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNEPOILane::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID: {
            std::string oldID = myID;
            myID = value;
            myNet->changeShapeID(this, oldID);
            break;
        }
        case SUMO_ATTR_COLOR:
            myColor = parse<RGBColor>(value);
            break;
        case SUMO_ATTR_LANE:
            myLane = value;
            myGNELane->removeShapeChild(this);
            myGNELane = myNet->retrieveLane(value);
            myGNELane->addShapeChild(this);
            updateGeometry();
            break;
        case SUMO_ATTR_POSITION:
            myPosOverLane = parse<double>(value) / myGNELane->getLaneParametricLength();
            break;
        case SUMO_ATTR_POSITION_LAT:
            myPosLat = parse<double>(value);
            break;
        case SUMO_ATTR_TYPE:
            myType = value;
            break;
        case SUMO_ATTR_LAYER:
            myLayer = parse<double>(value);
            break;
        case SUMO_ATTR_IMGFILE:
            myImgFile = value;
            break;
        case SUMO_ATTR_WIDTH:
            setWidth(parse<double>(value));
            break;
        case SUMO_ATTR_HEIGHT:
            setHeight(parse<double>(value));
            break;
        case SUMO_ATTR_ANGLE:
            setNaviDegree(parse<double>(value));
            break;
        case GNE_ATTR_BLOCK_MOVEMENT:
            myBlockMovement = parse<bool>(value);
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " attribute '" + toString(key) + "' not allowed");
    }
    // update Geometry after every change
    updateGeometry();
}

/****************************************************************************/
