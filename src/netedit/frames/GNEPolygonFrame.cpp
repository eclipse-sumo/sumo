/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEPolygonFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2017
/// @version $Id$
///
// The Widget for add polygons
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <iostream>
#include <utils/foxtools/fxexdefs.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/common/MsgHandler.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEAttributeCarrier.h>
#include <netedit/additionals/GNEPoly.h>
#include <netedit/additionals/GNEPOI.h>
#include <netedit/netelements/GNELane.h>

#include "GNEPolygonFrame.h"


// ===========================================================================
// method definitions
// ===========================================================================

GNEPolygonFrame::GNEPolygonFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "Shapes") {

    // create item Selector modul for shapes
    myItemSelector = new ItemSelector(this, GNEAttributeCarrier::TAGProperty::TAGPROPERTY_SHAPE);

    // Create shape parameters
    myShapeAttributes = new GNEPolygonFrame::ACAttributes(this);

    // Create Netedit parameter
    myNeteditAttributes = new GNEPolygonFrame::NeteditAttributes(this);

    // Create drawing controls
    myDrawingShape = new GNEPolygonFrame::DrawingShape(this);

    // set polygon as default shape
    myItemSelector->setCurrentTypeTag(SUMO_TAG_POLY);
}


GNEPolygonFrame::~GNEPolygonFrame() {
}


GNEPolygonFrame::AddShapeResult
GNEPolygonFrame::processClick(const Position& clickedPosition, GNELane* lane) {
    // Declare map to keep values
    std::map<SumoXMLAttr, std::string> valuesOfElement;
    // check if current selected shape is valid
    if (myItemSelector->getCurrentTypeTag() == SUMO_TAG_POI) {
        // show warning dialogbox and stop if input parameters are invalid
        if (myShapeAttributes->areValuesValid() == false) {
            myShapeAttributes->showWarningMessage();
            return ADDSHAPE_INVALID;
        }
        // obtain shape attributes and values
        valuesOfElement = myShapeAttributes->getAttributesAndValues();
        // obtain netedit attributes and values
        myNeteditAttributes->getNeteditAttributesAndValues(valuesOfElement, lane);
        // generate new ID
        valuesOfElement[SUMO_ATTR_ID] = myViewNet->getNet()->generateShapeID(myItemSelector->getCurrentTypeTag());
        // obtain position
        valuesOfElement[SUMO_ATTR_POSITION] = toString(clickedPosition);
        // return ADDSHAPE_SUCCESS if POI was sucesfully created
        if (addPOI(valuesOfElement)) {
            return ADDSHAPE_SUCCESS;
        } else {
            return ADDSHAPE_INVALID;
        }
    } else  if (myItemSelector->getCurrentTypeTag() == SUMO_TAG_POILANE) {
        // abort if lane is nullptr
        if (lane == nullptr) {
            WRITE_WARNING(toString(SUMO_TAG_POILANE) + " can be only placed over lanes");
            return ADDSHAPE_INVALID;
        }
        // show warning dialogbox and stop if input parameters are invalid
        if (myShapeAttributes->areValuesValid() == false) {
            myShapeAttributes->showWarningMessage();
            return ADDSHAPE_INVALID;
        }
        // obtain shape attributes and values
        valuesOfElement = myShapeAttributes->getAttributesAndValues();
        // obtain netedit attributes and values
        myNeteditAttributes->getNeteditAttributesAndValues(valuesOfElement, lane);
        // generate new ID
        valuesOfElement[SUMO_ATTR_ID] = myViewNet->getNet()->generateShapeID(myItemSelector->getCurrentTypeTag());
        // obtain Lane
        valuesOfElement[SUMO_ATTR_LANE] = lane->getID();
        // obtain position over lane
        valuesOfElement[SUMO_ATTR_POSITION] = toString(lane->getShape().nearest_offset_to_point2D(clickedPosition));
        // return ADDSHAPE_SUCCESS if POI was sucesfully created
        if (addPOILane(valuesOfElement)) {
            return ADDSHAPE_SUCCESS;
        } else {
            return ADDSHAPE_INVALID;
        }
    } else if (myItemSelector->getCurrentTypeTag() == SUMO_TAG_POLY) {
        if (myDrawingShape->isDrawing()) {
            // add or delete a new point depending of flag "delete last created point"
            if (myDrawingShape->getDeleteLastCreatedPoint()) {
                myDrawingShape->removeLastPoint();
            } else {
                myDrawingShape->addNewPoint(clickedPosition);
            }
            return ADDSHAPE_UPDATEDTEMPORALSHAPE;
        } else {
            // return ADDSHAPE_NOTHING if is drawing isn't enabled
            return ADDSHAPE_NOTHING;
        }
    } else {
        myViewNet->setStatusBarText("Current selected shape isn't valid.");
        return ADDSHAPE_INVALID;
    }
}


void
GNEPolygonFrame::show() {
    // Show frame
    GNEFrame::show();
}


std::string
GNEPolygonFrame::getIdsSelected(const FXList* list) {
    // Obtain Id's of list
    std::string vectorOfIds;
    for (int i = 0; i < list->getNumItems(); i++) {
        if (list->isItemSelected(i)) {
            if (vectorOfIds.size() > 0) {
                vectorOfIds += " ";
            }
            vectorOfIds += (list->getItem(i)->getText()).text();
        }
    }
    return vectorOfIds;
}


GNEPolygonFrame::DrawingShape*
GNEPolygonFrame::getDrawingShape() const {
    return myDrawingShape;
}


bool
GNEPolygonFrame::buildShape() {
    // show warning dialogbox and stop check if input parameters are valid
    if (myShapeAttributes->areValuesValid() == false) {
        myShapeAttributes->showWarningMessage();
        return false;
    } else if(myDrawingShape->getTemporalShape().size() == 0) {
        WRITE_WARNING("Polygon shape cannot be empty");
        return false;
    } else {
        // Declare map to keep values
        std::map<SumoXMLAttr, std::string> valuesOfElement = myShapeAttributes->getAttributesAndValues();

        // obtain netedit attributes and values
        myNeteditAttributes->getNeteditAttributesAndValues(valuesOfElement, nullptr);

        // generate new ID
        valuesOfElement[SUMO_ATTR_ID] = myViewNet->getNet()->generateShapeID(SUMO_TAG_POLY);

        // obtain shape and check if has to be closed
        PositionVector temporalShape = myDrawingShape->getTemporalShape();
        if(valuesOfElement[GNE_ATTR_CLOSE_SHAPE] == "true") {
            temporalShape.closePolygon();
        }
        valuesOfElement[SUMO_ATTR_SHAPE] = toString(temporalShape);

        // obtain geo (by default false)
        valuesOfElement[SUMO_ATTR_GEO] = "false";

        // return ADDSHAPE_SUCCESS if POI was sucesfully created
        return addPolygon(valuesOfElement);
    }
}


void 
GNEPolygonFrame::enableModuls(const GNEAttributeCarrier::TagValues &tagProperties) {
    // if there are parmeters, show and Recalc groupBox
    myShapeAttributes->showACAttributesModul(myItemSelector->getCurrentTypeTag(), tagProperties);
    // show netedit attributes
    myNeteditAttributes->showNeteditAttributesModul(tagProperties);
    // Check if drawing mode has to be shown
    if (myItemSelector->getCurrentTypeTag() == SUMO_TAG_POLY) {
        getDrawingShape()->showDrawingShape();
    } else {
        getDrawingShape()->hideDrawingShape();
    }
}


void 
GNEPolygonFrame::disableModuls() {
    // hide all widgets
    myShapeAttributes->hideACAttributesModul();
    myNeteditAttributes->hideNeteditAttributesModul();
    getDrawingShape()->hideDrawingShape();
}


bool
GNEPolygonFrame::addPolygon(const std::map<SumoXMLAttr, std::string>& polyValues) {
    // parse attributes from polyValues
    std::string id = polyValues.at(SUMO_ATTR_ID);
    std::string type = polyValues.at(SUMO_ATTR_TYPE);
    RGBColor color = RGBColor::parseColor(polyValues.at(SUMO_ATTR_COLOR));
    std::string layerStr = polyValues.at(SUMO_ATTR_LAYER);
    double angle = GNEAttributeCarrier::parse<double>(polyValues.at(SUMO_ATTR_ANGLE));
    std::string imgFile = polyValues.at(SUMO_ATTR_IMGFILE);
    bool relativePath = GNEAttributeCarrier::parse<bool>(polyValues.at(SUMO_ATTR_RELATIVEPATH));
    PositionVector shape = GNEAttributeCarrier::parse<PositionVector>(polyValues.at(SUMO_ATTR_SHAPE));
    bool fill = GNEAttributeCarrier::parse<bool>(polyValues.at(SUMO_ATTR_FILL));
    double lineWidth = GNEAttributeCarrier::parse<double>(polyValues.at(SUMO_ATTR_LINEWIDTH));
    // parse layer
    double layer = GNEAttributeCarrier::canParse<double>(layerStr) ? GNEAttributeCarrier::parse<double>(layerStr) : Shape::DEFAULT_LAYER;
    // create new Polygon only if number of shape points is greather than 2
    myViewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_POLY));
    if ((shape.size() > 0) && myViewNet->getNet()->addPolygon(id, type, color, layer, angle, imgFile, relativePath, shape, false, fill, lineWidth)) {
        // set manually attributes use GEO, block movement and block shape
        GNEPoly* polygon = myViewNet->getNet()->retrievePolygon(id);
        polygon->setAttribute(GNE_ATTR_BLOCK_MOVEMENT, polyValues.at(GNE_ATTR_BLOCK_MOVEMENT), myViewNet->getUndoList());
        polygon->setAttribute(GNE_ATTR_BLOCK_SHAPE, polyValues.at(GNE_ATTR_BLOCK_SHAPE), myViewNet->getUndoList());
        myViewNet->getUndoList()->p_end();
        return true;
    } else {
        // abort creation
        myViewNet->getUndoList()->p_abort();
        return false;
    }
}


bool
GNEPolygonFrame::addPOI(const std::map<SumoXMLAttr, std::string>& POIValues) {
    // parse attributes from POIValues
    std::string id = POIValues.at(SUMO_ATTR_ID);
    std::string type = POIValues.at(SUMO_ATTR_TYPE);
    RGBColor color = RGBColor::parseColor(POIValues.at(SUMO_ATTR_COLOR));
    std::string layerStr = POIValues.at(SUMO_ATTR_LAYER);
    Position pos = GNEAttributeCarrier::parse<Position>(POIValues.at(SUMO_ATTR_POSITION));
    double angle = GNEAttributeCarrier::parse<double>(POIValues.at(SUMO_ATTR_ANGLE));
    std::string imgFile = POIValues.at(SUMO_ATTR_IMGFILE);
    bool relativePath = GNEAttributeCarrier::parse<bool>(POIValues.at(SUMO_ATTR_RELATIVEPATH));
    double widthPOI = GNEAttributeCarrier::parse<double>(POIValues.at(SUMO_ATTR_WIDTH));
    double heightPOI = GNEAttributeCarrier::parse<double>(POIValues.at(SUMO_ATTR_HEIGHT));
    // parse layer
    double layer = GNEAttributeCarrier::canParse<double>(layerStr) ? GNEAttributeCarrier::parse<double>(layerStr) : Shape::DEFAULT_LAYER_POI;
    // create new POI
    myViewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_POI));
    if (myViewNet->getNet()->addPOI(id, type, color, pos, false, "", 0, 0, layer, angle, imgFile, relativePath, widthPOI, heightPOI)) {
        // Set manually the attribute block movement
        GNEPOI* poi = myViewNet->getNet()->retrievePOI(id);
        poi->setAttribute(GNE_ATTR_BLOCK_MOVEMENT, POIValues.at(GNE_ATTR_BLOCK_MOVEMENT), myViewNet->getUndoList());
        myViewNet->getUndoList()->p_end();
        return true;
    } else {
        // abort creation
        myViewNet->getUndoList()->p_abort();
        return false;
    }
}


bool
GNEPolygonFrame::addPOILane(const std::map<SumoXMLAttr, std::string>& POIValues) {
    // parse attributes from POIValues
    std::string id = POIValues.at(SUMO_ATTR_ID);
    std::string type = POIValues.at(SUMO_ATTR_TYPE);
    RGBColor color = RGBColor::parseColor(POIValues.at(SUMO_ATTR_COLOR));
    std::string layerStr = POIValues.at(SUMO_ATTR_LAYER);
    double angle = GNEAttributeCarrier::parse<double>(POIValues.at(SUMO_ATTR_ANGLE));
    std::string imgFile = POIValues.at(SUMO_ATTR_IMGFILE);
    bool relativePath = GNEAttributeCarrier::parse<bool>(POIValues.at(SUMO_ATTR_RELATIVEPATH));
    GNELane* lane = myViewNet->getNet()->retrieveLane(POIValues.at(SUMO_ATTR_LANE));
    double posLane = GNEAttributeCarrier::parse<double>(POIValues.at(SUMO_ATTR_POSITION));
    double posLat = GNEAttributeCarrier::parse<double>(POIValues.at(SUMO_ATTR_POSITION_LAT));
    double widthPOI = GNEAttributeCarrier::parse<double>(POIValues.at(SUMO_ATTR_WIDTH));
    double heightPOI = GNEAttributeCarrier::parse<double>(POIValues.at(SUMO_ATTR_HEIGHT));
    // parse layer
    double layer = GNEAttributeCarrier::canParse<double>(layerStr) ? GNEAttributeCarrier::parse<double>(layerStr) : Shape::DEFAULT_LAYER_POI;
    // create new POILane
    myViewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_POILANE));
    if (myViewNet->getNet()->addPOI(id, type, color, Position(), false, lane->getID(), posLane, posLat, layer, angle, imgFile, relativePath, widthPOI, heightPOI)) {
        // Set manually the attribute block movement
        GNEPOI* POI = myViewNet->getNet()->retrievePOI(id);
        POI->setAttribute(GNE_ATTR_BLOCK_MOVEMENT, POIValues.at(GNE_ATTR_BLOCK_MOVEMENT), myViewNet->getUndoList());
        myViewNet->getUndoList()->p_end();
        return true;
    } else {
        // abort creation
        myViewNet->getUndoList()->p_abort();
        return false;
    }
}

/****************************************************************************/
