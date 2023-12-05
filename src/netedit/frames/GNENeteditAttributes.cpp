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
#include <utils/gui/div/GLHelper.h>
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
    myReferencePoint(ReferencePoint::LEFT) {
    // fill reference points
    myReferencePoints.push_back(std::make_pair(TL("Reference Left"), ReferencePoint::LEFT));
    myReferencePoints.push_back(std::make_pair(TL("Reference Right"), ReferencePoint::RIGHT));
    myReferencePoints.push_back(std::make_pair(TL("Center"), ReferencePoint::CENTER));
    myReferencePoints.push_back(std::make_pair(TL("Extended Left"), ReferencePoint::EXTENDEDLEFT));
    myReferencePoints.push_back(std::make_pair(TL("Extended Right"), ReferencePoint::EXTENDEDRIGHT));
    myReferencePoints.push_back(std::make_pair(TL("Extended"), ReferencePoint::EXTENDED));
    // Create FXListBox for the reference points and fill it
    myReferencePointComboBox = new MFXComboBoxIcon(getCollapsableFrame(), GUIDesignComboBoxNCol, false, GUIDesignComboBoxVisibleItemsMedium,
            this, MID_GNE_SET_ATTRIBUTE, GUIDesignComboBox);
    for (const auto& referencePoint : myReferencePoints) {
        myReferencePointComboBox->appendIconItem(referencePoint.first.c_str());
    }
    myReferencePointComboBox->setCurrentItem(0);
    // Create Frame for Length Label and textField
    myLengthFrame = new FXHorizontalFrame(getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(myLengthFrame, toString(SUMO_ATTR_LENGTH).c_str(), 0, GUIDesignLabelThickedFixed(100));
    myLengthTextField = new FXTextField(myLengthFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldRestricted(TEXTFIELD_REAL));
    myLengthTextField->setText("10");
    // Create Frame for force length
    myForceLengthFrame = new FXHorizontalFrame(getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(myForceLengthFrame, TL("Force leng."), 0, GUIDesignLabelThickedFixed(100));
    myForceLengthCheckButton = new FXCheckButton(myForceLengthFrame, "false", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
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
        myForceLengthFrame->show();
        myReferencePointComboBox->show();
        showFrame = true;
    } else {
        myForceLengthFrame->hide();
        myLengthFrame->hide();
        myReferencePointComboBox->hide();
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
    if (myReferencePointComboBox->shown()) {
        // declare error message
        std::string errorMessage;
        // get element length
        const double elementLength = getElementLength();
        // we need a valid lane to calculate position over lane
        if (lane == nullptr) {
            // stop creating element, but without showing error message
            return false;
        } else if (myReferencePoint == ReferencePoint::INVALID) {
            // write warning and stop
            errorMessage = TL("Current selected reference point isn't valid");
            myFrameParent->getViewNet()->setStatusBarText(errorMessage);
            WRITE_WARNING(errorMessage);
            return false;
        } else if (elementLength == INVALID_DOUBLE) {
            // write warning and stop
            errorMessage = TL("Invalid length");
            myFrameParent->getViewNet()->setStatusBarText(errorMessage);
            WRITE_WARNING(errorMessage);
            return false;
        } else {
            // Obtain position of the mouse over lane (limited over grid)
            const double mousePosOverLane = lane->getLaneShape().nearest_offset_to_point2D(myFrameParent->getViewNet()->snapToActiveGrid(myFrameParent->getViewNet()->getPositionInformation())) / lane->getLengthGeometryFactor();
            // get start and end positions
            const double startPos = setStartPosition(mousePosOverLane, elementLength);
            const double endPos = setEndPosition(mousePosOverLane, elementLength, lane->getLaneShape().length2D());
            // check if force length
            if (myForceLengthFrame->shown() && (myForceLengthCheckButton->getCheck() == TRUE) && (endPos - startPos) != elementLength) {
                // write warning and stop
                errorMessage = TL("Invalid position. Uncheck 'Force length' to create element with flexible length");
                myFrameParent->getViewNet()->setStatusBarText(errorMessage);
                WRITE_WARNING(errorMessage);
                return false;
            }
            // set start and end position
            baseObject->addDoubleAttribute(SUMO_ATTR_STARTPOS, startPos);
            baseObject->addDoubleAttribute(SUMO_ATTR_ENDPOS, endPos);
        }
    }
    // add close shape value if shape's element can be closed
    if (myCloseShapeFrame->shown()) {
        baseObject->addBoolAttribute(GNE_ATTR_CLOSE_SHAPE, myCloseShapeCheckButton->getCheck() == TRUE);
    }
    // add center element after creation
    if (myCenterViewAfterCreationFrame->shown()) {
        baseObject->addBoolAttribute(GNE_ATTR_CENTER_AFTER_CREATION, myCenterViewAfterCreationButton->getCheck() == TRUE);
    }
    // all ok, then return true to continue creating element
    return true;
}


void
GNENeteditAttributes::drawLaneReference(const GNELane* lane) const {
    // get element length
    const double elementLength = getElementLength();
    // check lane
    if (lane && shown() && myReferencePointComboBox->shown() && (myReferencePoint != ReferencePoint::INVALID) &&
            (elementLength != INVALID_DOUBLE)) {
        // Obtain position of the mouse over lane (limited over grid)
        const double mousePosOverLane = lane->getLaneShape().nearest_offset_to_point2D(myFrameParent->getViewNet()->snapToActiveGrid(myFrameParent->getViewNet()->getPositionInformation())) / lane->getLengthGeometryFactor();
        // continue depending of mouse pos over lane
        if (mousePosOverLane >= 0) {
            // set start and end position
            const double startPos = setStartPosition(mousePosOverLane, elementLength);
            const double endPos = setEndPosition(mousePosOverLane, elementLength, lane->getLaneShape().length2D());
            // get lane geometry
            const auto laneShape = lane->getLaneGeometry().getShape();
            // difference between start-end position and elementLength
            const auto lengthDifference = (endPos - startPos) - elementLength;
            // set color
            RGBColor segmentColor;
            // check if force length
            if (myForceLengthFrame->shown() && (myForceLengthCheckButton->getCheck() == TRUE) && abs(lengthDifference) >= 0.1) {
                segmentColor = RGBColor::RED;
            } else {
                segmentColor = RGBColor::ORANGE;
            }
            // declare geometries
            GUIGeometry geometry;
            // trim geomtry
            geometry.updateGeometry(laneShape,
                                    (startPos == INVALID_DOUBLE) ? -1 : startPos,
                                    Position::INVALID,
                                    (endPos == INVALID_DOUBLE) ? -1 : endPos,
                                    Position::INVALID);
            // push layer matrix
            GLHelper::pushMatrix();
            // translate to temporal shape layer
            glTranslated(0, 0, GLO_TEMPORALSHAPE);
            // set color
            GLHelper::setColor(segmentColor);
            // draw temporal edge
            GUIGeometry::drawGeometry(lane->getDrawingConstants()->getDetail(), geometry, 0.45);
            // check if draw starPos
            if (startPos != INVALID_DOUBLE) {
                // cut start pos
                geometry.updateGeometry(laneShape, startPos, Position::INVALID, startPos + 0.5, Position::INVALID);
                // draw startPos
                GUIGeometry::drawGeometry(lane->getDrawingConstants()->getDetail(), geometry, 1);
            } else {
                // push circle matrix
                GLHelper::pushMatrix();
                // translate to test layer, but under magenta square
                glTranslated(laneShape.front().x(), laneShape.front().y(), 0);
                // draw circle
                GLHelper::drawFilledCircle(0.8, 8);
                // pop circle matrix
                GLHelper::popMatrix();
            }
            // check if draw endPos
            if (endPos != INVALID_DOUBLE) {
                // cut endPos
                geometry.updateGeometry(laneShape, endPos - 0.5, Position::INVALID, endPos, Position::INVALID);
                // draw endPos
                GUIGeometry::drawGeometry(lane->getDrawingConstants()->getDetail(), geometry, 1);
            } else {
                // push circle matrix
                GLHelper::pushMatrix();
                // translate to test layer, but under magenta square
                glTranslated(laneShape.back().x(), laneShape.back().y(), 0);
                // draw circle
                GLHelper::drawFilledCircle(0.8, 8);
                // pop circle matrix
                GLHelper::popMatrix();
            }
            // set color
            GLHelper::setColor(segmentColor.changedBrightness(-32));
            // translate to front
            glTranslated(0, 0, 2);
            // check if draw at end, or over circle
            if (endPos == INVALID_DOUBLE) {
                // cut endPos
                geometry.updateGeometry(laneShape, laneShape.length() - 0.5, Position::INVALID, laneShape.length(), Position::INVALID);
                // draw triangle at end
                GLHelper::drawTriangleAtEnd(geometry.getShape().front(), geometry.getShape().back(), (double) 0.45, (double) 0.3, 0.3);
            } else {
                // draw triangle at end
                GLHelper::drawTriangleAtEnd(geometry.getShape().front(), geometry.getShape().back(), (double) 0.45, (double) 0.3, -0.1);
            }
            // pop layer matrix
            GLHelper::popMatrix();
        }
    }
}


long
GNENeteditAttributes::onCmdSetNeteditAttribute(FXObject* obj, FXSelector, void*) {
    if (obj == myForceLengthCheckButton) {
        if (myForceLengthCheckButton->getCheck()) {
            myForceLengthCheckButton->setText(TL("true"));
        } else {
            myForceLengthCheckButton->setText(TL("false"));
        }
    } else if (obj == myCloseShapeCheckButton) {
        if (myCloseShapeCheckButton->getCheck()) {
            myCloseShapeCheckButton->setText(TL("true"));
        } else {
            myCloseShapeCheckButton->setText(TL("false"));
        }
    } else if (obj == myCenterViewAfterCreationButton) {
        if (myCenterViewAfterCreationButton->getCheck()) {
            myCenterViewAfterCreationButton->setText(TL("true"));
        } else {
            myCenterViewAfterCreationButton->setText(TL("false"));
        }
    } else if (obj == myLengthTextField) {
        // change color of text field depending of the input length
        if (getElementLength() != INVALID_DOUBLE) {
            myLengthTextField->setTextColor(FXRGB(0, 0, 0));
            myLengthTextField->killFocus();
        } else {
            myLengthTextField->setTextColor(FXRGB(255, 0, 0));
        }
        // set background color
        if (myLengthTextField->getText().empty()) {
            myLengthTextField->setBackColor(FXRGBA(255, 213, 213, 255));
        } else {
            myLengthTextField->setBackColor(FXRGBA(255, 255, 255, 255));
        }
        // Update additional frame
        update();
    } else if (obj == myReferencePointComboBox) {
        // iterate over all reference points
        for (const auto& referencePoint : myReferencePoints) {
            if (myReferencePointComboBox->getText().text() == referencePoint.first) {
                // update reference point
                myReferencePoint = referencePoint.second;
                // update comboBox
                myReferencePointComboBox->setTextColor(FXRGB(0, 0, 0));
                myReferencePointComboBox->killFocus();
                myReferencePointComboBox->setBackColor(FXRGBA(255, 255, 255, 255));
                // enable text fierld
                myLengthTextField->enable();
                // check if show force length
                if ((myReferencePoint == ReferencePoint::LEFT) ||
                        (myReferencePoint == ReferencePoint::RIGHT) ||
                        (myReferencePoint == ReferencePoint::CENTER)) {
                    myForceLengthFrame->show();
                } else {
                    myForceLengthFrame->hide();
                }
                // recalf modul
                recalc();
                return 1;
            }
        }
        // set invalid reference point
        myReferencePoint = ReferencePoint::INVALID;
        // update comboBox
        myReferencePointComboBox->setTextColor(FXRGB(255, 0, 0));
        // disable text field for length
        myLengthTextField->disable();
        // hide force length frame
        myForceLengthFrame->hide();
        // recalc modul
        recalc();
        // set background color
        if (myReferencePointComboBox->getText().empty()) {
            myReferencePointComboBox->setBackColor(FXRGBA(255, 213, 213, 255));
        } else {
            myReferencePointComboBox->setBackColor(FXRGBA(255, 255, 255, 255));
        }
    }
    return 1;
}


long
GNENeteditAttributes::onCmdHelp(FXObject*, FXSelector, void*) {
    // Create dialog box
    FXDialogBox* additionalNeteditAttributesHelpDialog = new FXDialogBox(getCollapsableFrame(), TL("Netedit Parameters Help"), GUIDesignDialogBox);
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
GNENeteditAttributes::getElementLength() const {
    if (GNEAttributeCarrier::canParse<double>(myLengthTextField->getText().text())) {
        const double elementLength = GNEAttributeCarrier::parse<double>(myLengthTextField->getText().text());
        if (elementLength > 0) {
            return elementLength;
        } else {
            return INVALID_DOUBLE;
        }
    } else {
        return INVALID_DOUBLE;
    }
}


double
GNENeteditAttributes::setStartPosition(const double mouseOverLanePos, double elementLength) const {
    switch (myReferencePoint) {
        case ReferencePoint::LEFT:
            return mouseOverLanePos;
        case ReferencePoint::RIGHT:
            if ((mouseOverLanePos - elementLength) <= 0) {
                return INVALID_DOUBLE;
            } else {
                return mouseOverLanePos - elementLength;
            }
        case ReferencePoint::CENTER:
            if ((mouseOverLanePos - (elementLength * 0.5)) <= 0) {
                return INVALID_DOUBLE;
            } else {
                return mouseOverLanePos - (elementLength * 0.5);
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
GNENeteditAttributes::setEndPosition(const double mouseOverLanePos, double elementLength, const double laneLength) const {
    switch (myReferencePoint) {
        case ReferencePoint::LEFT:
            if ((mouseOverLanePos + elementLength) >= laneLength) {
                return INVALID_DOUBLE;
            } else {
                return mouseOverLanePos + elementLength;
            }
        case ReferencePoint::RIGHT:
            return mouseOverLanePos;
        case ReferencePoint::CENTER:
            if ((mouseOverLanePos + (elementLength * 0.5)) >= laneLength) {
                return INVALID_DOUBLE;
            } else {
                return mouseOverLanePos + (elementLength * 0.5);
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
