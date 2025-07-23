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
/// @file    GNEFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2016
///
// The Widget for add additional elements
/****************************************************************************/

#include <netedit/dialogs/GNEHelpAttributesDialog.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/elements/GNEAttributeCarrier.h>
#include <utils/foxtools/MFXDialogBox.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNEFrame.h"

// ===========================================================================
// defines
// ===========================================================================

#define PADDINGFRAME 10 // (5+5)
#define VERTICALSCROLLBARWIDTH 15

// ===========================================================================
// static members
// ===========================================================================

FXFont* GNEFrame::myFrameHeaderFont = nullptr;

// ===========================================================================
// method definitions
// ===========================================================================

GNEFrame::GNEFrame(GNEViewParent* viewParent, GNEViewNet* viewNet, const std::string& frameLabel) :
    FXVerticalFrame(viewParent->getFramesArea(), GUIDesignAuxiliarFrame),
    myViewNet(viewNet) {

    // fill myPredefinedTagsMML (to avoid repeating this fill during every element creation)
    int i = 0;
    while (SUMOXMLDefinitions::attrs[i].key != SUMO_ATTR_NOTHING) {
        int key = SUMOXMLDefinitions::attrs[i].key;
        assert(key >= 0);
        while (key >= (int)myPredefinedTagsMML.size()) {
            myPredefinedTagsMML.push_back("");
        }
        myPredefinedTagsMML[key] = SUMOXMLDefinitions::attrs[i].str;
        i++;
    }

    // Create font only one time
    if (myFrameHeaderFont == nullptr) {
        myFrameHeaderFont = new FXFont(getApp(), "Arial", 14, FXFont::Bold);
    }

    // Create frame for header
    myHeaderFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);

    // Create frame for left elements of header (By default unused)
    myHeaderLeftFrame = new FXHorizontalFrame(myHeaderFrame, GUIDesignAuxiliarHorizontalFrameCenteredVertically);
    myHeaderLeftFrame->hide();

    // Create title frame
    myFrameHeaderLabel = new FXLabel(myHeaderFrame, frameLabel.c_str(), nullptr, GUIDesignLabelFrameInformation);

    // Create frame for right elements of header (By default unused)
    myHeaderRightFrame = new FXHorizontalFrame(myHeaderFrame, GUIDesignAuxiliarHorizontalFrameCenteredVertically);
    myHeaderRightFrame->hide();

    // Add separator
    new FXHorizontalSeparator(this, GUIDesignHorizontalSeparator);

    // Create scroll windows with fixed width for contents
    myScrollWindowsContents = new FXScrollWindow(this, GUIDesignScrollWindowFixed);

    // Create frame for contents (in which GroupBox will be placed)
    myContentFrame = new FXVerticalFrame(myScrollWindowsContents, GUIDesignAuxiliarFrameFixedWidth(0));

    // Set font of header
    myFrameHeaderLabel->setFont(myFrameHeaderFont);

    // set initial width (will be changed in the first update
    setWidth(10);

    // Hide Frame
    FXVerticalFrame::hide();
}


GNEFrame::~GNEFrame() {
    // delete frame header only one time
    if (myFrameHeaderFont) {
        delete myFrameHeaderFont;
        myFrameHeaderFont = nullptr;
    }
}


void
GNEFrame::focusUpperElement() {
    myFrameHeaderLabel->setFocus();
}


void
GNEFrame::show() {
    // show scroll window
    FXVerticalFrame::show();
    // Show and update Frame Area in which this GNEFrame is placed
    myViewNet->getViewParent()->showFramesArea();
}


void
GNEFrame::hide() {
    // hide scroll window
    FXVerticalFrame::hide();
    // Hide Frame Area in which this GNEFrame is placed
    myViewNet->getViewParent()->hideFramesArea();
}


void
GNEFrame::setFrameWidth(const int newWidth) {
    // set scroll windows size (minus MARGIN)
    myScrollWindowsContents->setWidth(newWidth - GUIDesignFrameAreaMargin - DEFAULT_SPACING - 1);
    // calculate new contentWidth
    int contentWidth = (newWidth - GUIDesignFrameAreaMargin - DEFAULT_SPACING - 1 - 15);
    // adjust contents frame
    myContentFrame->setWidth(contentWidth);
    // set size of all contents frame children
    for (auto child = myContentFrame->getFirst(); child != nullptr; child = child->getNext()) {
        child->setWidth(contentWidth);
    }
    // call frame width updated
    frameWidthUpdated();
}


GNEViewNet*
GNEFrame::getViewNet() const {
    return myViewNet;
}


FXVerticalFrame*
GNEFrame::getContentFrame() const {
    return myContentFrame;
}


FXLabel*
GNEFrame::getFrameHeaderLabel() const {
    return myFrameHeaderLabel;
}


FXFont*
GNEFrame::getFrameHeaderFont() const {
    return myFrameHeaderFont;
}


int
GNEFrame::getScrollBarWidth() const {
    if (myScrollWindowsContents->verticalScrollBar()->shown()) {
        return myScrollWindowsContents->verticalScrollBar()->getWidth();
    } else {
        return 0;
    }
}


void
GNEFrame::openHelpAttributesDialog(const GNEAttributeCarrier* AC) const {
    // open help dialog with attributes of the given attribute carrier
    GNEHelpAttributesDialog(myViewNet->getViewParent()->getGNEAppWindows(), AC).openModal();
}


void
GNEFrame::updateFrameAfterUndoRedo() {
    // this function has to be reimplemented in all child frames that needs to draw a polygon (for example, GNEFrame or GNETAZFrame)
}


void
GNEFrame::frameWidthUpdated() {
    // this function can be reimplemented in all child frames
}

// ---------------------------------------------------------------------------
// GNEFrame - protected methods
// ---------------------------------------------------------------------------

void
GNEFrame::tagSelected() {
    // this function has to be reimplemented in all child frames that uses a GNETagSelector module
}


void
GNEFrame::demandElementSelected() {
    // this function has to be reimplemented in all child frames that uses a DemandElementSelector
}


bool
GNEFrame::shapeDrawed() {
    // this function has to be reimplemented in all child frames that needs to draw a polygon (for example, GNEFrame or GNETAZFrame)
    return false;
}


void
GNEFrame::attributeUpdated(SumoXMLAttr /*attribute*/) {
    // this function has to be reimplemented in all child frames that uses a AttributeEditor module
}


void
GNEFrame::selectedOverlappedElement(GNEAttributeCarrier* /* AC */) {
    // this function has to be reimplemented in all child frames that uses a GNEOverlappedInspection
}


bool
GNEFrame::createPath(const bool /*useLastRoute*/) {
    // this function has to be reimplemented in all child frames that uses a path or consecutiveLanePath
    return false;
}


const std::vector<std::string>&
GNEFrame::getPredefinedTagsMML() const {
    return myPredefinedTagsMML;
}


FXLabel*
GNEFrame::buildRainbow(FXComposite* parent) {
    // create label for color information
    FXLabel* label = new FXLabel(parent, TL("Scale: Min -> Max"), nullptr, GUIDesignLabelThick(JUSTIFY_NORMAL));
    // create frame for color scale
    FXHorizontalFrame* horizontalFrameColors = new FXHorizontalFrame(parent, GUIDesignAuxiliarHorizontalFrame);
    for (const auto& color : GNEViewNetHelper::getRainbowScaledColors()) {
        FXLabel* colorLabel = new FXLabel(horizontalFrameColors, "", nullptr, GUIDesignLabel(JUSTIFY_LEFT));
        colorLabel->setBackColor(MFXUtils::getFXColor(color));
    }
    return label;
    // for whatever reason, sonar complains in the next line that horizontalFrameColors may leak, but fox does the cleanup
}  // NOSONAR

/****************************************************************************/
