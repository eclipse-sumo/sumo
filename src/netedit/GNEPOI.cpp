/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEPOI.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2017
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
#include "GNEPOI.h"
#include "GNELane.h"


// ===========================================================================
// method definitions
// ===========================================================================

GNEPOI::GNEPOI(GNENet* net, const std::string& id, const std::string& type, const RGBColor& color,
               const Position& pos, bool geo, double layer, double angle, const std::string& imgFile,
               bool relativePath, double width, double height, bool movementBlocked) :
    GUIPointOfInterest(id, type, color, pos, geo, "", 0, 0, layer, angle, imgFile, relativePath, width, height),
    GNEShape(net, SUMO_TAG_POI, ICON_LOCATEPOI, movementBlocked, false),
    myGNELane(NULL) {
    // set GEO Position
    myGEOPosition = pos;
    GeoConvHelper::getFinal().cartesian2geo(myGEOPosition);
}


GNEPOI::GNEPOI(GNENet* net, const std::string& id, const std::string& type, const RGBColor& color,
    double layer, double angle, const std::string& imgFile, bool relativePath, GNELane* lane, double posOverLane, double posLat,
    double width, double height, bool movementBlocked) :
    GUIPointOfInterest(id, type, color, Position(), false, lane->getID(), posOverLane / lane->getLaneParametricLength(), posLat, layer, angle, imgFile, relativePath, width, height),
    GNEShape(net, SUMO_TAG_POILANE, ICON_LOCATEPOI, movementBlocked, false),
    myGNELane(lane) {
}


GNEPOI::~GNEPOI() {}


void GNEPOI::writeShape(OutputDevice& device) {
    if(myGNELane) {
        // obtain fixed position over lane
        double fixedPositionOverLane = myPosOverLane > 1 ? 1 : myPosOverLane < 0 ? 0 : myPosOverLane;
        // write POILane using POI::writeXML
        writeXML(device, false, 0, myGNELane->getID(), fixedPositionOverLane * myGNELane->getShape().length(), myPosLat);
    } else {
        writeXML(device, myGeo);
    }
}


void
GNEPOI::moveGeometry(const Position& oldPos, const Position& offset) {
    if (!myBlockMovement) {
        if (myGNELane) {
            // Calculate new position using old position
            Position newPosition = oldPos;
            newPosition.add(offset);
            myPosOverLane = myGNELane->getShape().nearest_offset_to_point2D(newPosition, false) / myGNELane->getLaneShapeLength();
            // Update geometry
            updateGeometry();
        } else {
            // restore old position, apply offset and refresh element
            set(oldPos);
            add(offset);
            myNet->refreshElement(this);
        }
    }
}


void
GNEPOI::commitGeometryMoving(const Position& oldPos, GNEUndoList* undoList) {
    if (!myBlockMovement) {
        if (myGNELane) {
            // restore old position before commit new position
            double originalPosOverLane = myGNELane->getShape().nearest_offset_to_point2D(oldPos, false);
            undoList->p_begin("position of " + toString(getTag()));
            undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_POSITION, toString(myPosOverLane * myGNELane->getLaneParametricLength()), true, toString(originalPosOverLane)));
            undoList->p_end();
        } else {
            undoList->p_begin("position of " + toString(getTag()));
            undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_POSITION, toString(getPositionInView()), true, toString(oldPos)));
            undoList->p_end();
        }
    }
}


GNELane*
GNEPOI::getLane() const {
    return myGNELane;
}


void
GNEPOI::updateGeometry() {
    if (myGNELane) {
        // obtain fixed position over lane
        double fixedPositionOverLane = myPosOverLane > 1 ? 1 : myPosOverLane < 0 ? 0 : myPosOverLane;
        // set new position regarding to lane
        set(myGNELane->getShape().positionAtOffset(fixedPositionOverLane * myGNELane->getLaneShapeLength(), myPosLat));
    }
    // refresh element to avoid grabbings problem
    myNet->refreshElement(this);
}


Position
GNEPOI::getPositionInView() const {
    return Position(x(), y());
}


GUIGlID
GNEPOI::getGlID() const {
    return GUIPointOfInterest::getGlID();
}


const std::string&
GNEPOI::getParentName() const {
    return myNet->getMicrosimID();
}


GUIGLObjectPopupMenu*
GNEPOI::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    if(myGNELane) {
        // build shape header
        buildShapePopupOptions(app, ret, getShapeType());
        // add option for convert to GNEPOI
        new FXMenuCommand(ret, ("Release from " + toString(SUMO_TAG_LANE)).c_str(), GUIIconSubSys::getIcon(ICON_LANE), &parent, MID_GNE_POI_TRANSFORM);
        return ret;
    } else {
        // build shape header
        buildShapePopupOptions(app, ret, getShapeType());
        // add option for convert to GNEPOI
        new FXMenuCommand(ret, ("Attach to nearest " + toString(SUMO_TAG_LANE)).c_str(), GUIIconSubSys::getIcon(ICON_LANE), &parent, MID_GNE_POI_TRANSFORM);
    }
    return ret;
}


GUIParameterTableWindow*
GNEPOI::getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    return GUIPointOfInterest::getParameterWindow(app, parent);
}


Boundary
GNEPOI::getCenteringBoundary() const {
    return GUIPointOfInterest::getCenteringBoundary();
}


void
GNEPOI::drawGL(const GUIVisualizationSettings& s) const {
    GUIPointOfInterest::drawGL(s);
    // draw a label with the type of POI
    if(myGNELane) {
        GLHelper::drawText("POI Lane", *this, getShapeLayer() + .1, 0.6, getShapeColor().invertedColor());
    } else {
        GLHelper::drawText("POI", *this, getShapeLayer() + .1, 0.6, getShapeColor().invertedColor());
    }
    // draw lock icon
    drawLockIcon(*this + Position(0, -0.5), GLO_POI, 0.2);
}


std::string
GNEPOI::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return myID;
        case SUMO_ATTR_COLOR:
            return toString(getShapeColor());
        case SUMO_ATTR_LANE:
            return myLane;
        case SUMO_ATTR_POSITION:
            if(myGNELane) {
                return toString(myPosOverLane * myGNELane->getLaneParametricLength());
            } else {
                return toString(*this);
            }
        case SUMO_ATTR_POSITION_LAT:
            return toString(myPosLat);
        case SUMO_ATTR_GEOPOSITION:
            return toString(myGEOPosition);
        case SUMO_ATTR_GEO:
            return toString(myGeo);
        case SUMO_ATTR_TYPE:
            return getShapeType();
        case SUMO_ATTR_LAYER:
            return toString(getShapeLayer());
        case SUMO_ATTR_IMGFILE:
            return getShapeImgFile();
        case SUMO_ATTR_RELATIVEPATH:
            return toString(getShapeRelativePath());
        case SUMO_ATTR_WIDTH:
            return toString(getWidth());
        case SUMO_ATTR_HEIGHT:
            return toString(getHeight());
        case SUMO_ATTR_ANGLE:
            return toString(getShapeNaviDegree());
        case GNE_ATTR_BLOCK_MOVEMENT:
            return toString(myBlockMovement);
        default:
            throw InvalidArgument(toString(getTag()) + " attribute '" + toString(key) + "' not allowed");
    }
}


void
GNEPOI::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_COLOR:
        case SUMO_ATTR_LANE:
        case SUMO_ATTR_POSITION:
        case SUMO_ATTR_POSITION_LAT:
        case SUMO_ATTR_GEOPOSITION:
        case SUMO_ATTR_GEO:
        case SUMO_ATTR_TYPE:
        case SUMO_ATTR_LAYER:
        case SUMO_ATTR_IMGFILE:
        case SUMO_ATTR_RELATIVEPATH:
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
GNEPOI::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidID(value) && (myNet->retrievePOI(value, false) == 0);
        case SUMO_ATTR_COLOR:
            return canParse<RGBColor>(value);
        case SUMO_ATTR_LANE:
            return (myNet->retrieveLane(value, false) != NULL);
        case SUMO_ATTR_POSITION: {
            bool ok;
            return GeomConvHelper::parseShapeReporting(value, "user-supplied position", 0, ok, false).size() == 1;
        }
        case SUMO_ATTR_POSITION_LAT:
            return canParse<double>(value);
        case SUMO_ATTR_GEOPOSITION: {
            bool ok;
            return GeomConvHelper::parseShapeReporting(value, "user-supplied GEO position", 0, ok, false).size() == 1;
        }
        case SUMO_ATTR_GEO:
            return canParse<bool>(value);
        case SUMO_ATTR_TYPE:
            return true;
        case SUMO_ATTR_LAYER:
            return canParse<double>(value);
        case SUMO_ATTR_IMGFILE:
            if (value == "") {
                return true;
            } else {
                // check that image can be loaded
                return GUITexturesHelper::getTextureID(value) != -1;
            }
        case SUMO_ATTR_RELATIVEPATH:
            return canParse<bool>(value);
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
GNEPOI::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID: {
            std::string oldID = myID;
            myID = value;
            myNet->changeShapeID(this, oldID);
            break;
        }
        case SUMO_ATTR_COLOR:
            setShapeColor(parse<RGBColor>(value));
            break;
        case SUMO_ATTR_LANE:
            myLane = value;
            myGNELane->removeShapeChild(this);
            myGNELane = myNet->retrieveLane(value);
            myGNELane->addShapeChild(this);
            updateGeometry();
            break;
        case SUMO_ATTR_POSITION: {
            if(myGNELane) {
                myPosOverLane = parse<double>(value) / myGNELane->getLaneParametricLength();
            } else {
                bool ok = true;
                set(GeomConvHelper::parseShapeReporting(value, "netedit-given", 0, ok, false)[0]);
                // set GEO Position
                myGEOPosition = *this;
                GeoConvHelper::getFinal().cartesian2geo(myGEOPosition);
            }
            break;
        }
        case SUMO_ATTR_POSITION_LAT:
            myPosLat = parse<double>(value);
            break;
        case SUMO_ATTR_GEOPOSITION: {
            bool ok = true;
            myGEOPosition = GeomConvHelper::parseShapeReporting(value, "netedit-given", 0, ok, false)[0];
            // set cartesian Position
            set(myGEOPosition);
            GeoConvHelper::getFinal().x2cartesian_const(*this);
            break;
        }
        case SUMO_ATTR_GEO:
            myGeo = parse<bool>(value);
            break;
        case SUMO_ATTR_TYPE:
            setShapeType(value);
            break;
        case SUMO_ATTR_LAYER:
            setShapeLayer(parse<double>(value));
            break;
        case SUMO_ATTR_IMGFILE:
            setShapeImgFile(value);
            // all textures must be refresh
            GUITexturesHelper::clearTextures();
            break;
        case SUMO_ATTR_RELATIVEPATH:
            setShapeRelativePath(parse<bool>(value));
            break;
        case SUMO_ATTR_WIDTH:
            setWidth(parse<double>(value));
            break;
        case SUMO_ATTR_HEIGHT:
            setHeight(parse<double>(value));
            break;
        case SUMO_ATTR_ANGLE:
            setShapeNaviDegree(parse<double>(value));
            break;
        case GNE_ATTR_BLOCK_MOVEMENT:
            myBlockMovement = parse<bool>(value);
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " attribute '" + toString(key) + "' not allowed");
    }
    // Update geometry after every change
    updateGeometry();
}

/****************************************************************************/
