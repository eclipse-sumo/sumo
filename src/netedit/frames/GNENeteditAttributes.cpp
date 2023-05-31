/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
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
    myActualAdditionalReferencePoint(AdditionalReferencePoint::LEFT) {
    // Create FXListBox for the reference points and fill it
    myReferencePointMatchBox = new FXComboBox(getCollapsableFrame(), GUIDesignComboBoxNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignComboBox);
    myReferencePointMatchBox->appendItem(TL("reference left"));
    myReferencePointMatchBox->appendItem(TL("reference right"));
    myReferencePointMatchBox->appendItem(TL("reference center"));
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
    helpReferencePoint = new FXButton(getCollapsableFrame(), TL("Help"), 0, this, MID_HELP, GUIDesignButtonRectangular);
    // Set visible items
    myReferencePointMatchBox->setNumVisible((int)myReferencePointMatchBox->getNumItems());
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
            if (myActualAdditionalReferencePoint == AdditionalReferencePoint::INVALID) {
                std::string errorMessage = TL("Current selected reference point isn't valid");
                myFrameParent->getViewNet()->setStatusBarText(errorMessage);
                // Write Warning in console if we're in testing mode
                WRITE_DEBUG(errorMessage);
                return false;
            } else {
                // obtain length
                double length = GNEAttributeCarrier::parse<double>(myLengthTextField->getText().text());
                // set start and end position
                baseObject->addDoubleAttribute(SUMO_ATTR_STARTPOS, setStartPosition(mousePositionOverLane, length));
                baseObject->addDoubleAttribute(SUMO_ATTR_ENDPOS, setEndPosition(mousePositionOverLane, length));
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
        // Cast actual reference point type
        if (myReferencePointMatchBox->getText() == TL("reference left")) {
            myReferencePointMatchBox->setTextColor(FXRGB(0, 0, 0));
            myReferencePointMatchBox->killFocus();
            myActualAdditionalReferencePoint = AdditionalReferencePoint::LEFT;
            myLengthTextField->enable();
        } else if (myReferencePointMatchBox->getText() == TL("reference right")) {
            myReferencePointMatchBox->setTextColor(FXRGB(0, 0, 0));
            myReferencePointMatchBox->killFocus();
            myActualAdditionalReferencePoint = AdditionalReferencePoint::RIGHT;
            myLengthTextField->enable();
        } else if (myReferencePointMatchBox->getText() == TL("reference center")) {
            myLengthTextField->enable();
            myReferencePointMatchBox->setTextColor(FXRGB(0, 0, 0));
            myReferencePointMatchBox->killFocus();
            myActualAdditionalReferencePoint = AdditionalReferencePoint::CENTER;
            myLengthTextField->enable();
        } else {
            myReferencePointMatchBox->setTextColor(FXRGB(255, 0, 0));
            myActualAdditionalReferencePoint = AdditionalReferencePoint::INVALID;
            myLengthTextField->disable();
        }
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
            << TL("  - Reference Left will create it with startPos = 70 and endPos = 100.") << "\n"
            << TL("  - Reference Right will create it with startPos = 100 and endPos = 130.") << "\n"
            << TL("  - Reference Center will create it with startPos = 85 and endPos = 115.") << "\n"
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
    new FXButton(myHorizontalFrameOKButton, (TL("OK") + std::string("\t\t") + TL("close")).c_str(), GUIIconSubSys::getIcon(GUIIcon::ACCEPT), additionalNeteditAttributesHelpDialog, FXDialogBox::ID_ACCEPT, GUIDesignButtonOK);
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
GNENeteditAttributes::setStartPosition(double positionOfTheMouseOverLane, double lengthOfAdditional) const {
    switch (myActualAdditionalReferencePoint) {
        case AdditionalReferencePoint::LEFT:
            return positionOfTheMouseOverLane;
        case AdditionalReferencePoint::RIGHT:
            return positionOfTheMouseOverLane - lengthOfAdditional;
        case AdditionalReferencePoint::CENTER:
            return positionOfTheMouseOverLane - lengthOfAdditional / 2;
        default:
            throw InvalidArgument("Reference Point invalid");
    }
}


double
GNENeteditAttributes::setEndPosition(double positionOfTheMouseOverLane, double lengthOfAdditional)  const {
    switch (myActualAdditionalReferencePoint) {
        case AdditionalReferencePoint::LEFT:
            return positionOfTheMouseOverLane + lengthOfAdditional;
        case AdditionalReferencePoint::RIGHT:
            return positionOfTheMouseOverLane;
        case AdditionalReferencePoint::CENTER:
            return positionOfTheMouseOverLane + lengthOfAdditional / 2;
        default:
            throw InvalidArgument("Reference Point invalid");
    }
}

/****************************************************************************/
