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
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <utils/foxtools/fxexdefs.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/common/MsgHandler.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/images/GUITexturesHelper.h>

#include "GNEPolygonFrame.h"
#include "GNEViewNet.h"
#include "GNEViewParent.h"
#include "GNENet.h"
#include "GNEUndoList.h"
#include "GNEChange_Selection.h"
#include "GNEAttributeCarrier.h"
#include "GNEPoly.h"
#include "GNEPOI.h"
#include "GNEPOILane.h"
#include "GNELane.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEPolygonFrame) GNEPolygonFrameMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_SELECTADDITIONALTYPE,   GNEPolygonFrame::onCmdSelectShape),
};

FXDEFMAP(GNEPolygonFrame::ShapeAttributeSingle) GNESingleShapeParameterMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_CHANGEPARAMETER_TEXT,   GNEPolygonFrame::ShapeAttributeSingle::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_CHANGEPARAMETER_BOOL,   GNEPolygonFrame::ShapeAttributeSingle::onCmdSetBooleanAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_INSPECTORFRAME_ATTRIBUTEEDITOR,         GNEPolygonFrame::ShapeAttributeSingle::onCmdSetColorAttribute),
};

FXDEFMAP(GNEPolygonFrame::ShapeAttributes) GNEShapeParametersMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,                                       GNEPolygonFrame::ShapeAttributes::onCmdHelp),
};

// Object implementation
FXIMPLEMENT(GNEPolygonFrame,                        FXVerticalFrame,    GNEPolygonFrameMap,             ARRAYNUMBER(GNEPolygonFrameMap))
FXIMPLEMENT(GNEPolygonFrame::ShapeAttributeSingle,  FXHorizontalFrame,  GNESingleShapeParameterMap,     ARRAYNUMBER(GNESingleShapeParameterMap))
FXIMPLEMENT(GNEPolygonFrame::ShapeAttributes,       FXGroupBox,         GNEShapeParametersMap,          ARRAYNUMBER(GNEShapeParametersMap))

// ===========================================================================
// method definitions
// ===========================================================================

GNEPolygonFrame::GNEPolygonFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet):
    GNEFrame(horizontalFrameParent, viewNet, "Shapes"),
    myActualShapeType(SUMO_TAG_NOTHING) {

    // Create groupBox for myShapeMatchBox
    myGroupBoxForMyShapeMatchBox = new FXGroupBox(myContentFrame, "Shape element", GUIDesignGroupBoxFrame);

    // Create FXListBox in myGroupBoxForMyShapeMatchBox
    myShapeMatchBox = new FXComboBox(myGroupBoxForMyShapeMatchBox, GUIDesignComboBoxNCol, this, MID_GNE_ADDITIONALFRAME_SELECTADDITIONALTYPE, GUIDesignComboBox);

    // Create shape parameters
    myShapeAttributes = new GNEPolygonFrame::ShapeAttributes(myViewNet, myContentFrame);

    // Create Netedit parameter
    myNeteditAttributes = new GNEPolygonFrame::NeteditAttributes(this);

    // Create drawing controls
    myDrawingMode = new GNEPolygonFrame::DrawingMode(this);

    // Add options to myShapeMatchBox
    for (auto i : GNEAttributeCarrier::allowedShapeTags()) {
        myShapeMatchBox->appendItem(toString(i).c_str());
    }

    // Set visible items
    myShapeMatchBox->setNumVisible((int)myShapeMatchBox->getNumItems());

    // If there are shapes
    if (GNEAttributeCarrier::allowedShapeTags().size() > 0) {
        // Set myActualShapeType and show
        myActualShapeType = GNEAttributeCarrier::allowedShapeTags().front();
        setParametersOfShape(myActualShapeType);
        // show drawing controls if we're creating a polygon
        if (myActualShapeType == SUMO_TAG_POLY) {
            myDrawingMode->showDrawingMode();
        } else {
            myDrawingMode->hideDrawingMode();
        }
    }
}


GNEPolygonFrame::~GNEPolygonFrame() {
    gSelected.remove2Update();
}


GNEPolygonFrame::AddShapeResult
GNEPolygonFrame::processClick(const Position& clickedPosition, GNELane* lane) {
    // Declare map to keep values
    std::map<SumoXMLAttr, std::string> valuesOfElement = myShapeAttributes->getAttributesAndValues();
    // check if current selected shape is valid
    if (myActualShapeType == SUMO_TAG_POI) {
        // show warning dialogbox and stop if input parameters are invalid
        if (myShapeAttributes->areValuesValid() == false) {
            myShapeAttributes->showWarningMessage();
            return ADDSHAPE_INVALID;
        }
        // generate new ID
        valuesOfElement[SUMO_ATTR_ID] = myViewNet->getNet()->generateShapeID(myActualShapeType);
        // obtain position
        valuesOfElement[SUMO_ATTR_POSITION] = toString(clickedPosition);
        // obtain block movement value
        valuesOfElement[GNE_ATTR_BLOCK_MOVEMENT] = toString(myNeteditAttributes->isBlockMovementEnabled());
        // return ADDSHAPE_SUCCESS if POI was sucesfully created
        if (addPOI(valuesOfElement)) {
            return ADDSHAPE_SUCCESS;
        } else {
            return ADDSHAPE_INVALID;
        }
    } else  if (myActualShapeType == SUMO_TAG_POILANE) {
        // show warning dialogbox and stop if input parameters are invalid
        if (myShapeAttributes->areValuesValid() == false) {
            myShapeAttributes->showWarningMessage();
            return ADDSHAPE_INVALID;
        }
        // abort if lane is NULL
        if (lane == NULL) {
            WRITE_WARNING(toString(SUMO_TAG_POILANE) + " can be only placed over lanes");
            return ADDSHAPE_INVALID;
        }
        // generate new ID
        valuesOfElement[SUMO_ATTR_ID] = myViewNet->getNet()->generateShapeID(myActualShapeType);
        // obtain Lane
        valuesOfElement[SUMO_ATTR_LANE] = lane->getID();
        // obtain position over lane
        valuesOfElement[SUMO_ATTR_POSITION] = toString(lane->getShape().nearest_offset_to_point2D(clickedPosition));
        // obtain block movement value
        valuesOfElement[GNE_ATTR_BLOCK_MOVEMENT] = toString(myNeteditAttributes->isBlockMovementEnabled());
        // return ADDSHAPE_SUCCESS if POI was sucesfully created
        if (addPOILane(valuesOfElement)) {
            return ADDSHAPE_SUCCESS;
        } else {
            return ADDSHAPE_INVALID;
        }
    } else if (myActualShapeType == SUMO_TAG_POLY) {
        if (myDrawingMode->isDrawing()) {
            myDrawingMode->addNewPoint(clickedPosition);
            return ADDSHAPE_NEWPOINT;
        } else {
            // return ADDSHAPE_NOTHING if is drawing isn't enabled
            return ADDSHAPE_NOTHING;
        }
    } else {
        myViewNet->setStatusBarText("Current selected shape isn't valid.");
        return ADDSHAPE_INVALID;
    }
}


bool
GNEPolygonFrame::buildPoly(const PositionVector& drawedShape) {
    // show warning dialogbox and stop check if input parameters are valid
    if (myShapeAttributes->areValuesValid() == false) {
        myShapeAttributes->showWarningMessage();
        return false;
    } else {
        // Declare map to keep values
        std::map<SumoXMLAttr, std::string> valuesOfElement = myShapeAttributes->getAttributesAndValues();

        // generate new ID
        valuesOfElement[SUMO_ATTR_ID] = myViewNet->getNet()->generateShapeID(SUMO_TAG_POLY);

        // obtain shape
        valuesOfElement[SUMO_ATTR_SHAPE] = toString(drawedShape);

        // obtain geo (by default false)
        valuesOfElement[SUMO_ATTR_GEO] = "false";

        // obtain block movement value
        valuesOfElement[GNE_ATTR_BLOCK_MOVEMENT] = toString(myNeteditAttributes->isBlockMovementEnabled());

        // obtain block shape value
        valuesOfElement[GNE_ATTR_BLOCK_SHAPE] = toString(myNeteditAttributes->isBlockShapeEnabled());

        // return ADDSHAPE_SUCCESS if POI was sucesfully created
        return addPolygon(valuesOfElement);
    }
}


long
GNEPolygonFrame::onCmdSelectShape(FXObject*, FXSelector, void*) {
    // Flag to save if name is valid
    bool shapeNameCorrect = false;
    // set parameters of shape, if it's correct
    for (auto i : GNEAttributeCarrier::allowedShapeTags()) {
        if (toString(i) == myShapeMatchBox->getText().text()) {
            myShapeMatchBox->setTextColor(FXRGB(0, 0, 0));
            myShapeAttributes->show();
            myNeteditAttributes->showNeteditAttributes(i == SUMO_TAG_POLY);
            setParametersOfShape(i);
            shapeNameCorrect = true;
        }
    }
    // if shape name isn't correct, hidde all
    if (shapeNameCorrect == false) {
        myActualShapeType = SUMO_TAG_NOTHING;
        myShapeMatchBox->setTextColor(FXRGB(255, 0, 0));
        myShapeAttributes->hide();
        myNeteditAttributes->hideNeteditAttributes();
        myDrawingMode->hideDrawingMode();
    } else {
        // show drawing controls if we're creating a polygon
        if (myActualShapeType == SUMO_TAG_POLY) {
            myDrawingMode->showDrawingMode();
        } else {
            myDrawingMode->hideDrawingMode();
        }
    }
    return 1;
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


void
GNEPolygonFrame::setParametersOfShape(SumoXMLTag actualShapeType) {
    // Set new actualShapeType
    myActualShapeType = actualShapeType;
    // Clear internal attributes
    myShapeAttributes->clearAttributes();
    // Iterate over attributes of myActualShapeType
    for (auto i : GNEAttributeCarrier::allowedAttributes(myActualShapeType)) {
        if (!GNEAttributeCarrier::isUnique(myActualShapeType, i.first)) {
            myShapeAttributes->addAttribute(myActualShapeType, i.first);
        }
    }
    // if there are parmeters, show and Recalc groupBox
    if (myShapeAttributes->getNumberOfAddedAttributes() > 0) {
        myShapeAttributes->showShapeParameters();
    } else {
        myShapeAttributes->hideShapeParameters();
    }
}


bool
GNEPolygonFrame::addPolygon(const std::map<SumoXMLAttr, std::string>& polyValues) {
    bool ok = true;
    // parse attributes from polyValues
    std::string id = polyValues.at(SUMO_ATTR_ID);
    std::string type = polyValues.at(SUMO_ATTR_TYPE);
    RGBColor color = RGBColor::parseColor(polyValues.at(SUMO_ATTR_COLOR));
    double layer = GNEAttributeCarrier::parse<double>(polyValues.at(SUMO_ATTR_LAYER));
    double angle = GNEAttributeCarrier::parse<double>(polyValues.at(SUMO_ATTR_ANGLE));
    std::string imgFile = polyValues.at(SUMO_ATTR_IMGFILE);
    PositionVector shape =  GeomConvHelper::parseShapeReporting(polyValues.at(SUMO_ATTR_SHAPE), "user-supplied position", 0, ok, true);
    bool fill = GNEAttributeCarrier::parse<bool>(polyValues.at(SUMO_ATTR_FILL));

    // create new Polygon only if number of shape points is greather than 2
    myViewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_POLY));
    if ((shape.size() > 0) && myViewNet->getNet()->addPolygon(id, type, color, layer, angle, imgFile, shape, false, fill)) {
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
    bool ok = true;
    // parse attributes from POIValues
    std::string id = POIValues.at(SUMO_ATTR_ID);
    std::string type = POIValues.at(SUMO_ATTR_TYPE);
    RGBColor color = RGBColor::parseColor(POIValues.at(SUMO_ATTR_COLOR));
    double layer = GNEAttributeCarrier::parse<double>(POIValues.at(SUMO_ATTR_LAYER));
    Position pos = GeomConvHelper::parseShapeReporting(POIValues.at(SUMO_ATTR_POSITION), "netedit-given", 0, ok, false)[0];
    double angle = GNEAttributeCarrier::parse<double>(POIValues.at(SUMO_ATTR_ANGLE));
    std::string imgFile = POIValues.at(SUMO_ATTR_IMGFILE);
    double widthPOI = GNEAttributeCarrier::parse<double>(POIValues.at(SUMO_ATTR_WIDTH));
    double heightPOI = GNEAttributeCarrier::parse<double>(POIValues.at(SUMO_ATTR_HEIGHT));

    // create new POI
    myViewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_POI));
    if (myViewNet->getNet()->addPOI(id, type, color, pos, false, "", 0, 0, layer, angle, imgFile, widthPOI, heightPOI)) {
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
    double layer = GNEAttributeCarrier::parse<double>(POIValues.at(SUMO_ATTR_LAYER));
    double angle = GNEAttributeCarrier::parse<double>(POIValues.at(SUMO_ATTR_ANGLE));
    std::string imgFile = POIValues.at(SUMO_ATTR_IMGFILE);
    GNELane* lane = myViewNet->getNet()->retrieveLane(POIValues.at(SUMO_ATTR_LANE));
    double posLane = GNEAttributeCarrier::parse<double>(POIValues.at(SUMO_ATTR_POSITION));
    double posLat = GNEAttributeCarrier::parse<double>(POIValues.at(SUMO_ATTR_POSITION_LAT));
    double widthPOI = GNEAttributeCarrier::parse<double>(POIValues.at(SUMO_ATTR_WIDTH));
    double heightPOI = GNEAttributeCarrier::parse<double>(POIValues.at(SUMO_ATTR_HEIGHT));

    // create new POILane
    myViewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_POILANE));
    if (myViewNet->getNet()->addPOI(id, type, color, Position(), false, lane->getID(), posLane, posLat, layer, angle, imgFile, widthPOI, heightPOI)) {
        // Set manually the attribute block movement
        GNEPOILane* POILane = myViewNet->getNet()->retrievePOILane(id);
        POILane->setAttribute(GNE_ATTR_BLOCK_MOVEMENT, POIValues.at(GNE_ATTR_BLOCK_MOVEMENT), myViewNet->getUndoList());
        myViewNet->getUndoList()->p_end();
        return true;
    } else {
        // abort creation
        myViewNet->getUndoList()->p_abort();
        return false;
    }
}

// ---------------------------------------------------------------------------
// GNEPolygonFrame::ShapeAttributeSingle - methods
// ---------------------------------------------------------------------------

GNEPolygonFrame::ShapeAttributeSingle::ShapeAttributeSingle(FXComposite* parent) :
    FXHorizontalFrame(parent, GUIDesignAuxiliarHorizontalFrame),
    myShapeTag(SUMO_TAG_NOTHING),
    myShapeAttr(SUMO_ATTR_NOTHING) {
    // Create visual elements
    myLabel = new FXLabel(this, "name", 0, GUIDesignLabelAttribute);
    mycolorEditor = new FXButton(this, "ColorButton", 0, this, MID_GNE_INSPECTORFRAME_ATTRIBUTEEDITOR, GUIDesignButtonAttribute);
    myTextFieldInt = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_ADDITIONALFRAME_CHANGEPARAMETER_TEXT, GUIDesignTextFieldInt);
    myTextFieldReal = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_ADDITIONALFRAME_CHANGEPARAMETER_TEXT, GUIDesignTextFieldReal);
    myTextFieldStrings = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_ADDITIONALFRAME_CHANGEPARAMETER_TEXT, GUIDesignTextField);
    myBoolCheckButton = new FXCheckButton(this, "Disabled", this, MID_GNE_ADDITIONALFRAME_CHANGEPARAMETER_BOOL, GUIDesignCheckButtonAttribute);
    // Hide elements
    hideParameter();
}


GNEPolygonFrame::ShapeAttributeSingle::~ShapeAttributeSingle() {}


void
GNEPolygonFrame::ShapeAttributeSingle::showParameter(SumoXMLTag shapeTag, SumoXMLAttr shapeAttr, std::string value) {
    myShapeTag = shapeTag;
    myShapeAttr = shapeAttr;
    myInvalidValue = "";
    myLabel->setText(toString(myShapeAttr).c_str());
    myLabel->show();
    myTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
    myTextFieldStrings->setText(value.c_str());
    myTextFieldStrings->show();
    show();
}


void
GNEPolygonFrame::ShapeAttributeSingle::showParameter(SumoXMLTag shapeTag, SumoXMLAttr shapeAttr, int value) {
    myShapeTag = shapeTag;
    myShapeAttr = shapeAttr;
    myInvalidValue = "";
    myLabel->setText(toString(myShapeAttr).c_str());
    myLabel->show();
    myTextFieldInt->setTextColor(FXRGB(0, 0, 0));
    myTextFieldInt->setText(toString(value).c_str());
    myTextFieldInt->show();
    show();
}


void
GNEPolygonFrame::ShapeAttributeSingle::showParameter(SumoXMLTag shapeTag, SumoXMLAttr shapeAttr, double value) {
    myShapeTag = shapeTag;
    myShapeAttr = shapeAttr;
    myInvalidValue = "";
    myLabel->setText(toString(myShapeAttr).c_str());
    myLabel->show();
    myTextFieldReal->setTextColor(FXRGB(0, 0, 0));
    myTextFieldReal->setText(toString(value).c_str());
    myTextFieldReal->show();
    show();
}


void
GNEPolygonFrame::ShapeAttributeSingle::showParameter(SumoXMLTag shapeTag, SumoXMLAttr shapeAttr, bool value) {
    myShapeTag = shapeTag;
    myShapeAttr = shapeAttr;
    myInvalidValue = "";
    myLabel->setText(toString(myShapeAttr).c_str());
    myLabel->show();
    if (value) {
        myBoolCheckButton->setCheck(true);
        myBoolCheckButton->setText("true");
    } else {
        myBoolCheckButton->setCheck(false);
        myBoolCheckButton->setText("false");
    }
    myBoolCheckButton->show();
    show();
}


void
GNEPolygonFrame::ShapeAttributeSingle::showParameter(SumoXMLTag shapeTag, SumoXMLAttr shapeAttr, RGBColor value) {
    myShapeTag = shapeTag;
    myShapeAttr = shapeAttr;
    myInvalidValue = "";
    mycolorEditor->setText(toString(myShapeAttr).c_str());
    mycolorEditor->show();
    myTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
    myTextFieldStrings->setText(toString(value).c_str());
    myTextFieldStrings->show();
    show();
}


void
GNEPolygonFrame::ShapeAttributeSingle::hideParameter() {
    myShapeTag = SUMO_TAG_NOTHING;
    myShapeAttr = SUMO_ATTR_NOTHING;
    myLabel->hide();
    myTextFieldInt->hide();
    myTextFieldReal->hide();
    myTextFieldStrings->hide();
    myBoolCheckButton->hide();
    mycolorEditor->hide();
    hide();
}


SumoXMLTag
GNEPolygonFrame::ShapeAttributeSingle::getTag() const {
    return myShapeTag;
}


SumoXMLAttr
GNEPolygonFrame::ShapeAttributeSingle::getAttr() const {
    return myShapeAttr;
}


std::string
GNEPolygonFrame::ShapeAttributeSingle::getValue() const {
    if (GNEAttributeCarrier::isBool(myShapeTag, myShapeAttr)) {
        return (myBoolCheckButton->getCheck() == 1) ? "true" : "false";
    } else if (GNEAttributeCarrier::isInt(myShapeTag, myShapeAttr)) {
        return myTextFieldInt->getText().text();
    } else if (GNEAttributeCarrier::isFloat(myShapeTag, myShapeAttr) || GNEAttributeCarrier::isTime(myShapeTag, myShapeAttr)) {
        return myTextFieldReal->getText().text();
    } else {
        return myTextFieldStrings->getText().text();
    }
}


const std::string&
GNEPolygonFrame::ShapeAttributeSingle::isAttributeValid() const {
    return myInvalidValue;
}


long
GNEPolygonFrame::ShapeAttributeSingle::onCmdSetAttribute(FXObject*, FXSelector, void*) {
    // We assume that current value is valid
    myInvalidValue = "";
    // Check if format of current value of myTextField is correct
    if (GNEAttributeCarrier::isInt(myShapeTag, myShapeAttr)) {
        if (GNEAttributeCarrier::canParse<int>(myTextFieldInt->getText().text())) {
            // convert string to int
            int intValue = GNEAttributeCarrier::parse<int>(myTextFieldInt->getText().text());
            // Check if int value must be positive
            if (GNEAttributeCarrier::isPositive(myShapeTag, myShapeAttr) && (intValue < 0)) {
                myInvalidValue = "'" + toString(myShapeAttr) + "' cannot be negative";
            }
        } else {
            myInvalidValue = "'" + toString(myShapeAttr) + "' doesn't have a valid 'int' format";
        }
    } else if (GNEAttributeCarrier::isTime(myShapeTag, myShapeAttr)) {
        // time attributes work as positive doubles
        if (GNEAttributeCarrier::canParse<double>(myTextFieldReal->getText().text())) {
            // convert string to double
            double doubleValue = GNEAttributeCarrier::parse<double>(myTextFieldReal->getText().text());
            // Check if parsed value is negative
            if (doubleValue < 0) {
                myInvalidValue = "'" + toString(myShapeAttr) + "' cannot be negative";
            }
        } else {
            myInvalidValue = "'" + toString(myShapeAttr) + "' doesn't have a valid 'time' format";
        }
    } else if (GNEAttributeCarrier::isFloat(myShapeTag, myShapeAttr)) {
        if (GNEAttributeCarrier::canParse<double>(myTextFieldReal->getText().text())) {
            // convert string to double
            double doubleValue = GNEAttributeCarrier::parse<double>(myTextFieldReal->getText().text());
            // Check if double value must be positive
            if (GNEAttributeCarrier::isPositive(myShapeTag, myShapeAttr) && (doubleValue < 0)) {
                myInvalidValue = "'" + toString(myShapeAttr) + "' cannot be negative";
                // check if double value is a probability
            } else if (GNEAttributeCarrier::isProbability(myShapeTag, myShapeAttr) && ((doubleValue < 0) || doubleValue > 1)) {
                myInvalidValue = "'" + toString(myShapeAttr) + "' takes only values between 0 and 1";
            }
        } else {
            myInvalidValue = "'" + toString(myShapeAttr) + "' doesn't have a valid 'float' format";
        }
    } else if (GNEAttributeCarrier::isColor(myShapeTag, myShapeAttr)) {
        // check if filename format is valid
        if (GNEAttributeCarrier::canParse<RGBColor>(myTextFieldStrings->getText().text()) == false) {
            myInvalidValue = "'" + toString(myShapeAttr) + "' doesn't have a valid 'RBGColor' format";
        }
    } else if (GNEAttributeCarrier::isFilename(myShapeTag, myShapeAttr)) {
        std::string file = myTextFieldStrings->getText().text();
        // check if filename format is valid
        if (GNEAttributeCarrier::isValidFilename(file) == false) {
            myInvalidValue = "input contains invalid characters for a filename";
        } else if (myShapeAttr == SUMO_ATTR_IMGFILE) {
            if (!file.empty()) {
                // only load value if file isn't empty
                if (GUITexturesHelper::getTextureID(file) == -1) {
                    myInvalidValue = "doesn't exist image '" + file + "'";
                }
            }
        }
    }
    // change color of text field depending of myCurrentValueValid
    if (myInvalidValue.size() == 0) {
        myTextFieldInt->setTextColor(FXRGB(0, 0, 0));
        myTextFieldInt->killFocus();
        myTextFieldReal->setTextColor(FXRGB(0, 0, 0));
        myTextFieldReal->killFocus();
        myTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
        myTextFieldStrings->killFocus();
    } else {
        // IF value of TextField isn't valid, change their color to Red
        myTextFieldInt->setTextColor(FXRGB(255, 0, 0));
        myTextFieldReal->setTextColor(FXRGB(255, 0, 0));
        myTextFieldStrings->setTextColor(FXRGB(255, 0, 0));
    }
    // Update aditional frame
    update();
    return 1;
}


long
GNEPolygonFrame::ShapeAttributeSingle::onCmdSetBooleanAttribute(FXObject*, FXSelector, void*) {
    if (myBoolCheckButton->getCheck()) {
        myBoolCheckButton->setText("true");
    } else {
        myBoolCheckButton->setText("false");
    }
    return 0;
}


long GNEPolygonFrame::ShapeAttributeSingle::onCmdSetColorAttribute(FXObject*, FXSelector, void*) {
    // create FXColorDialog
    FXColorDialog colordialog(this, tr("Color Dialog"));
    colordialog.setTarget(this);
    colordialog.setRGBA(MFXUtils::getFXColor(RGBColor::parseColor(myTextFieldStrings->getText().text())));
    // execute dialog to get a new color
    if (colordialog.execute()) {
        myTextFieldStrings->setText(toString(MFXUtils::getRGBColor(colordialog.getRGBA())).c_str());
    }
    return 0;
}

// ---------------------------------------------------------------------------
// GNEPolygonFrame::NeteditAttributes- methods
// ---------------------------------------------------------------------------

GNEPolygonFrame::ShapeAttributes::ShapeAttributes(GNEViewNet* viewNet, FXComposite* parent) :
    FXGroupBox(parent, "Internal attributes", GUIDesignGroupBoxFrame),
    myViewNet(viewNet),
    myIndexParameter(0),
    myIndexParameterList(0),
    myMaxNumberOfParameters(GNEAttributeCarrier::getHigherNumberOfAttributes()) {

    // Create single parameters
    for (int i = 0; i < myMaxNumberOfParameters; i++) {
        myVectorOfsingleShapeParameter.push_back(new ShapeAttributeSingle(this));
    }

    // Create help button
    new FXButton(this, "Help", 0, this, MID_HELP, GUIDesignButtonRectangular);
}


GNEPolygonFrame::ShapeAttributes::~ShapeAttributes() {
}


void
GNEPolygonFrame::ShapeAttributes::clearAttributes() {
    // Hidde al fields
    for (int i = 0; i < myMaxNumberOfParameters; i++) {
        myVectorOfsingleShapeParameter.at(i)->hideParameter();
    }

    // Reset indexs
    myIndexParameterList = 0;
    myIndexParameter = 0;
}


void
GNEPolygonFrame::ShapeAttributes::addAttribute(SumoXMLTag shapeTag, SumoXMLAttr ShapeAttributeSingle) {
    // Set current shape
    myShapeTag = shapeTag;

    if (myIndexParameter < myMaxNumberOfParameters) {
        // Check type of attribute list
        if (GNEAttributeCarrier::isInt(myShapeTag, ShapeAttributeSingle)) {
            myVectorOfsingleShapeParameter.at(myIndexParameter)->showParameter(myShapeTag, ShapeAttributeSingle, GNEAttributeCarrier::getDefaultValue<int>(myShapeTag, ShapeAttributeSingle));
        } else if (GNEAttributeCarrier::isFloat(myShapeTag, ShapeAttributeSingle) || GNEAttributeCarrier::isTime(myShapeTag, ShapeAttributeSingle)) {
            myVectorOfsingleShapeParameter.at(myIndexParameter)->showParameter(myShapeTag, ShapeAttributeSingle, GNEAttributeCarrier::getDefaultValue<double>(myShapeTag, ShapeAttributeSingle));
        } else if (GNEAttributeCarrier::isBool(myShapeTag, ShapeAttributeSingle)) {
            myVectorOfsingleShapeParameter.at(myIndexParameter)->showParameter(myShapeTag, ShapeAttributeSingle, GNEAttributeCarrier::getDefaultValue<bool>(myShapeTag, ShapeAttributeSingle));
        } else if (GNEAttributeCarrier::isColor(myShapeTag, ShapeAttributeSingle)) {
            myVectorOfsingleShapeParameter.at(myIndexParameter)->showParameter(myShapeTag, ShapeAttributeSingle, GNEAttributeCarrier::getDefaultValue<RGBColor>(myShapeTag, ShapeAttributeSingle));
        } else if (GNEAttributeCarrier::isString(myShapeTag, ShapeAttributeSingle)) {
            myVectorOfsingleShapeParameter.at(myIndexParameter)->showParameter(myShapeTag, ShapeAttributeSingle, GNEAttributeCarrier::getDefaultValue<std::string>(myShapeTag, ShapeAttributeSingle));
        } else {
            WRITE_WARNING("Attribute '" + toString(ShapeAttributeSingle) + "' doesn't have a defined type. Check definition in GNEAttributeCarrier");
        }
        // Update index parameter
        myIndexParameter++;
    } else {
        WRITE_ERROR("Max number of attributes reached (" + toString(myMaxNumberOfParameters) + ").");
    }
}


void
GNEPolygonFrame::ShapeAttributes::showShapeParameters() {
    recalc();
    show();
}


void
GNEPolygonFrame::ShapeAttributes::hideShapeParameters() {
    hide();
}


std::map<SumoXMLAttr, std::string>
GNEPolygonFrame::ShapeAttributes::getAttributesAndValues() const {
    std::map<SumoXMLAttr, std::string> values;
    // get standar Parameters
    for (int i = 0; i < myIndexParameter; i++) {
        values[myVectorOfsingleShapeParameter.at(i)->getAttr()] = myVectorOfsingleShapeParameter.at(i)->getValue();
    }
    return values;
}


void
GNEPolygonFrame::ShapeAttributes::showWarningMessage(std::string extra) const {
    std::string errorMessage;
    // iterate over standar parameters
    for (int i = 0; (i < myIndexParameter) && errorMessage.empty(); i++) {
        // Return string with the error if at least one of the parameter isn't valid
        std::string attributeValue = myVectorOfsingleShapeParameter.at(i)->isAttributeValid();
        if (attributeValue.size() != 0) {
            errorMessage = attributeValue;
        }
    }
    // show warning box if input parameters aren't invalid
    if (extra.size() == 0) {
        errorMessage = "Invalid input parameter of " + toString(myShapeTag) + ": " + errorMessage;
    } else {
        errorMessage = "Invalid input parameter of " + toString(myShapeTag) + ": " + extra;
    }

    // set message in status bar
    myViewNet->setStatusBarText(errorMessage);
    // Write Warning in console if we're in testing mode
    if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
        WRITE_WARNING(errorMessage);
    }
}


bool
GNEPolygonFrame::ShapeAttributes::areValuesValid() const {
    // iterate over standar parameters
    for (int i = 0; i < myIndexParameter; i++) {
        // Return false if error message of attriuve isn't empty
        if (myVectorOfsingleShapeParameter.at(i)->isAttributeValid().size() != 0) {
            return false;
        }
    }
    return true;
}


int
GNEPolygonFrame::ShapeAttributes::getNumberOfAddedAttributes() const {
    return (myIndexParameter + myIndexParameterList);
}


long
GNEPolygonFrame::ShapeAttributes::onCmdHelp(FXObject*, FXSelector, void*) {
    // Create help dialog
    FXDialogBox* helpDialog = new FXDialogBox(this, ("Parameters of " + toString(myShapeTag)).c_str(), GUIDesignDialogBox);
    // Create FXTable
    FXTable* myTable = new FXTable(helpDialog, this, MID_TABLE, TABLE_READONLY);
    myTable->setVisibleRows((FXint)(myIndexParameter + myIndexParameterList));
    myTable->setVisibleColumns(3);
    myTable->setTableSize((FXint)(myIndexParameter + myIndexParameterList), 3);
    myTable->setBackColor(FXRGB(255, 255, 255));
    myTable->setColumnText(0, "Name");
    myTable->setColumnText(1, "Value");
    myTable->setColumnText(2, "Definition");
    myTable->getRowHeader()->setWidth(0);
    FXHeader* header = myTable->getColumnHeader();
    header->setItemJustify(0, JUSTIFY_CENTER_X);
    header->setItemSize(0, 120);
    header->setItemJustify(1, JUSTIFY_CENTER_X);
    header->setItemSize(1, 80);
    int maxSizeColumnDefinitions = 0;
    // Iterate over vector of shape parameters
    for (int i = 0; i < myIndexParameter; i++) {
        SumoXMLTag shapeTag = myVectorOfsingleShapeParameter.at(i)->getTag();
        SumoXMLAttr shapeAttr = myVectorOfsingleShapeParameter.at(i)->getAttr();
        // Set name of attribute
        myTable->setItem(i, 0, new FXTableItem(toString(shapeAttr).c_str()));
        // Set type
        FXTableItem* type = new FXTableItem("");
        if (GNEAttributeCarrier::isInt(shapeTag, shapeAttr)) {
            type->setText("int");
        } else if (GNEAttributeCarrier::isFloat(shapeTag, shapeAttr)) {
            type->setText("float");
        } else if (GNEAttributeCarrier::isTime(shapeTag, shapeAttr)) {
            type->setText("time");
        } else if (GNEAttributeCarrier::isBool(shapeTag, shapeAttr)) {
            type->setText("bool");
        } else if (GNEAttributeCarrier::isColor(shapeTag, shapeAttr)) {
            type->setText("color");
        } else if (GNEAttributeCarrier::isString(shapeTag, shapeAttr)) {
            type->setText("string");
        }
        type->setJustify(FXTableItem::CENTER_X);
        myTable->setItem(i, 1, type);
        // Set definition
        FXTableItem* definition = new FXTableItem(GNEAttributeCarrier::getDefinition(shapeTag, shapeAttr).c_str());
        definition->setJustify(FXTableItem::LEFT);
        myTable->setItem(i, 2, definition);
        if ((int)GNEAttributeCarrier::getDefinition(shapeTag, shapeAttr).size() > maxSizeColumnDefinitions) {
            maxSizeColumnDefinitions = int(GNEAttributeCarrier::getDefinition(shapeTag, shapeAttr).size());
        }
    }
    // Set size of column
    header->setItemJustify(2, JUSTIFY_CENTER_X);
    header->setItemSize(2, maxSizeColumnDefinitions * 6);
    // Button Close
    new FXButton(helpDialog, "OK\t\tclose", GUIIconSubSys::getIcon(ICON_ACCEPT), helpDialog, FXDialogBox::ID_ACCEPT, GUIDesignButtonOK);
    helpDialog->create();
    helpDialog->show();
    return 1;
}

/****************************************************************************/
