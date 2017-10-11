/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2017 German Aerospace Center (DLR) and others.
/****************************************************************************/
//
//   This program and the accompanying materials
//   are made available under the terms of the Eclipse Public License v2.0
//   which accompanies this distribution, and is available at
//   http://www.eclipse.org/legal/epl-v20.html
//
/****************************************************************************/
/// @file    GNEFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2016
/// @version $Id$
///
// The Widget for add additional elements
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

#include "GNEFrame.h"
#include "GNEViewParent.h"
#include "GNEViewNet.h"
#include "GNEAttributeCarrier.h"
#include "GNEInspectorFrame.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEFrame::GEOAttributes) GNEFrameGEOAttributes[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNEFRAME_GEOATTRIBUTE,  GNEFrame::GEOAttributes::onCmdSetGEOAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNEFRAME_USEGEO,        GNEFrame::GEOAttributes::onCmdUseGEOParameters),
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,                   GNEFrame::GEOAttributes::onCmdHelp),
};

FXIMPLEMENT(GNEFrame::GEOAttributes,    FXGroupBox,     GNEFrameGEOAttributes,  ARRAYNUMBER(GNEFrameGEOAttributes))


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEFrame::GEOAttributes - methods
// ---------------------------------------------------------------------------

GNEFrame::GEOAttributes::GEOAttributes(FXComposite* parent, GNEViewNet *viewNet) :
    FXGroupBox(parent, "GEO Attributes", GUIDesignGroupBoxFrame),
    myViewNet(viewNet) {

    // Create Frame for GEOAttribute
    myGEOAttributeFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myGEOAttributeLabel = new FXLabel(myGEOAttributeFrame, "Undefined GEO Attribute", 0, GUIDesignLabelAttribute);
    myGEOAttributeTextField = new FXTextField(myGEOAttributeFrame, GUIDesignTextFieldNCol, this, MID_GNEFRAME_GEOATTRIBUTE, GUIDesignTextField);
    
    // Create Frame for use GEO
    myUseGEOFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myUseGEOLabel = new FXLabel(myUseGEOFrame, "Use GEO", 0, GUIDesignLabelAttribute);
    myUseGEOCheckButton = new FXCheckButton(myUseGEOFrame, "false", this, MID_GNEFRAME_USEGEO, GUIDesignCheckButtonAttribute);
    
    // Create help button
    myHelpButton = new FXButton(this, "Help", 0, this, MID_HELP, GUIDesignButtonRectangular);
}


GNEFrame::GEOAttributes::~GEOAttributes() {}


void 
GNEFrame::GEOAttributes::showGEOAttributes(const std::vector<GNEAttributeCarrier*> &ACs) {
    // make sure that ACs has elements
    if(ACs.size() > 0) {
        // set myACs with the inspected elements
        myACs = ACs;
        if(ACs.front()->getTag() == SUMO_TAG_POLY) {
            myGEOAttribute = SUMO_ATTR_GEOSHAPE;
            // set label name
            myGEOAttributeLabel->setText(toString(myGEOAttribute).c_str());
            // fill attributes using refresh attributes
            refreshGEOAttributes();
            // show FXGroupBox
            FXGroupBox::show();
        } else if(ACs.front()->getTag() == SUMO_TAG_POI) {
            myGEOAttribute = SUMO_ATTR_GEOPOSITION;
            // set label name
            myGEOAttributeLabel->setText(toString(myGEOAttribute).c_str());
            // fill attributes using refresh attributes
            refreshGEOAttributes();
            // show FXGroupBox
            FXGroupBox::show();
        } else {
            // hide GEO Attributes
            hideGEOAttributes();
        }
    } else {
        // hide GEO Attributes
        hideGEOAttributes();
    }
}


void 
GNEFrame::GEOAttributes::hideGEOAttributes() {
    myACs.clear();
    myGEOAttribute = SUMO_ATTR_NOTHING;
    // hide FXGroupBox
    FXGroupBox::hide();
}


void 
GNEFrame::GEOAttributes::refreshGEOAttributes() {
    // only refresh element if myACs has elements
    if(myACs.size() > 0) {
        // hide GEOAttribute Frame
        myGEOAttributeFrame->hide();
        // check if we're handling a single or multiple selection
        if(myACs.size() > 1) {
            // only useGEO can be changed in multiple selections
            bool useGEO = true;
            for (auto i : myACs) {
                useGEO &= GNEAttributeCarrier::parse<bool>(i->getAttribute(SUMO_ATTR_GEO));
            }
            myUseGEOCheckButton->setCheck(useGEO);
        } else {
            myUseGEOCheckButton->setCheck(GNEAttributeCarrier::parse<bool>(myACs.front()->getAttribute(SUMO_ATTR_GEO)));
            // show GEO Attribute (GNEShape or GNEPosition)
            myGEOAttributeFrame->show();
            myGEOAttributeTextField->setText(myACs.front()->getAttribute(myGEOAttribute).c_str());
            myGEOAttributeTextField->setTextColor(FXRGB(0, 0, 0));
        }
        // set text orf GEO button
        if (myUseGEOCheckButton->getCheck()) {
            myUseGEOCheckButton->setText("true");
        } else {
            myUseGEOCheckButton->setText("false");
        }
    }
}


std::map<SumoXMLAttr, std::string> 
GNEFrame::GEOAttributes::getGEOAttributes() const {
    std::map<SumoXMLAttr, std::string> attributes;
    // fill map with the GEO Attributes
    attributes[myGEOAttribute] = myGEOAttributeTextField->getText().text();
    attributes[SUMO_ATTR_GEO] = myUseGEOCheckButton->getCheck()? "true" : "false";
    return attributes;
}


long 
GNEFrame::GEOAttributes::onCmdSetGEOAttribute(FXObject*, FXSelector, void*) {
    if(myGEOAttributeTextField->getText().empty()) {
        WRITE_WARNING("GEO Shapes cannot be empty.");
    } else if(myACs.front()->isValid(myGEOAttribute, myGEOAttributeTextField->getText().text())) {
        myACs.front()->setAttribute(myGEOAttribute, myGEOAttributeTextField->getText().text(), myViewNet->getUndoList());
        myGEOAttributeTextField->setTextColor(FXRGB(0, 0, 0));
    } else {
        myGEOAttributeTextField->setTextColor(FXRGB(255, 0, 0));
        myGEOAttributeTextField->killFocus();
    }
    // refresh values of current inspected item (because attribute shape changes)
    myViewNet->getViewParent()->getInspectorFrame()->refreshValues();
    return 0;
}


long 
GNEFrame::GEOAttributes::onCmdUseGEOParameters(FXObject*, FXSelector, void*) {
    // change label of Check button depending of check
    if (myUseGEOCheckButton->getCheck()) {
        myUseGEOCheckButton->setText("true");
    } else {
        myUseGEOCheckButton->setText("false");
    }
    // update GEO Attribute of entire selection
    for (auto i : myACs) {
        i->setAttribute(SUMO_ATTR_GEO, myUseGEOCheckButton->getText().text(), myViewNet->getUndoList());
    }
    return 1;
}


long 
GNEFrame::GEOAttributes::onCmdHelp(FXObject*, FXSelector, void*) {
    FXDialogBox* helpDialog = new FXDialogBox(this, "GEO attributes Help", GUIDesignDialogBox);
    std::ostringstream help;
    help
        << " SUMO uses the World Geodetic System 84 (WGS84/UTM).\n" 
        << " GEO coordinates are represented as pairs of Longitude and Latitude\n"
        << " in decimal degrees without extra symbols. (°,N,W..)\n"
        << " - Longitude: East-west position of a point on the Earth's surface.\n"
        << " - Latitude: North–south position of a point on the Earth's surface.\n"
        << " - CheckBox 'use GEO' enable or disable saving position in GEO coordinates\n";
    new FXLabel(helpDialog, help.str().c_str(), 0, GUIDesignLabelLeft);
    // "OK"
    new FXButton(helpDialog, "OK\t\tclose", GUIIconSubSys::getIcon(ICON_ACCEPT), helpDialog, FXDialogBox::ID_ACCEPT, GUIDesignButtonOK);
    helpDialog->create();
    helpDialog->show();
    return 1;
}

// ---------------------------------------------------------------------------
// GNEFrame - methods
// ---------------------------------------------------------------------------

GNEFrame::GNEFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet, const std::string& frameLabel) :
    FXVerticalFrame(horizontalFrameParent, GUIDesignAuxiliarFrame),
    myViewNet(viewNet) {
    // Create font
    myFrameHeaderFont = new FXFont(getApp(), "Arial", 14, FXFont::Bold),

    // Create frame for header
    myHeaderFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);

    // Create frame for left elements of header (By default unused)
    myHeaderLeftFrame = new FXHorizontalFrame(myHeaderFrame, GUIDesignAuxiliarHorizontalFrame);
    myHeaderLeftFrame->hide();

    // Create titel frame
    myFrameHeaderLabel = new FXLabel(myHeaderFrame, frameLabel.c_str(), 0, GUIDesignLabelLeft);

    // Create frame for right elements of header (By default unused)
    myHeaderRightFrame = new FXHorizontalFrame(myHeaderFrame, GUIDesignAuxiliarHorizontalFrame);
    myHeaderRightFrame->hide();

    // Add separator
    new FXHorizontalSeparator(this, GUIDesignHorizontalSeparator);

    // Create frame for contents
    myScrollWindowsContents = new FXScrollWindow(this, GUIDesignContentsScrollWindow);

    // Create frame for contents
    myContentFrame = new FXVerticalFrame(myScrollWindowsContents, GUIDesignContentsFrame);

    // Set font of header
    myFrameHeaderLabel->setFont(myFrameHeaderFont);

    // Hide Frame
    FXVerticalFrame::hide();
}


GNEFrame::~GNEFrame() {
    delete myFrameHeaderFont;
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
GNEFrame::setFrameWidth(int newWidth) {
    setWidth(newWidth);
    myScrollWindowsContents->setWidth(newWidth);
}


GNEViewNet*
GNEFrame::getViewNet() const {
    return myViewNet;
}


FXLabel*
GNEFrame::getFrameHeaderLabel() const {
    return myFrameHeaderLabel;
}


FXFont*
GNEFrame::getFrameHeaderFont() const {
    return myFrameHeaderFont;
}


/****************************************************************************/
