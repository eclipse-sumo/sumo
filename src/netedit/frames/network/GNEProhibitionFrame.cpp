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
/// @file    GNEProhibitionFrame.cpp
/// @author  Mirko Barthauer (Technische Universitaet Braunschweig)
/// @date    May 2018
///
// The Widget for editing connection prohibits
/****************************************************************************/

#include <utils/gui/div/GUIDesigns.h>
#include <netedit/elements/network/GNELane.h>
#include <netedit/elements/network/GNEConnection.h>
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/elements/network/GNEJunction.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNEApplicationWindow.h>

#include "GNEProhibitionFrame.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEProhibitionFrame::Selection) SelectionMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_OK,         GNEProhibitionFrame::Selection::onCmdOK),
    FXMAPFUNC(SEL_COMMAND,  MID_CANCEL,     GNEProhibitionFrame::Selection::onCmdCancel),
    FXMAPFUNC(SEL_UPDATE,   MID_CANCEL,     GNEProhibitionFrame::Selection::onCmdRequireConnection),
};

// Object implementation
FXIMPLEMENT(GNEProhibitionFrame::Selection, FXVerticalFrame, SelectionMap, ARRAYNUMBER(SelectionMap))

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEProhibitionFrame::RelativeToConnection - methods
// ---------------------------------------------------------------------------

GNEProhibitionFrame::RelativeToConnection::RelativeToConnection(GNEProhibitionFrame* prohibitionFrameParent) :
    MFXGroupBoxModule(prohibitionFrameParent, TL("Selected connection")),
    myProhibitionFrameParent(prohibitionFrameParent) {
    // Create label for current connection description and update it
    myConnDescriptionLabel = new FXLabel(getCollapsableFrame(), "", nullptr, GUIDesignLabelFrameInformation);
    // update description
    updateDescription();
}


GNEProhibitionFrame::RelativeToConnection::~RelativeToConnection() {}


void
GNEProhibitionFrame::RelativeToConnection::updateDescription() const {
    // update depending of myCurrentConn
    if (myProhibitionFrameParent->myCurrentConn == nullptr) {
        myConnDescriptionLabel->setText(TL("No Connection selected\n"));
    } else {
        myConnDescriptionLabel->setText(
            (TL("- Junction: ") + myProhibitionFrameParent->myCurrentConn->getEdgeFrom()->getToJunction()->getID() + "\n" +
             TL("- From lane: ") + myProhibitionFrameParent->myCurrentConn->getLaneFrom()->getMicrosimID() + "\n" +
             TL("- To lane: ") + myProhibitionFrameParent->myCurrentConn->getLaneTo()->getMicrosimID()).c_str());
    }
}

// ---------------------------------------------------------------------------
// GNEProhibitionFrame::ProhibitionLegend - methods
// ---------------------------------------------------------------------------

GNEProhibitionFrame::Legend::Legend(GNEProhibitionFrame* prohibitionFrameParent) :
    MFXGroupBoxModule(prohibitionFrameParent, TL("Information")),
    myUndefinedColor(RGBColor::GREY),
    myProhibitedColor(RGBColor(0, 179, 0)),
    myProhibitingColor(RGBColor::RED),
    myUnregulatedConflictColor(RGBColor::ORANGE),
    myMutualConflictColor(RGBColor::CYAN) {
    // Create labels for color legend
    MFXLabelTooltip* legendLabel = new MFXLabelTooltip(getCollapsableFrame(),
            prohibitionFrameParent->getViewNet()->getViewParent()->getGNEAppWindows()->getStaticTooltipMenu(),
            TL("Selected"), nullptr, GUIDesignLabelFrameInformation);
    legendLabel->setTipText(TL("Current selected connection"));
    legendLabel->setTextColor(MFXUtils::getFXColor(RGBColor::WHITE));
    legendLabel->setBackColor(MFXUtils::getFXColor(prohibitionFrameParent->myViewNet->getVisualisationSettings().colorSettings.selectedProhibitionColor));
    // label for conflicts
    legendLabel = new MFXLabelTooltip(getCollapsableFrame(),
                                      prohibitionFrameParent->getViewNet()->getViewParent()->getGNEAppWindows()->getStaticTooltipMenu(),
                                      TL("No conflict"), nullptr, GUIDesignLabelFrameInformation);
    legendLabel->setTipText(TL("No conflict with the selected connection"));
    legendLabel->setBackColor(MFXUtils::getFXColor(myUndefinedColor));
    // label for yields
    legendLabel = new MFXLabelTooltip(getCollapsableFrame(),
                                      prohibitionFrameParent->getViewNet()->getViewParent()->getGNEAppWindows()->getStaticTooltipMenu(),
                                      TL("Yields"), nullptr, GUIDesignLabelFrameInformation);
    legendLabel->setTipText(TL("Connection yields the selected connection"));
    legendLabel->setBackColor(MFXUtils::getFXColor(myProhibitedColor));
    // label for right of way
    legendLabel = new MFXLabelTooltip(getCollapsableFrame(),
                                      prohibitionFrameParent->getViewNet()->getViewParent()->getGNEAppWindows()->getStaticTooltipMenu(),
                                      TL("Has right of way"), nullptr, GUIDesignLabelFrameInformation);
    legendLabel->setTipText(TL("Connection has right of way with the selected connection"));
    legendLabel->setBackColor(MFXUtils::getFXColor(myProhibitingColor));
    // label for unregulated conflict
    legendLabel = new MFXLabelTooltip(getCollapsableFrame(),
                                      prohibitionFrameParent->getViewNet()->getViewParent()->getGNEAppWindows()->getStaticTooltipMenu(),
                                      TL("Unregulated conflict"), nullptr, GUIDesignLabelFrameInformation);
    legendLabel->setTipText(TL("Connection has a unregulated conflict with the selected connection"));
    legendLabel->setBackColor(MFXUtils::getFXColor(myUnregulatedConflictColor));
    // label for mutual conflict
    legendLabel = new MFXLabelTooltip(getCollapsableFrame(),
                                      prohibitionFrameParent->getViewNet()->getViewParent()->getGNEAppWindows()->getStaticTooltipMenu(),
                                      TL("Mutual conflict"), nullptr, GUIDesignLabelFrameInformation);
    legendLabel->setTipText(TL("Connection has a mutual conflict with the selected connection"));
    legendLabel->setBackColor(MFXUtils::getFXColor(myMutualConflictColor));
}


GNEProhibitionFrame::Legend::~Legend() {}


const RGBColor&
GNEProhibitionFrame::Legend::getUndefinedColor() const {
    return myUndefinedColor;
}


const RGBColor&
GNEProhibitionFrame::Legend::getProhibitedColor() const {
    return myProhibitedColor;
}


const RGBColor&
GNEProhibitionFrame::Legend::getProhibitingColor() const {
    return myProhibitingColor;
}


const RGBColor&
GNEProhibitionFrame::Legend::getUnregulatedConflictColor() const {
    return myUnregulatedConflictColor;
}


const RGBColor&
GNEProhibitionFrame::Legend::getMutualConflictColor() const {
    return myMutualConflictColor;
}

// ---------------------------------------------------------------------------
// GNEProhibitionFrame::Selection - methods
// ---------------------------------------------------------------------------

GNEProhibitionFrame::Selection::Selection(GNEProhibitionFrame* prohibitionFrameParent) :
    MFXGroupBoxModule(prohibitionFrameParent, TL("Selection")),
    myProhibitionFrameParent(prohibitionFrameParent) {
    // Create "OK" button
    mySaveButton = new MFXButtonTooltip(getCollapsableFrame(),
                                        prohibitionFrameParent->getViewNet()->getViewParent()->getGNEAppWindows()->getStaticTooltipMenu(),
                                        TL("OK"),
                                        GUIIconSubSys::getIcon(GUIIcon::ACCEPT), this, MID_OK, GUIDesignButton);
    mySaveButton->setTipText(TL("Save prohibition modifications (Enter)"));
    // Create "Cancel" button
    myCancelButton = new MFXButtonTooltip(getCollapsableFrame(),
                                          prohibitionFrameParent->getViewNet()->getViewParent()->getGNEAppWindows()->getStaticTooltipMenu(),
                                          TL("Unselect connection"),
                                          GUIIconSubSys::getIcon(GUIIcon::CANCEL), this, MID_CANCEL, GUIDesignButton);
    myCancelButton->setTipText(TL("Unselect connection (Esc)"));
    // Currently mySaveButton is hidden
    mySaveButton->hide();
}


GNEProhibitionFrame::Selection::~Selection() {}


long
GNEProhibitionFrame::Selection::onCmdOK(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNEProhibitionFrame::Selection::onCmdCancel(FXObject*, FXSelector, void*) {
    if (myProhibitionFrameParent->myCurrentConn != nullptr) {
        for (const auto& conn : myProhibitionFrameParent->myConcernedConns) {
            conn->setSpecialColor(nullptr);
        }
        myProhibitionFrameParent->myCurrentConn->setSpecialColor(nullptr);
        myProhibitionFrameParent->myCurrentConn = nullptr;
        myProhibitionFrameParent->myConcernedConns.clear();
        myProhibitionFrameParent->myRelativeToConnection->updateDescription();
        myProhibitionFrameParent->myViewNet->updateViewNet();
    }
    return 1;
}


long
GNEProhibitionFrame::Selection::onCmdRequireConnection(FXObject*, FXSelector, void*) {
    if (myProhibitionFrameParent->myCurrentConn) {
        mySaveButton->enable();
        myCancelButton->enable();
    } else {
        mySaveButton->disable();
        myCancelButton->disable();
    }
    return 1;
}

// ---------------------------------------------------------------------------
// GNEProhibitionFrame - methods
// ---------------------------------------------------------------------------

GNEProhibitionFrame::GNEProhibitionFrame(GNEViewParent* viewParent, GNEViewNet* viewNet) :
    GNEFrame(viewParent, viewNet, TL("Prohibitions")),
    myCurrentConn(nullptr) {

    // create RelativeToConnection
    myRelativeToConnection = new RelativeToConnection(this);

    // create legend
    myLegend = new Legend(this);

    // create Selection modul
    mySelectionModul = new Selection(this);
}


GNEProhibitionFrame::~GNEProhibitionFrame() {}


void
GNEProhibitionFrame::handleProhibitionClick(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor) {
    // build prohibition
    buildProhibition(objectsUnderCursor.getConnectionFront(), myViewNet->getMouseButtonKeyPressed().shiftKeyPressed(), myViewNet->getMouseButtonKeyPressed().controlKeyPressed(), true);
}


void
GNEProhibitionFrame::show() {
    GNEFrame::show();
}


void
GNEProhibitionFrame::hide() {
    GNEFrame::hide();
}


GNEProhibitionFrame::Selection*
GNEProhibitionFrame::getSelectionModul() const {
    return mySelectionModul;
}

// ---------------------------------------------------------------------------
// GNEProhibitionFrame - private methods
// ---------------------------------------------------------------------------

void
GNEProhibitionFrame::buildProhibition(GNEConnection* conn, bool /* mayDefinitelyPass */, bool /* allowConflict */, bool /* toggle */) {
    if (myCurrentConn != nullptr) {
        mySelectionModul->onCmdCancel(nullptr, 0, nullptr);
    }
    if (conn != nullptr) {
        myCurrentConn = conn;
        myCurrentConn->setSpecialColor(&myViewNet->getVisualisationSettings().colorSettings.selectedProhibitionColor);

        // determine prohibition status of all other connections with respect to the selected one
        GNEJunction* junction = myCurrentConn->getEdgeFrom()->getToJunction();
        std::vector<GNEConnection*> connections = junction->getGNEConnections();
        NBNode* node = junction->getNBNode();
        NBEdge* currentConnFrom = myCurrentConn->getEdgeFrom()->getNBEdge();

        const int currentLinkIndex = node->getConnectionIndex(currentConnFrom, myCurrentConn->getNBEdgeConnection());
        std::string currentFoesString = node->getFoes(currentLinkIndex);
        std::string currentResponseString = node->getResponse(currentLinkIndex);
        std::reverse(currentFoesString.begin(), currentFoesString.end());
        std::reverse(currentResponseString.begin(), currentResponseString.end());
        // iterate over all connections
        for (const auto& connection : connections) {
            if (connection != myCurrentConn) {
                NBEdge* otherConnFrom = connection->getEdgeFrom()->getNBEdge();
                const int linkIndex = node->getConnectionIndex(otherConnFrom, connection->getNBEdgeConnection());
                std::string responseString = node->getResponse(linkIndex);
                std::reverse(responseString.begin(), responseString.end());
                // determine the prohibition status
                bool foes = ((int)currentFoesString.size() > linkIndex) && (currentFoesString[linkIndex] == '1');
                bool forbids = ((int)responseString.size() > currentLinkIndex) && (responseString[currentLinkIndex] == '1');
                bool forbidden = ((int)currentResponseString.size() > linkIndex) && (currentResponseString[linkIndex] == '1');
                // insert in myConcernedConns
                myConcernedConns.insert(connection);
                // change color depending of prohibition status
                if (!foes) {
                    connection->setSpecialColor(&myLegend->getUndefinedColor());
                } else {
                    if (forbids && forbidden) {
                        connection->setSpecialColor(&myLegend->getMutualConflictColor());
                    } else if (forbids) {
                        connection->setSpecialColor(&myLegend->getProhibitedColor());
                    } else if (forbidden) {
                        connection->setSpecialColor(&myLegend->getProhibitingColor());
                    } else {
                        connection->setSpecialColor(&myLegend->getUnregulatedConflictColor());
                    }
                }
            }
        }
        // update description
        myRelativeToConnection->updateDescription();
    }
}


/****************************************************************************/
