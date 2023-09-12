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
/// @file    GNENeteditAttributes.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2022
///
// Modul for edit netedit attributes
/****************************************************************************/
#include <config.h>

#include <netedit/frames/GNEFrame.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/dialogs/GNESingleParametersDialog.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNENeteditAttributes.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNENeteditAttributes) NeteditAttributesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNENeteditAttributes::onCmdSetNeteditAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,               GNENeteditAttributes::onCmdHelp)
};

// Object implementation
FXIMPLEMENT(GNENeteditAttributes,  MFXGroupBoxModule,   NeteditAttributesMap,   ARRAYNUMBER(NeteditAttributesMap))


// ===========================================================================
// method definitions
// ===========================================================================

GNENeteditAttributes::GNENeteditAttributes(GNEFrame* frameParent) :
    MFXGroupBoxModule(frameParent, TL("Netedit attributes")),
    myFrameParent(frameParent),
    myCurrentLengthValid(true),
    myReferencePoint(ReferencePoint::LEFT) {
    // fill reference points
    myReferencePoints.push_back(std::make_pair(TL("Reference Left"), ReferencePoint::LEFT));
    myReferencePoints.push_back(std::make_pair(TL("Reference Right"), ReferencePoint::RIGHT));
    myReferencePoints.push_back(std::make_pair(TL("Center"), ReferencePoint::CENTER));
    myReferencePoints.push_back(std::make_pair(TL("Extended Left"), ReferencePoint::EXTENDEDLEFT));
    myReferencePoints.push_back(std::make_pair(TL("Extended Right"), ReferencePoint::EXTENDEDRIGHT));
    myReferencePoints.push_back(std::make_pair(TL("Extended"), ReferencePoint::EXTENDED));
    // Create FXListBox for the reference points and fill it
    myReferencePointMatchBox = new MFXComboBoxIcon(getCollapsableFrame(), GUIDesignComboBoxNCol, false, GUIDesignComboBoxSizeMedium,
                                                   this, MID_GNE_SET_ATTRIBUTE, GUIDesignComboBox);
    for (const auto &referencePoint : myReferencePoints) {
        myReferencePointMatchBox->appendIconItem(referencePoint.first.c_str());
    }
    myReferencePointMatchBox->setCurrentItem(0);
    // Create Frame for Length Label and textField
    myLengthFrame = new FXHorizontalFrame(getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(myLengthFrame, toString(SUMO_ATTR_LENGTH).c_str(), 0, GUIDesignLabelThickedFixed(100));
    myLengthTextField = new FXTextField(myLengthFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myLengthTextField->setText("10");
    // Create Frame for block close polygon and checkBox (By default disabled)
    myCloseShapeFrame = new FXHorizontalFrame(getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(myCloseShapeFrame, TL("Close shape"), 0, GUIDesignLabelThickedFixed(100));
    myCloseShapeCheckButton = new FXCheckButton(myCloseShapeFrame, "false", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    // Create Frame for center element after creation (By default enabled)
    myCenterViewAfterCreationFrame = new FXHorizontalFrame(getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(myCenterViewAfterCreationFrame, TL("Center view"), 0, GUIDesignLabelThickedFixed(100));
    myCenterViewAfterCreationButton = new FXCheckButton(myCenterViewAfterCreationFrame, "false", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    myCenterViewAfterCreationButton->setCheck(true);
    // Create help button
    GUIDesigns::buildFXButton(getCollapsableFrame(), TL("Help"), "", "", 0, this, MID_HELP, GUIDesignButtonRectangular);
}


GNENeteditAttributes::~GNENeteditAttributes() {}


void
GNENeteditAttributes::showNeteditAttributesModule(GNEAttributeCarrier* templateAC) {
    // we assume that frame will not be show
    bool showFrame = false;
    // check if length text field has to be showed
    if (templateAC->getTagProperty().canMaskStartEndPos()) {
        myLengthFrame->show();
        myReferencePointMatchBox->show();
        showFrame = true;
    } else {
        myLengthFrame->hide();
        myReferencePointMatchBox->hide();
    }
    // check if close shape check button has to be show
    if (templateAC->getTagProperty().canCloseShape()) {
        myCloseShapeFrame->show();
        showFrame = true;
    } else {
        myCloseShapeFrame->hide();
    }
    // check if center camera after creation check button has to be show
    if (templateAC->getTagProperty().canCenterCameraAfterCreation()) {
        myCenterViewAfterCreationFrame->show();
        showFrame = true;
    } else {
        myCenterViewAfterCreationFrame->hide();
    }
    // if at least one element is show, show modul
    if (showFrame) {
        recalc();
        show();
    } else {
        hide();
    }
}


void
GNENeteditAttributes::hideNeteditAttributesModule() {
    hide();
}


bool
GNENeteditAttributes::getNeteditAttributesAndValues(CommonXMLStructure::SumoBaseObject* baseObject, const GNELane* lane) const {
    // check if we need to obtain a start and end position over an edge
    if (myReferencePointMatchBox->shown()) {
        // we need a valid lane to calculate position over lane
        if (lane == nullptr) {
            return false;
        } else if (myCurrentLengthValid) {
            // Obtain position of the mouse over lane (limited over grid)
            double mousePositionOverLane = lane->getLaneShape().nearest_offset_to_point2D(myFrameParent->getViewNet()->snapToActiveGrid(myFrameParent->getViewNet()->getPositionInformation())) / lane->getLengthGeometryFactor();
            // check if current reference point is valid
            if (myReferencePoint == ReferencePoint::INVALID) {
                std::string errorMessage = TL("Current selected reference point isn't valid");
                myFrameParent->getViewNet()->setStatusBarText(errorMessage);
                // Write Warning in console if we're in testing mode
                WRITE_DEBUG(errorMessage);
                return false;
            } else {
                // obtain length
                double elementLength = GNEAttributeCarrier::parse<double>(myLengthTextField->getText().text());
                // set start and end position
                baseObject->addDoubleAttribute(SUMO_ATTR_STARTPOS, setStartPosition(mousePositionOverLane, elementLength));
                baseObject->addDoubleAttribute(SUMO_ATTR_ENDPOS, setEndPosition(mousePositionOverLane, elementLength, lane->getLaneShape().length2D()));
            }
        } else {
            return false;
        }
    }
    // Save close shape value if shape's element can be closed
    if (myCloseShapeCheckButton->shown()) {
        baseObject->addBoolAttribute(GNE_ATTR_CLOSE_SHAPE, myCloseShapeCheckButton->getCheck() == 1);
    }
    // check center element after creation
    if (myCenterViewAfterCreationButton->shown()) {
        baseObject->addBoolAttribute(GNE_ATTR_CENTER_AFTER_CREATION, myCenterViewAfterCreationButton->getCheck() == 1);
    }
    // all ok, then return true to continue creating element
    return true;
}


bool
GNENeteditAttributes::canDrawLaneReference() const {
    return shown() && myReferencePointMatchBox->shown() && (myReferencePoint != ReferencePoint::INVALID);
}


void
GNENeteditAttributes::drawLaneReference(const GNELane* lane) const {
    // check lane
    if (lane) {


    }
}


long
GNENeteditAttributes::onCmdSetNeteditAttribute(FXObject* obj, FXSelector, void*) {
    if (obj == myCloseShapeCheckButton) {
        if (myCloseShapeCheckButton->getCheck()) {
            myCloseShapeCheckButton->setText("true");
        } else {
            myCloseShapeCheckButton->setText("false");
        }
    } else if (obj == myCenterViewAfterCreationButton) {
        if (myCenterViewAfterCreationButton->getCheck()) {
            myCenterViewAfterCreationButton->setText("true");
        } else {
            myCenterViewAfterCreationButton->setText("false");
        }
    } else if (obj == myLengthTextField) {
        // change color of text field depending of the input length
        if (GNEAttributeCarrier::canParse<double>(myLengthTextField->getText().text()) &&
                GNEAttributeCarrier::parse<double>(myLengthTextField->getText().text()) > 0) {
            myLengthTextField->setTextColor(FXRGB(0, 0, 0));
            myLengthTextField->killFocus();
            myCurrentLengthValid = true;
        } else {
            myLengthTextField->setTextColor(FXRGB(255, 0, 0));
            myCurrentLengthValid = false;
        }
        // Update additional frame
        update();
    } else if (obj == myReferencePointMatchBox) {
        // iterate over all reference points
        for (const auto &referencePoint : myReferencePoints) {
            if (myReferencePointMatchBox->getText().text() == referencePoint.first) {
                myReferencePointMatchBox->setTextColor(FXRGB(0, 0, 0));
                myReferencePointMatchBox->killFocus();
                myReferencePoint = referencePoint.second;
                myLengthTextField->enable();
                return 1;
            }
        }
        // invalid reference point
        myReferencePointMatchBox->setTextColor(FXRGB(255, 0, 0));
        myReferencePoint = ReferencePoint::INVALID;
        myLengthTextField->disable();
    }
    return 1;
}


long
GNENeteditAttributes::onCmdHelp(FXObject*, FXSelector, void*) {
    // Create dialog box
    FXDialogBox* additionalNeteditAttributesHelpDialog = new FXDialogBox(getCollapsableFrame(), "Netedit Parameters Help", GUIDesignDialogBox);
    additionalNeteditAttributesHelpDialog->setIcon(GUIIconSubSys::getIcon(GUIIcon::MODEADDITIONAL));
    // set help text
    std::ostringstream help;
    help
            << TL("- Reference point: Mark the initial position of the additional element.") << "\n"
            << TL("  Example: If you want to create a busStop with a length of 30 in the point 100 of the lane:") << "\n"
            // references
            << TL("  - Reference Left: will have startPos = 70 and endPos = 100.") << "\n"
            << TL("  - Reference Right: will have startPos = 100 and endPos = 130.") << "\n"
            << TL("  - Center: will have startPos = 85 and endPos = 115.") << "\n"
            // extended
            << TL("  - Extended Left: will have startPos = 85 and endPos until lane's length.") << "\n"
            << TL("  - Extended Right: will have startPos from start and endPos = 115.") << "\n"
            << TL("  - Extended: will have startPos from start and endPos until lane's length.") << "\n"
            // other options
            << TL("- Block movement: if is enabled, the created additional element will be blocked. i.e. cannot be moved with") << "\n"
            << TL("  the mouse. This option can be modified inspecting element.") << "\n"
            << TL("- Center view: if is enabled, view will be center over created element.");
    // Create label with the help text
    new FXLabel(additionalNeteditAttributesHelpDialog, help.str().c_str(), 0, GUIDesignLabelFrameInformation);
    // Create horizontal separator
    new FXHorizontalSeparator(additionalNeteditAttributesHelpDialog, GUIDesignHorizontalSeparator);
    // Create frame for OK Button
    FXHorizontalFrame* myHorizontalFrameOKButton = new FXHorizontalFrame(additionalNeteditAttributesHelpDialog, GUIDesignAuxiliarHorizontalFrame);
    // Create Button Close (And two more horizontal frames to center it)
    new FXHorizontalFrame(myHorizontalFrameOKButton, GUIDesignAuxiliarHorizontalFrame);
    GUIDesigns::buildFXButton(myHorizontalFrameOKButton, TL("OK"), "", TL("close"), GUIIconSubSys::getIcon(GUIIcon::ACCEPT), additionalNeteditAttributesHelpDialog, FXDialogBox::ID_ACCEPT, GUIDesignButtonOK);
    new FXHorizontalFrame(myHorizontalFrameOKButton, GUIDesignAuxiliarHorizontalFrame);
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Opening GNENeteditAttributes help dialog");
    // create Dialog
    additionalNeteditAttributesHelpDialog->create();
    // show in the given position
    additionalNeteditAttributesHelpDialog->show(PLACEMENT_CURSOR);
    // refresh APP
    getApp()->refresh();
    // open as modal dialog (will block all windows until stop() or stopModal() is called)
    getApp()->runModalFor(additionalNeteditAttributesHelpDialog);
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Closing GNENeteditAttributes help dialog");
    return 1;
}


double
GNENeteditAttributes::setStartPosition(const double mouseOverLanePos, double elementLenght) const {
    switch (myReferencePoint) {
        case ReferencePoint::LEFT:
            return mouseOverLanePos;
        case ReferencePoint::RIGHT:
            if ((mouseOverLanePos - elementLenght) <= 0) {
                return INVALID_DOUBLE;
            } else {
                return mouseOverLanePos - elementLenght;
            }
        case ReferencePoint::CENTER:
            if ((mouseOverLanePos - (elementLenght * 0.5)) <= 0) {
                return INVALID_DOUBLE;
            } else {
                return mouseOverLanePos - (elementLenght * 0.5);
            }
        case ReferencePoint::EXTENDEDLEFT:
            return INVALID_DOUBLE;
        case ReferencePoint::EXTENDEDRIGHT:
            return mouseOverLanePos;
        case ReferencePoint::EXTENDED:
            return INVALID_DOUBLE;
        default:
            throw InvalidArgument("Reference Point invalid");
    }
}


double
GNENeteditAttributes::setEndPosition(const double mouseOverLanePos, double elementLenght, const double laneLength) const {
    switch (myReferencePoint) {
        case ReferencePoint::LEFT:
            if ((mouseOverLanePos + elementLenght) >= laneLength) {
                return INVALID_DOUBLE;
            } else {
                return mouseOverLanePos + elementLenght;
            }
        case ReferencePoint::RIGHT:
            return mouseOverLanePos;
        case ReferencePoint::CENTER:
            if ((mouseOverLanePos + (elementLenght * 0.5)) >= laneLength) {
                return INVALID_DOUBLE;
            } else {
                return mouseOverLanePos + (elementLenght * 0.5);
            }
        case ReferencePoint::EXTENDEDLEFT:
            return mouseOverLanePos;
        case ReferencePoint::EXTENDEDRIGHT:
            return INVALID_DOUBLE;
        case ReferencePoint::EXTENDED:
            return INVALID_DOUBLE;
        default:
            throw InvalidArgument("Reference Point invalid");
    }
}

/****************************************************************************/
