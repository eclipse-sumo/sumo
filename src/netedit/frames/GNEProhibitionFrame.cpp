/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEProhibitionFrame.cpp
/// @author  Mirko Barthauer (Technische Universitaet Braunschweig)
/// @date    May 2018
/// @version $Id$
///
// The Widget for editing connection prohibits
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <iostream>
#include <set>
#include <utils/common/MsgHandler.h>
#include <utils/foxtools/MFXMenuHeader.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/foxtools/fxexdefs.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <netedit/GNEAttributeCarrier.h>
#include <netedit/GNENet.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/netelements/GNEConnection.h>
#include <netedit/netelements/GNEInternalLane.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNEJunction.h>
#include <netedit/additionals/GNEPOI.h>
#include <netedit/additionals/GNEPoly.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>

#include "GNEProhibitionFrame.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GNEProhibitionFrame) GNEProhibitionFrameMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_CANCEL, GNEProhibitionFrame::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND, MID_OK, GNEProhibitionFrame::onCmdOK)
};

// Object implementation
FXIMPLEMENT(GNEProhibitionFrame, FXVerticalFrame, GNEProhibitionFrameMap, ARRAYNUMBER(GNEProhibitionFrameMap))

// ===========================================================================
// static members
// ===========================================================================
RGBColor GNEProhibitionFrame::selectedColor;
RGBColor GNEProhibitionFrame::undefinedColor;
RGBColor GNEProhibitionFrame::prohibitedColor;
RGBColor GNEProhibitionFrame::prohibitingColor;
RGBColor GNEProhibitionFrame::unregulatedConflictColor;
RGBColor GNEProhibitionFrame::mutualConflictColor;

// ===========================================================================
// method definitions
// ===========================================================================

GNEProhibitionFrame::GNEProhibitionFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "Prohibits"), myCurrentConn(0) {
    getFrameHeaderLabel()->setText("Prohibitions");

    // init colors here
    selectedColor = GNENet::selectedConnectionColor;
    undefinedColor = RGBColor::GREY;
    prohibitedColor = RGBColor(0, 179, 0);
    prohibitingColor = RGBColor::RED;
    unregulatedConflictColor = RGBColor::ORANGE;
    mutualConflictColor = RGBColor::CYAN;

    // Create groupbox for current connection information
    myGroupBoxDescription = new FXGroupBox(myContentFrame, "Relative to connection", GUIDesignGroupBoxFrame);

    // Create label for current connection description and update it
    myConnDescriptionLabel = new FXLabel(myGroupBoxDescription, "", 0, GUIDesignLabelFrameInformation);
    updateDescription();

    // Create groupbox for color legend
    myGroupBoxLegend = new FXGroupBox(myContentFrame, "Legend", GUIDesignGroupBoxFrame);

    // Create labels for color legend
    mySelectedLabel = new FXLabel(myGroupBoxLegend, "Selected", 0, GUIDesignLabelFrameInformation);
    mySelectedLabel->setTextColor(MFXUtils::getFXColor(RGBColor::WHITE));
    mySelectedLabel->setBackColor(MFXUtils::getFXColor(selectedColor));
    myUndefinedLabel = new FXLabel(myGroupBoxLegend, "No conflict", 0, GUIDesignLabelFrameInformation);
    myUndefinedLabel->setBackColor(MFXUtils::getFXColor(undefinedColor));
    myProhibitedLabel = new FXLabel(myGroupBoxLegend, "Yields", 0, GUIDesignLabelFrameInformation);
    myProhibitedLabel->setBackColor(MFXUtils::getFXColor(prohibitedColor));
    myProhibitingLabel = new FXLabel(myGroupBoxLegend, "Has right of way", 0, GUIDesignLabelFrameInformation);
    myProhibitingLabel->setBackColor(MFXUtils::getFXColor(prohibitingColor));
    myProhibitingLabel = new FXLabel(myGroupBoxLegend, "Unregulated conflict", 0, GUIDesignLabelFrameInformation);
    myProhibitingLabel->setBackColor(MFXUtils::getFXColor(unregulatedConflictColor));
    myProhibitingLabel = new FXLabel(myGroupBoxLegend, "Mutual conflict", 0, GUIDesignLabelFrameInformation);
    myProhibitingLabel->setBackColor(MFXUtils::getFXColor(mutualConflictColor));

    // Create "Cancel" button
    myCancelButton = new FXButton(this, "Cancel\t\tDiscard prohibition modifications (Esc)",
                                  GUIIconSubSys::getIcon(ICON_CANCEL), this, MID_CANCEL, GUIDesignButton);
    // Create "OK" button
    //mySaveButton = new FXButton(this, "OK\t\tSave prohibition modifications (Enter)",
    //    GUIIconSubSys::getIcon(ICON_ACCEPT), this, MID_OK, GUIDesignButton);
}

GNEProhibitionFrame::~GNEProhibitionFrame() {}

void
GNEProhibitionFrame::handleConnectionClick(GNEConnection* conn, bool /* mayDefinitelyPass */, bool /* allowConflict */, bool /* toggle */) {

    if (myCurrentConn == 0) {
        myCurrentConn = conn;
        myCurrentConn->setSpecialColor(&selectedColor);

        // determine prohibition status of all other connections with respect to the selected one
        GNEJunction* junction = myCurrentConn->getEdgeFrom()->getGNEJunctionDestiny();
        std::vector<GNEConnection*> allConns = junction->getGNEConnections();
        NBNode* node = junction->getNBNode();
        NBEdge* currentConnFrom = myCurrentConn->getEdgeFrom()->getNBEdge();
        NBEdge* currentConnTo = myCurrentConn->getEdgeTo()->getNBEdge();

        for (auto conn : allConns) {
            if (conn != myCurrentConn) {
                NBEdge* otherConnFrom = conn->getEdgeFrom()->getNBEdge();
                NBEdge* otherConnTo = conn->getEdgeTo()->getNBEdge();

                // determine the prohibition status
                bool foes = node->foes(currentConnFrom, currentConnTo, otherConnFrom, otherConnTo);
                bool forbids = node->forbids(currentConnFrom, currentConnTo, otherConnFrom, otherConnTo, true);
                bool forbidden = node->forbids(otherConnFrom, otherConnTo, currentConnFrom, currentConnTo, true);

                myConcernedConns.insert(conn);

                if (!foes) {
                    conn->setSpecialColor(&undefinedColor);
                } else {
                    if (forbids && forbidden) {
                        conn->setSpecialColor(&mutualConflictColor);
                    } else if (forbids) {
                        conn->setSpecialColor(&prohibitedColor);
                    } else if (forbidden) {
                        conn->setSpecialColor(&prohibitingColor);
                    } else {
                        conn->setSpecialColor(&unregulatedConflictColor);
                    }
                }
            }
        }

        updateDescription();
    }
}

void
GNEProhibitionFrame::show() {
    GNEFrame::show();
}

void
GNEProhibitionFrame::hide() {
    GNEFrame::hide();
}

void
GNEProhibitionFrame::updateDescription() const {
    if (myCurrentConn == 0) {
        myConnDescriptionLabel->setText("No Connection selected\n");
    } else {
        myConnDescriptionLabel->setText(("from lane " + myCurrentConn->getLaneFrom()->getMicrosimID() + "\nto lane " + myCurrentConn->getLaneTo()->getMicrosimID()).c_str());
    }
}

long
GNEProhibitionFrame::onCmdCancel(FXObject*, FXSelector, void*) {
    if (myCurrentConn != 0) {
        for (auto conn : myConcernedConns) {
            conn->setSpecialColor(0);
        }
        myCurrentConn->setSpecialColor(0);
        myCurrentConn = 0;
        myConcernedConns.clear();
        updateDescription();
        myViewNet->update();
    }
    return 1;
}

long
GNEProhibitionFrame::onCmdOK(FXObject*, FXSelector, void*) {
    return 1;
}


/****************************************************************************/
