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
/// @file    GNEShapeFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2017
///
// The Widget for add polygons
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/elements/additional/GNEAdditionalHandler.h>
#include <netedit/frames/GNEAttributesEditor.h>
#include <netedit/frames/GNEDrawingShape.h>
#include <netedit/frames/GNETagSelector.h>
#include <utils/foxtools/MFXDynamicLabel.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/div/GUIUserIO.h>

#include "GNEShapeFrame.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEShapeFrame::GEOPOICreator) GEOPOICreatorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,      GNEShapeFrame::GEOPOICreator::onCmdSetCoordinates),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_OPERATION,      GNEShapeFrame::GEOPOICreator::onCmdSetFormat),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CREATE,             GNEShapeFrame::GEOPOICreator::onCmdCreateGEOPOI),
};

// Object implementation
FXIMPLEMENT(GNEShapeFrame::GEOPOICreator,     GNEGroupBoxModule,     GEOPOICreatorMap,   ARRAYNUMBER(GEOPOICreatorMap))


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEShapeFrame::GEOPOICreator - methods
// ---------------------------------------------------------------------------

GNEShapeFrame::GEOPOICreator::GEOPOICreator(GNEShapeFrame* polygonFrameParent) :
    GNEGroupBoxModule(polygonFrameParent, TL("GEO POI Creator")),
    myShapeFrameParent(polygonFrameParent) {
    // create RadioButtons for formats
    myLonLatRadioButton = new FXRadioButton(getCollapsableFrame(), TL("Format: Lon-Lat"), this, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    myLatLonRadioButton = new FXRadioButton(getCollapsableFrame(), TL("Format: Lat-Lon"), this, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    // set lat-lon as default
    myLatLonRadioButton->setCheck(TRUE);
    // create text field for coordinates
    myCoordinatesTextField = new FXTextField(getCollapsableFrame(), GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // create checkBox
    myCenterViewAfterCreationCheckButton = new FXCheckButton(getCollapsableFrame(), TL("Center View after creation"), this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    // create button for create GEO POIs
    myCreateGEOPOIButton = GUIDesigns::buildFXButton(getCollapsableFrame(), TL("Create GEO POI (clipboard)"), "", "", nullptr, this, MID_GNE_CREATE, GUIDesignButton);
    // create information label
    myLabelCartesianPosition = new MFXDynamicLabel(getCollapsableFrame(),
            (TL("Cartesian equivalence:") + std::string("\n") +
             TL("- X = give valid longitude") + std::string("\n") +
             TL("- Y = give valid latitude")).c_str(),
            0, GUIDesignLabelFrameInformation);
}


GNEShapeFrame::GEOPOICreator::~GEOPOICreator() {}


void
GNEShapeFrame::GEOPOICreator::showGEOPOICreatorModule() {
    // check if there is an GEO Proj string is defined
    if (GeoConvHelper::getFinal().getProjString() != "!") {
        myCoordinatesTextField->enable();
        myCoordinatesTextField->setText("");
        myCoordinatesTextField->enable();
        myCreateGEOPOIButton->enable();
    } else  {
        myCoordinatesTextField->setText(TL("No geo-conversion defined"));
        myCoordinatesTextField->disable();
        myCreateGEOPOIButton->disable();
    }
    show();
}


void
GNEShapeFrame::GEOPOICreator::hideGEOPOICreatorModule() {
    hide();
}


long
GNEShapeFrame::GEOPOICreator::onCmdSetCoordinates(FXObject*, FXSelector, void*) {
    // check if input contains spaces
    std::string input = myCoordinatesTextField->getText().text();
    std::string inputWithoutSpaces;
    for (const auto& i : input) {
        if (i != ' ') {
            inputWithoutSpaces.push_back(i);
        }
    }
    // if input contains spaces, call this function again, and in other case set red text color
    if (input.size() != inputWithoutSpaces.size()) {
        myCoordinatesTextField->setText(inputWithoutSpaces.c_str());
    }
    if (inputWithoutSpaces.size() > 0) {
        myCreateGEOPOIButton->setText(TL("Create GEO POI"));
    } else {
        myCreateGEOPOIButton->setText(TL("Create GEO POI (clipboard)"));
    }
    // simply check if given value can be parsed to Position
    if (GNEAttributeCarrier::canParse<Position>(myCoordinatesTextField->getText().text())) {
        myCoordinatesTextField->setTextColor(GUIDesignTextColorBlack);
        myCoordinatesTextField->killFocus();
        // convert coordinates into lon-lat
        Position geoPos = GNEAttributeCarrier::parse<Position>(myCoordinatesTextField->getText().text());
        if (myLatLonRadioButton->getCheck() == TRUE) {
            geoPos.swapXY();
        }
        GeoConvHelper::getFinal().x2cartesian_const(geoPos);
        // check if GEO Position has to be swapped
        // update myLabelCartesianPosition
        myLabelCartesianPosition->setText(
            (TL("Cartesian equivalence:") + std::string("\n- X = ") + toString(geoPos.x()) + std::string("\n- Y = ") + toString(geoPos.y())).c_str());
    } else {
        myCoordinatesTextField->setTextColor(GUIDesignTextColorRed);
        myLabelCartesianPosition->setText(
            (TL("Cartesian equivalence:") + std::string("\n") +
             TL("- X = give valid longitude") + std::string("\n") +
             TL("- Y = give valid latitude")).c_str());
    };
    return 1;
}


long
GNEShapeFrame::GEOPOICreator::onCmdSetFormat(FXObject* obj, FXSelector, void*) {
    //disable other radio button depending of selected option
    if (obj == myLonLatRadioButton) {
        myLonLatRadioButton->setCheck(TRUE);
        myLatLonRadioButton->setCheck(FALSE);
    } else if (obj == myLatLonRadioButton) {
        myLonLatRadioButton->setCheck(FALSE);
        myLatLonRadioButton->setCheck(TRUE);
    }
    // in both cases call onCmdSetCoordinates(0,0,0) to set new cartesian equivalence
    onCmdSetCoordinates(0, 0, 0);
    return 1;
}


long
GNEShapeFrame::GEOPOICreator::onCmdCreateGEOPOI(FXObject*, FXSelector, void*) {
    // first check if current GEO Position is valid
    if (myShapeFrameParent->myShapeAttributesEditor->checkAttributes(true)) {
        std::string geoPosStr = myCoordinatesTextField->getText().text();
        if (geoPosStr.empty()) {
            // use clipboard
            WRITE_WARNING(TL("Using clipboard"));
            geoPosStr = GUIUserIO::copyFromClipboard(*getApp());
            myCoordinatesTextField->setText(geoPosStr.c_str());
            // remove spaces, update cartesian value
            onCmdSetCoordinates(0, 0, 0);
            geoPosStr = myCoordinatesTextField->getText().text();
            myCoordinatesTextField->setText("");
            myCreateGEOPOIButton->setText(TL("Create GEO POI (clipboard)"));
        }
        if (GNEAttributeCarrier::canParse<Position>(geoPosStr)) {
            // create baseShape object
            myShapeFrameParent->createBaseShapeObject(SUMO_TAG_POI);
            // obtain shape attributes and values
            myShapeFrameParent->myShapeAttributesEditor->fillSumoBaseObject(myShapeFrameParent->myBaseShape);
            // force GEO attribute to true and obtain position
            myShapeFrameParent->myBaseShape->addBoolAttribute(SUMO_ATTR_GEO, true);
            Position geoPos = GNEAttributeCarrier::parse<Position>(geoPosStr);
            // convert coordinates into lon-lat
            if (myLatLonRadioButton->getCheck() == TRUE) {
                geoPos.swapXY();
            }
            // add lon/lat
            myShapeFrameParent->myBaseShape->addDoubleAttribute(SUMO_ATTR_LON, geoPos.x());
            myShapeFrameParent->myBaseShape->addDoubleAttribute(SUMO_ATTR_LAT, geoPos.y());
            // set GEO Position as true
            myShapeFrameParent->myBaseShape->addBoolAttribute(SUMO_ATTR_GEO, true);
            // declare additional handler
            GNEAdditionalHandler additionalHandler(myShapeFrameParent->myViewNet->getNet(),
                                                   myShapeFrameParent->myViewNet->getNet()->getACTemplates()->getTemplateAC(GNE_TAG_POIGEO)->getFileBucket(),
                                                   myShapeFrameParent->myViewNet->getViewParent()->getGNEAppWindows()->isUndoRedoAllowed());
            // build shape
            additionalHandler.parseSumoBaseObject(myShapeFrameParent->myBaseShape);
            // check if view has to be centered over created GEO POI
            if (myCenterViewAfterCreationCheckButton->getCheck() == TRUE) {
                // create a boundary over given GEO Position and center view over it
                Boundary centerPosition;
                GeoConvHelper::getFinal().x2cartesian_const(geoPos);
                centerPosition.add(geoPos);
                centerPosition = centerPosition.grow(10);
                myShapeFrameParent->myViewNet->getViewParent()->getView()->centerTo(centerPosition);
            }
        }
        // refresh shape attributes
        myShapeFrameParent->myShapeAttributesEditor->refreshAttributesEditor();
    }
    return 1;
}


// ---------------------------------------------------------------------------
// GNEShapeFrame - methods
// ---------------------------------------------------------------------------

GNEShapeFrame::GNEShapeFrame(GNEViewParent* viewParent, GNEViewNet* viewNet) :
    GNEFrame(viewParent, viewNet, TL("Shapes")),
    myBaseShape(nullptr) {

    // create item Selector module for shapes
    myShapeTagSelector = new GNETagSelector(this, GNETagProperties::Type::SHAPE, SUMO_TAG_POLY);

    // Create shape parameters
    myShapeAttributesEditor = new GNEAttributesEditor(this, GNEAttributesEditorType::EditorType::CREATOR);

    // Create drawing controls
    myDrawingShape = new GNEDrawingShape(this);

    /// @brief create GEOPOICreator
    myGEOPOICreator = new GEOPOICreator(this);
}


GNEShapeFrame::~GNEShapeFrame() {
    // check if we have to delete base additional object
    if (myBaseShape) {
        delete myBaseShape;
    }
}


void
GNEShapeFrame::show() {
    // refresh tag selector
    myShapeTagSelector->refreshTagSelector();
    // show frame
    GNEFrame::show();
}


bool
GNEShapeFrame::processClick(const Position& clickedPosition, const GNEViewNetHelper::ViewObjectsSelector& viewObjects, bool& updateTemporalShape) {
    // reset updateTemporalShape
    updateTemporalShape = false;
    // check if current selected shape is valid
    if (myShapeTagSelector->getCurrentTemplateAC() != nullptr) {
        // get tag
        SumoXMLTag shapeTag = myShapeTagSelector->getCurrentTemplateAC()->getTagProperty()->getTag();
        // continue depending of tag
        switch (shapeTag) {
            case SUMO_TAG_POI:
                return processClickPOI(shapeTag, clickedPosition);
            case GNE_TAG_POIGEO:
                return processClickPOIGeo(clickedPosition);
            case GNE_TAG_POILANE:
                return processClickPOILanes(viewObjects);
            case SUMO_TAG_POLY:
            case GNE_TAG_JPS_WALKABLEAREA:
            case GNE_TAG_JPS_OBSTACLE:
                return processClickPolygons(clickedPosition, updateTemporalShape);
            default:
                break;
        }
    }
    myViewNet->setStatusBarText(TL("Current selected shape isn't valid."));
    return false;
}


std::string
GNEShapeFrame::getIdsSelected(const FXList* list) {
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


GNEDrawingShape*
GNEShapeFrame::getDrawingShapeModule() const {
    return myDrawingShape;
}


void
GNEShapeFrame::createBaseShapeObject(const SumoXMLTag shapeTag) {
    // check if baseShape exist, and if yes, delete it
    if (myBaseShape) {
        // delete baseShape (and all children)
        delete myBaseShape;
    }
    // just create a base shape
    myBaseShape = new CommonXMLStructure::SumoBaseObject(nullptr);
    // set tag
    myBaseShape->setTag(shapeTag);
}


bool
GNEShapeFrame::shapeDrawed() {
    // show warning dialogbox and stop check if input parameters are valid
    if (!myShapeAttributesEditor->checkAttributes(true)) {
        return false;
    } else if (myDrawingShape->getTemporalShape().size() == 0) {
        WRITE_WARNING(TL("Polygon shape cannot be empty"));
        return false;
    } else {
        // get tag
        SumoXMLTag shapeTag = myShapeTagSelector->getCurrentTemplateAC()->getTagProperty()->getTag();
        // create baseShape object
        createBaseShapeObject(shapeTag);
        // obtain shape attributes and values
        myShapeAttributesEditor->fillSumoBaseObject(myBaseShape);
        // obtain shape and check if has to be closed
        PositionVector temporalShape = myDrawingShape->getTemporalShape();
        if ((myBaseShape->hasBoolAttribute(GNE_ATTR_CLOSE_SHAPE) && myBaseShape->getBoolAttribute(GNE_ATTR_CLOSE_SHAPE)) ||
                (shapeTag == GNE_TAG_JPS_WALKABLEAREA) || (shapeTag == GNE_TAG_JPS_OBSTACLE)) {
            temporalShape.closePolygon();
        }
        myBaseShape->addPositionVectorAttribute(SUMO_ATTR_SHAPE, temporalShape);
        // declare additional handler
        GNEAdditionalHandler additionalHandler(myViewNet->getNet(), myViewNet->getNet()->getACTemplates()->getTemplateAC(shapeTag)->getFileBucket(),
                                               myViewNet->getViewParent()->getGNEAppWindows()->isUndoRedoAllowed());
        // build shape
        additionalHandler.parseSumoBaseObject(myBaseShape);
        // refresh shape attributes
        myShapeAttributesEditor->refreshAttributesEditor();
        // shape added, then return true;
        return true;
    }
}


void
GNEShapeFrame::tagSelected() {
    if (myShapeTagSelector->getCurrentTemplateAC()) {
        // show editors
        myShapeAttributesEditor->showAttributesEditor(myShapeTagSelector->getCurrentTemplateAC(), true);
        // get shape tag
        SumoXMLTag shapeTag = myShapeTagSelector->getCurrentTemplateAC()->getTagProperty()->getTag();
        // Check if drawing mode has to be shown
        if ((shapeTag == SUMO_TAG_POLY) || (shapeTag == GNE_TAG_JPS_WALKABLEAREA) || (shapeTag == GNE_TAG_JPS_OBSTACLE)) {
            myDrawingShape->showDrawingShape();
        } else {
            myDrawingShape->hideDrawingShape();
        }
        // Check if GEO POI Creator has to be shown
        if (shapeTag == GNE_TAG_POIGEO) {
            myGEOPOICreator->showGEOPOICreatorModule();
        } else {
            myGEOPOICreator->hideGEOPOICreatorModule();
        }
    } else {
        // hide all widgets
        myShapeAttributesEditor->hideAttributesEditor();
        myDrawingShape->hideDrawingShape();
        myGEOPOICreator->hideGEOPOICreatorModule();
    }
}


bool
GNEShapeFrame::processClickPolygons(const Position& clickedPosition, bool& updateTemporalShape) {
    if (myDrawingShape->isDrawing()) {
        // add or delete a new point depending of flag "delete last created point"
        if (myDrawingShape->getDeleteLastCreatedPoint()) {
            myDrawingShape->removeLastPoint();
        } else {
            myDrawingShape->addNewPoint(clickedPosition);
        }
        // set temporal shape
        updateTemporalShape = true;
        return true;
    } else {
        return false;
    }
}


bool
GNEShapeFrame::processClickPOI(SumoXMLTag POITag, const Position& clickedPosition) {
    // show warning dialogbox and stop if input parameters are invalid
    if (!myShapeAttributesEditor->checkAttributes(true)) {
        return false;
    }
    // create baseShape object
    createBaseShapeObject(POITag);
    // obtain shape attributes and values
    myShapeAttributesEditor->fillSumoBaseObject(myBaseShape);
    // add X-Y
    myBaseShape->addDoubleAttribute(SUMO_ATTR_X, clickedPosition.x());
    myBaseShape->addDoubleAttribute(SUMO_ATTR_Y, clickedPosition.y());
    // set GEO Position as false (because we have created POI clicking over View
    myBaseShape->addBoolAttribute(SUMO_ATTR_GEO, false);
    // declare additional handler
    GNEAdditionalHandler additionalHandler(myViewNet->getNet(), myViewNet->getNet()->getACTemplates()->getTemplateAC(POITag)->getFileBucket(),
                                           myViewNet->getViewParent()->getGNEAppWindows()->isUndoRedoAllowed());
    // build shape
    additionalHandler.parseSumoBaseObject(myBaseShape);
    // refresh shape attributes
    myShapeAttributesEditor->refreshAttributesEditor();
    // shape added, then return true
    return true;
}


bool
GNEShapeFrame::processClickPOIGeo(const Position& clickedPosition) {
    // show warning dialogbox and stop if input parameters are invalid
    if (!myShapeAttributesEditor->checkAttributes(true)) {
        return false;
    }
    // create baseShape object
    createBaseShapeObject(SUMO_TAG_POI);
    // obtain shape attributes and values
    myShapeAttributesEditor->fillSumoBaseObject(myBaseShape);
    // convert position to cartesian
    Position GEOPos = clickedPosition;
    GeoConvHelper::getFinal().cartesian2geo(GEOPos);
    // add X-Y in geo format
    myBaseShape->addDoubleAttribute(SUMO_ATTR_LON, GEOPos.x());
    myBaseShape->addDoubleAttribute(SUMO_ATTR_LAT, GEOPos.y());
    // set GEO Position as false (because we have created POI clicking over View
    myBaseShape->addBoolAttribute(SUMO_ATTR_GEO, true);
    // declare additional handler
    GNEAdditionalHandler additionalHandler(myViewNet->getNet(), myViewNet->getNet()->getACTemplates()->getTemplateAC(GNE_TAG_POIGEO)->getFileBucket(),
                                           myViewNet->getViewParent()->getGNEAppWindows()->isUndoRedoAllowed());
    // build shape
    additionalHandler.parseSumoBaseObject(myBaseShape);
    // refresh shape attributes
    myShapeAttributesEditor->refreshAttributesEditor();
    // shape added, then return true
    return true;
}


bool
GNEShapeFrame::processClickPOILanes(const GNEViewNetHelper::ViewObjectsSelector& viewObjects) {
    // abort if lane is nullptr
    if (viewObjects.getLaneFront() == nullptr) {
        WRITE_WARNING(TL("POILane can be only placed over lanes"));
        return false;
    }
    // show warning dialogbox and stop if input parameters are invalid
    if (!myShapeAttributesEditor->checkAttributes(true)) {
        return false;
    }
    // create baseShape object
    createBaseShapeObject(SUMO_TAG_POI);
    // obtain shape attributes and values
    myShapeAttributesEditor->fillSumoBaseObject(myBaseShape);
    // obtain Lane
    myBaseShape->addStringAttribute(SUMO_ATTR_LANE, viewObjects.getLaneFront()->getID());
    // obtain position over lane
    myBaseShape->addDoubleAttribute(SUMO_ATTR_POSITION, viewObjects.getLaneFront()->getLaneShape().nearest_offset_to_point2D(
                                        myViewNet->snapToActiveGrid(myViewNet->getPositionInformation())) /
                                    viewObjects.getLaneFront()->getLengthGeometryFactor());
    // declare additional handler
    GNEAdditionalHandler additionalHandler(myViewNet->getNet(), myViewNet->getNet()->getACTemplates()->getTemplateAC(GNE_TAG_POILANE)->getFileBucket(),
                                           myViewNet->getViewParent()->getGNEAppWindows()->isUndoRedoAllowed());
    // build shape
    additionalHandler.parseSumoBaseObject(myBaseShape);
    // refresh shape attributes
    myShapeAttributesEditor->refreshAttributesEditor();
    // shape added, then return true
    return true;
}

/****************************************************************************/
