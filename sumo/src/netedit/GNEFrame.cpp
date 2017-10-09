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

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEFrame::GEOAttributes) GNEFrameGEOAttributes[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNEFRAME_GEOSHAPE,      GNEFrame::GEOAttributes::onCmdSetGEOShape),
    FXMAPFUNC(SEL_COMMAND,  MID_GNEFRAME_LONGITUDE,     GNEFrame::GEOAttributes::onCmdSetLongitude),
    FXMAPFUNC(SEL_COMMAND,  MID_GNEFRAME_LATITUDE,      GNEFrame::GEOAttributes::onCmdSetLatitude),
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

    // Create Frame for GEO Shape
    myGEOShapeFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myGEOShapeLabel = new FXLabel(myGEOShapeFrame, toString(SUMO_ATTR_GEOSHAPE).c_str(), 0, GUIDesignLabelAttribute);
    myGEOShapeTextField = new FXTextField(myGEOShapeFrame, GUIDesignTextFieldNCol, this, MID_GNEFRAME_GEOSHAPE, GUIDesignTextField);


    // Create Frame for longitude
    myLongitudeFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myLongitudeLabel = new FXLabel(myLongitudeFrame, toString(SUMO_ATTR_LON).c_str(), 0, GUIDesignLabelAttribute);
    myLongitudeTextField = new FXTextField(myLongitudeFrame, GUIDesignTextFieldNCol, this, MID_GNEFRAME_LONGITUDE, GUIDesignTextField);

    // Create Frame for latitude
    myLatitudeFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myLatitudeLabel = new FXLabel(myLatitudeFrame, toString(SUMO_ATTR_LAT).c_str(), 0, GUIDesignLabelAttribute);
    myLatitudeTextField = new FXTextField(myLatitudeFrame, GUIDesignTextFieldNCol, this, MID_GNEFRAME_LATITUDE, GUIDesignTextField);
    
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
    assert(ACs.size() > 0);
    myACs = ACs;
    // fill attributes
    UpdateGEOAttributes();
    // show FXGroupBox
    FXGroupBox::show();
}


void 
GNEFrame::GEOAttributes::hideGEOAttributes() {
    myACs.clear();
    // hide FXGroupBox
    FXGroupBox::hide();
}


void 
GNEFrame::GEOAttributes::UpdateGEOAttributes() {
    // hide all frane parameters
    myGEOShapeFrame->hide();
    myLongitudeFrame->hide();
    myLatitudeFrame->hide();
    if(myACs.size() > 1) {
        // only useGEO can be changed in multiple selections
        bool useGEO = true;
        for (auto i : myACs) {
            useGEO &= GNEAttributeCarrier::parse<bool>(i->getAttribute(SUMO_ATTR_GEO));
        }
        myUseGEOCheckButton->setCheck(useGEO);
    } else {
        myUseGEOCheckButton->setCheck(GNEAttributeCarrier::parse<bool>(myACs.front()->getAttribute(SUMO_ATTR_GEO)));
        // show GEO shape only if we're inspecting a POLY
        if(myACs.front()->getTag() == SUMO_TAG_POLY) {
            myGEOShapeFrame->show();
            myGEOShapeTextField->setText(myACs.front()->getAttribute(SUMO_ATTR_GEOSHAPE).c_str());
        } else {
            myLongitudeFrame->show();
            myLatitudeFrame->show();
            myLongitudeTextField->setText(myACs.front()->getAttribute(SUMO_ATTR_LON).c_str());
            myLatitudeTextField->setText(myACs.front()->getAttribute(SUMO_ATTR_LAT).c_str());
        }
    }
    // set text orf GEO button
    if (myUseGEOCheckButton->getCheck()) {
        myUseGEOCheckButton->setText("true");
    } else {
        myUseGEOCheckButton->setText("false");
    }
}


std::map<SumoXMLAttr, std::string> 
GNEFrame::GEOAttributes::getGEOAttributes() const {
    std::map<SumoXMLAttr, std::string> attributes;
    if(GNEAttributeCarrier::hasAttribute(myACs.front()->getTag(), SUMO_ATTR_GEOSHAPE)) {
        attributes[SUMO_ATTR_GEOSHAPE] = myGEOShapeTextField->getText().text();
    } else {
        attributes[SUMO_ATTR_LON] = myLongitudeTextField->getText().text();
        attributes[SUMO_ATTR_LAT] = myLatitudeTextField->getText().text();
    }
    if(myUseGEOCheckButton->getCheck()) {
        attributes[SUMO_ATTR_GEO] = "true";
    } else {
        attributes[SUMO_ATTR_GEO] = "false";
    }
    return attributes;
}


long 
GNEFrame::GEOAttributes::onCmdSetGEOShape(FXObject*, FXSelector, void*) {
    if(myACs.front()->isValid(SUMO_ATTR_GEOSHAPE, myGEOShapeTextField->getText().text())) {
        myACs.front()->setAttribute(SUMO_ATTR_GEOSHAPE, myGEOShapeTextField->getText().text(), myViewNet->getUndoList());
    }
    return 0;
}


long 
GNEFrame::GEOAttributes::onCmdSetLongitude(FXObject*, FXSelector, void*) {
    if(myACs.front()->isValid(SUMO_ATTR_LON, myLongitudeTextField->getText().text())) {
        myACs.front()->setAttribute(SUMO_ATTR_LON, myLongitudeTextField->getText().text(), myViewNet->getUndoList());
    }
    return 0;
}


long 
GNEFrame::GEOAttributes::onCmdSetLatitude(FXObject*, FXSelector, void*) {
    if(myACs.front()->isValid(SUMO_ATTR_LAT, myLongitudeTextField->getText().text())) {
        myACs.front()->setAttribute(SUMO_ATTR_LAT, myLongitudeTextField->getText().text(), myViewNet->getUndoList());
    }
    return 0;
}


long 
GNEFrame::GEOAttributes::onCmdUseGEOParameters(FXObject*, FXSelector, void*) {
    if (myUseGEOCheckButton->getCheck()) {
        myUseGEOCheckButton->setText("true");
    } else {
        myUseGEOCheckButton->setText("false");
    }

    for (auto i : myACs) {
        i->setAttribute(SUMO_ATTR_GEO, myUseGEOCheckButton->getText().text(), myViewNet->getUndoList());
    }

    return 1;
}


long 
GNEFrame::GEOAttributes::onCmdHelp(FXObject*, FXSelector, void*) {
    return 0;
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
