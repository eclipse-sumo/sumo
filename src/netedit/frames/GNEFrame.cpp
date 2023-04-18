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
/// @file    GNEFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2016
///
// The Widget for add additional elements
/****************************************************************************/
#include <config.h>

#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNEApplicationWindow.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNEFrame.h"


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
    myHeaderLeftFrame = new FXHorizontalFrame(myHeaderFrame, GUIDesignAuxiliarHorizontalFrame);
    myHeaderLeftFrame->hide();

    // Create titel frame
    myFrameHeaderLabel = new FXLabel(myHeaderFrame, frameLabel.c_str(), nullptr, GUIDesignLabelFrameInformation);

    // Create frame for right elements of header (By default unused)
    myHeaderRightFrame = new FXHorizontalFrame(myHeaderFrame, GUIDesignAuxiliarHorizontalFrame);
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
    // set scroll windows size (minus MARGING)
    myScrollWindowsContents->setWidth(newWidth - GUIDesignFrameAreaMarging - DEFAULT_SPACING - 1);
    // calculate new contentWidth
    int contentWidth = (newWidth - GUIDesignFrameAreaMarging - DEFAULT_SPACING - 1 - 15);
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
    FXDialogBox* attributesHelpDialog = new FXDialogBox(myScrollWindowsContents, (TL("Parameters of ") + AC->getTagStr()).c_str(), GUIDesignDialogBoxResizable, 0, 0, 0, 0, 10, 10, 10, 38, 4, 4);
    // Create FXTable
    FXTable* myTable = new FXTable(attributesHelpDialog, attributesHelpDialog, MID_TABLE, GUIDesignTableNotEditable);
    attributesHelpDialog->setIcon(GUIIconSubSys::getIcon(GUIIcon::MODEINSPECT));
    int sizeColumnDescription = 0;
    int sizeColumnDefinitions = 0;
    myTable->setVisibleRows((FXint)(AC->getTagProperty().getNumberOfAttributes()));
    myTable->setVisibleColumns(3);
    myTable->setTableSize((FXint)(AC->getTagProperty().getNumberOfAttributes()), 3);
    myTable->setBackColor(FXRGB(255, 255, 255));
    myTable->setColumnText(0, TL("Attribute"));
    myTable->setColumnText(1, TL("Description"));
    myTable->setColumnText(2, TL("Definition"));
    myTable->getRowHeader()->setWidth(0);
    // Iterate over vector of additional parameters
    int itemIndex = 0;
    for (const auto& tagProperty : AC->getTagProperty()) {
        // Set attribute
        FXTableItem* attribute = new FXTableItem(tagProperty.getAttrStr().c_str());
        attribute->setJustify(FXTableItem::CENTER_X);
        myTable->setItem(itemIndex, 0, attribute);
        // Set description of element
        FXTableItem* type = new FXTableItem("");
        type->setText(tagProperty.getDescription().c_str());
        sizeColumnDescription = MAX2(sizeColumnDescription, (int)tagProperty.getDescription().size());
        type->setJustify(FXTableItem::CENTER_X);
        myTable->setItem(itemIndex, 1, type);
        // Set definition
        FXTableItem* definition = new FXTableItem(tagProperty.getDefinition().c_str());
        definition->setJustify(FXTableItem::LEFT);
        myTable->setItem(itemIndex, 2, definition);
        sizeColumnDefinitions = MAX2(sizeColumnDefinitions, (int)tagProperty.getDefinition().size());
        itemIndex++;
    }
    myTable->fitRowsToContents(0, itemIndex);
    // set header
    FXHeader* header = myTable->getColumnHeader();
    header->setItemJustify(0, JUSTIFY_CENTER_X);
    header->setItemSize(0, 120);
    header->setItemJustify(1, JUSTIFY_CENTER_X);
    header->setItemSize(1, sizeColumnDescription * 7);
    header->setItemJustify(2, JUSTIFY_CENTER_X);
    header->setItemSize(2, sizeColumnDefinitions * 6);
    // Create horizontal separator
    new FXHorizontalSeparator(attributesHelpDialog, GUIDesignHorizontalSeparator);
    // Create frame for OK Button
    FXHorizontalFrame* myHorizontalFrameOKButton = new FXHorizontalFrame(attributesHelpDialog, GUIDesignAuxiliarHorizontalFrame);
    // Create Button Close (And two more horizontal frames to center it)
    new FXHorizontalFrame(myHorizontalFrameOKButton, GUIDesignAuxiliarHorizontalFrame);
    new FXButton(myHorizontalFrameOKButton, (TL("OK") + std::string("\t\t") + TL("close")).c_str(), GUIIconSubSys::getIcon(GUIIcon::ACCEPT), attributesHelpDialog, FXDialogBox::ID_ACCEPT, GUIDesignButtonOK);
    new FXHorizontalFrame(myHorizontalFrameOKButton, GUIDesignAuxiliarHorizontalFrame);
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG(TL("Opening HelpAttributes dialog for tag '") + AC->getTagProperty().getTagStr() + TL("' showing ") + toString(AC->getTagProperty().getNumberOfAttributes()) + TL(" attributes"));
    // create Dialog
    attributesHelpDialog->create();
    // show in the given position
    attributesHelpDialog->show(PLACEMENT_CURSOR);
    // refresh APP
    getApp()->refresh();
    // open as modal dialog (will block all windows until stop() or stopModal() is called)
    getApp()->runModalFor(attributesHelpDialog);
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG(TL("Closing HelpAttributes dialog for tag '") + AC->getTagProperty().getTagStr() + "'");
}


void
GNEFrame::updateFrameAfterUndoRedo() {
    // this function has to be reimplemente in all child frames that needs to draw a polygon (for example, GNEFrame or GNETAZFrame)
}


void
GNEFrame::frameWidthUpdated() {
    // this function can be reimplemente in all child frames
}

// ---------------------------------------------------------------------------
// GNEFrame - protected methods
// ---------------------------------------------------------------------------

void
GNEFrame::tagSelected() {
    // this function has to be reimplemente in all child frames that uses a GNETagSelector modul
}


void
GNEFrame::demandElementSelected() {
    // this function has to be reimplemente in all child frames that uses a DemandElementSelector
}


bool
GNEFrame::shapeDrawed() {
    // this function has to be reimplemente in all child frames that needs to draw a polygon (for example, GNEFrame or GNETAZFrame)
    return false;
}


void
GNEFrame::attributeUpdated(SumoXMLAttr /*attribute*/) {
    // this function has to be reimplemente in all child frames that uses a AttributeEditor modul
}


void
GNEFrame::attributesEditorExtendedDialogOpened()  {
    // this function has to be reimplemente in all child frames that uses a GNEAttributesCreator editor with extended attributes
}


void
GNEFrame::selectedOverlappedElement(GNEAttributeCarrier* /* AC */) {
    // this function has to be reimplemente in all child frames that uses a GNEOverlappedInspection
}


bool
GNEFrame::createPath(const bool /*useLastRoute*/) {
    // this function has to be reimplemente in all child frames that uses a path or consecutiveLanePath
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
