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

#include <utils/foxtools/MFXUtils.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/netelements/GNEConnection.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNEJunction.h>

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
    GNEFrame(horizontalFrameParent, viewNet, "Prohibits"), myCurrentConn(nullptr) {
    getFrameHeaderLabel()->setText("Prohibitions");

    // init colors here
    selectedColor = viewNet->getVisualisationSettings()->selectedConnectionColor;
    undefinedColor = RGBColor::GREY;
    prohibitedColor = RGBColor(0, 179, 0);
    prohibitingColor = RGBColor::RED;
    unregulatedConflictColor = RGBColor::ORANGE;
    mutualConflictColor = RGBColor::CYAN;

    // Create groupbox for current connection information
    myGroupBoxDescription = new FXGroupBox(myContentFrame, "Relative to connection", GUIDesignGroupBoxFrame);

    // Create label for current connection description and update it
    myConnDescriptionLabel = new FXLabel(myGroupBoxDescription, "", nullptr, GUIDesignLabelFrameInformation);
    updateDescription();

    // Create groupbox for color legend
    myGroupBoxLegend = new FXGroupBox(myContentFrame, "Legend", GUIDesignGroupBoxFrame);

    // Create labels for color legend
    mySelectedLabel = new FXLabel(myGroupBoxLegend, "Selected", nullptr, GUIDesignLabelFrameInformation);
    mySelectedLabel->setTextColor(MFXUtils::getFXColor(RGBColor::WHITE));
    mySelectedLabel->setBackColor(MFXUtils::getFXColor(selectedColor));
    myUndefinedLabel = new FXLabel(myGroupBoxLegend, "No conflict", nullptr, GUIDesignLabelFrameInformation);
    myUndefinedLabel->setBackColor(MFXUtils::getFXColor(undefinedColor));
    myProhibitedLabel = new FXLabel(myGroupBoxLegend, "Yields", nullptr, GUIDesignLabelFrameInformation);
    myProhibitedLabel->setBackColor(MFXUtils::getFXColor(prohibitedColor));
    myProhibitingLabel = new FXLabel(myGroupBoxLegend, "Has right of way", nullptr, GUIDesignLabelFrameInformation);
    myProhibitingLabel->setBackColor(MFXUtils::getFXColor(prohibitingColor));
    myProhibitingLabel = new FXLabel(myGroupBoxLegend, "Unregulated conflict", nullptr, GUIDesignLabelFrameInformation);
    myProhibitingLabel->setBackColor(MFXUtils::getFXColor(unregulatedConflictColor));
    myProhibitingLabel = new FXLabel(myGroupBoxLegend, "Mutual conflict", nullptr, GUIDesignLabelFrameInformation);
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
GNEProhibitionFrame::handleProhibitionClick(const GNEViewNet::ObjectsUnderCursor &objectsUnderCursor) {
    // build prohibition
    buildProhibition(objectsUnderCursor.getConnectionFront(), myViewNet->getKeyPressed().shiftKeyPressed(), myViewNet->getKeyPressed().controlKeyPressed(), true);
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
    if (myCurrentConn == nullptr) {
        myConnDescriptionLabel->setText("No Connection selected\n");
    } else {
        myConnDescriptionLabel->setText(("from lane " + myCurrentConn->getLaneFrom()->getMicrosimID() + "\nto lane " + myCurrentConn->getLaneTo()->getMicrosimID()).c_str());
    }
}


long
GNEProhibitionFrame::onCmdCancel(FXObject*, FXSelector, void*) {
    if (myCurrentConn != nullptr) {
        for (auto conn : myConcernedConns) {
            conn->setSpecialColor(nullptr);
        }
        myCurrentConn->setSpecialColor(nullptr);
        myCurrentConn = nullptr;
        myConcernedConns.clear();
        updateDescription();
        myViewNet->update();
    }
    return 1;
}


void
GNEProhibitionFrame::buildProhibition(GNEConnection* conn, bool /* mayDefinitelyPass */, bool /* allowConflict */, bool /* toggle */) {
    if (myCurrentConn == nullptr) {
        myCurrentConn = conn;
        myCurrentConn->setSpecialColor(&selectedColor);

        // determine prohibition status of all other connections with respect to the selected one
        GNEJunction* junction = myCurrentConn->getEdgeFrom()->getGNEJunctionDestiny();
        std::vector<GNEConnection*> allConns = junction->getGNEConnections();
        NBNode* node = junction->getNBNode();
        NBEdge* currentConnFrom = myCurrentConn->getEdgeFrom()->getNBEdge();

        const int currentLinkIndex = node->getConnectionIndex(currentConnFrom, myCurrentConn->getNBEdgeConnection());
        std::string currentFoesString = node->getFoes(currentLinkIndex);
        std::string currentResponseString = node->getResponse(currentLinkIndex);
        std::reverse(currentFoesString.begin(), currentFoesString.end());
        std::reverse(currentResponseString.begin(), currentResponseString.end());

        for (auto i : allConns) {
            if (i != myCurrentConn) {
                NBEdge* otherConnFrom = i->getEdgeFrom()->getNBEdge();
                const int linkIndex = node->getConnectionIndex(otherConnFrom, i->getNBEdgeConnection());
                std::string responseString = node->getResponse(linkIndex);
                std::reverse(responseString.begin(), responseString.end());
                // determine the prohibition status
                bool foes = (int)currentFoesString.size() > linkIndex && currentFoesString[linkIndex] == '1';
                bool forbids = (int)responseString.size() > currentLinkIndex && responseString[currentLinkIndex] == '1';
                bool forbidden = (int)currentResponseString.size() > linkIndex && currentResponseString[linkIndex] == '1';

                myConcernedConns.insert(i);

                if (!foes) {
                    i->setSpecialColor(&undefinedColor);
                } else {
                    if (forbids && forbidden) {
                        i->setSpecialColor(&mutualConflictColor);
                    } else if (forbids) {
                        i->setSpecialColor(&prohibitedColor);
                    } else if (forbidden) {
                        i->setSpecialColor(&prohibitingColor);
                    } else {
                        i->setSpecialColor(&unregulatedConflictColor);
                    }
                }
            }
        }

        updateDescription();
    }
}


long
GNEProhibitionFrame::onCmdOK(FXObject*, FXSelector, void*) {
    return 1;
}


/****************************************************************************/
