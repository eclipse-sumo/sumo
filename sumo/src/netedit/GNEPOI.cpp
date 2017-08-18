/****************************************************************************/
/// @file    GNEPOI.cpp
/// @author  Jakob Erdmann
/// @date    Sept 2012
/// @version $Id$
///
// A class for visualizing and editing POIS in netedit (adapted from
// GUIPointOfInterest and NLHandler)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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

#include <string>
#include <utility>
#include <foreign/polyfonts/polyfonts.h>
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
               double layer, double angle, const std::string& imgFile,
               const Position& pos, double width, double height) :
    GUIPointOfInterest(id, type, color, pos, layer, angle, imgFile, width, height),
    GNEShape(net, SUMO_TAG_POI, ICON_LOCATEPOI),
    myLane(NULL),
    myPositionOverLane(0) {
}


GNEPOI::~GNEPOI() {}


void GNEPOI::writeShape(OutputDevice &device) {
    writeXML(device);
}


void 
GNEPOI::moveGeometry(const Position &newPosition) {
    set(newPosition);
    myNet->refreshElement(this);
}


void 
GNEPOI::commitGeometryMoving(const Position& oldPos, GNEUndoList* undoList) {

}


void 
GNEPOI::updateGeometry() {

}


Position 
GNEPOI::getPositionInView() const {
    return Position(x(), y());
}


const std::string& 
GNEPOI::getParentName() const {
    return myNet->getMicrosimID();
}


GUIGLObjectPopupMenu* 
GNEPOI::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    return GUIPointOfInterest::getPopUpMenu(app, parent);
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
}


std::string
GNEPOI::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return myID;
        case SUMO_ATTR_COLOR:
            return toString(myColor);
        case SUMO_ATTR_LANE:
            if(myLane) {
                return myLane->getID();
            } else {
                return "";
            }
        case SUMO_ATTR_POSITION:
            if(myLane) {
                return toString(myPositionOverLane);
            } else {
                return toString(Position(x(), y()));
            }
        case SUMO_ATTR_FILL:
            return myImgFile;
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
        default:
            throw InvalidArgument("POI attribute '" + toString(key) + "' not allowed");
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
        case SUMO_ATTR_FILL:
        case SUMO_ATTR_TYPE:
        case SUMO_ATTR_LAYER:
        case SUMO_ATTR_IMGFILE:
        case SUMO_ATTR_WIDTH:
        case SUMO_ATTR_HEIGHT:
        case SUMO_ATTR_ANGLE:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEPOI::isValid(SumoXMLAttr key, const std::string& value ) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidID(value) && (myNet->retrievePOI(value, false) == 0);
        case SUMO_ATTR_COLOR:
            return canParse<RGBColor>(value);
        case SUMO_ATTR_LANE:
            if(value == "") {
                return true;
            } else {
                return (myNet->retrieveLane(value, false) != NULL);
            }
        case SUMO_ATTR_POSITION:
            if(myLane != NULL) {
                return canParse<double>(value);
            } else {
                bool ok;
                return GeomConvHelper::parseShapeReporting(value, "user-supplied position", 0, ok, false).size() == 1;
            }
        case SUMO_ATTR_FILL:
            return canParse<bool>(value);
        case SUMO_ATTR_TYPE:
            return true;
        case SUMO_ATTR_LAYER:
            return canParse<double>(value);
        case SUMO_ATTR_IMGFILE:
            return isValidFilename(value);
        case SUMO_ATTR_WIDTH:
            return canParse<double>(value);
        case SUMO_ATTR_HEIGHT:
            return canParse<double>(value);
        case SUMO_ATTR_ANGLE:
            return canParse<double>(value);
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
            myNet->changePOIID(this, oldID);
            break;
        }
        case SUMO_ATTR_COLOR:
            myColor = parse<RGBColor>(value);
            break;
        case SUMO_ATTR_LANE:
            myLane = myNet->retrieveLane(value, false);
        case SUMO_ATTR_POSITION:
            if(myLane) {
                myPositionOverLane = parse<double>(value);
            } else {
                bool ok = true;
                Position p = GeomConvHelper::parseShapeReporting(value, "netedit-given", 0, ok, false)[0];
                setx(p.x());
                sety(p.y());
            }
        case SUMO_ATTR_FILL:
            myImgFile = value;
        case SUMO_ATTR_TYPE:
            myType = value;
        case SUMO_ATTR_LAYER:
            myLayer = parse<double>(value);
        case SUMO_ATTR_IMGFILE:
            myImgFile = value;
        case SUMO_ATTR_WIDTH:
            setWidth(parse<double>(value));
        case SUMO_ATTR_HEIGHT:
            setHeight(parse<double>(value));
        case SUMO_ATTR_ANGLE:
            return setNaviDegree(parse<double>(value));
        default:
            throw InvalidArgument("POI attribute '" + toString(key) + "' not allowed");
    }
}

/****************************************************************************/
