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
/// @file    GNEOverlappedInspection.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2022
///
// Frame for overlapped elements
/****************************************************************************/

#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNETagProperties.h>
#include <netedit/dialogs/basic/GNEHelpBasicDialog.h>
#include <netedit/elements/network/GNELane.h>
#include <netedit/frames/common/GNEInspectorFrame.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNEOverlappedInspection.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEOverlappedInspection) OverlappedInspectionMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_OVERLAPPED_NEXT,            GNEOverlappedInspection::onCmdInspectNextElement),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_OVERLAPPED_PREVIOUS,        GNEOverlappedInspection::onCmdInspectPreviousElement),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_OVERLAPPED_SHOWLIST,        GNEOverlappedInspection::onCmdShowList),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_OVERLAPPED_ITEMSELECTED,    GNEOverlappedInspection::onCmdListItemSelected),
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,                           GNEOverlappedInspection::onCmdOverlappingHelp)
};

// Object implementation
FXIMPLEMENT(GNEOverlappedInspection,       MFXGroupBoxModule,     OverlappedInspectionMap,        ARRAYNUMBER(OverlappedInspectionMap))

// ===========================================================================
// method definitions
// ===========================================================================

GNEOverlappedInspection::GNEOverlappedInspection(GNEFrame* frameParent, const bool onlyJunctions) :
    MFXGroupBoxModule(frameParent, onlyJunctions ? TL("Overlapped junctions") : TL("Overlapped elements")),
    myFrameParent(frameParent),
    myOnlyJunctions(onlyJunctions) {
    FXHorizontalFrame* frameButtons = new FXHorizontalFrame(getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame);
    // Create previous Item Button
    myPreviousElement = GUIDesigns::buildFXButton(frameButtons, "", "", "", GUIIconSubSys::getIcon(GUIIcon::BIGARROWLEFT), this, MID_GNE_OVERLAPPED_PREVIOUS, GUIDesignButtonRectangular);
    // create current index button
    myCurrentIndexButton = GUIDesigns::buildFXButton(frameButtons, "", "", "", nullptr, this, MID_GNE_OVERLAPPED_SHOWLIST, GUIDesignButton);
    // Create next Item Button
    myNextElement = GUIDesigns::buildFXButton(frameButtons, "", "", "", GUIIconSubSys::getIcon(GUIIcon::BIGARROWRIGHT), this, MID_GNE_OVERLAPPED_NEXT, GUIDesignButtonRectangular);
    // Create list of overlapped elements (by default hidden)
    myOverlappedElementList = new FXList(getCollapsableFrame(), this, MID_GNE_OVERLAPPED_ITEMSELECTED, GUIDesignListFixedHeight);
    // by default list of overlapped elements is hidden)
    myOverlappedElementList->hide();
    // Create help button
    myHelpButton = GUIDesigns::buildFXButton(getCollapsableFrame(), TL("Help"), "", "", nullptr, this, MID_HELP, GUIDesignButtonRectangular);
    // by default hidden
    hide();
}


GNEOverlappedInspection::~GNEOverlappedInspection() {}


void
GNEOverlappedInspection::showOverlappedInspection(GNEViewNetHelper::ViewObjectsSelector& viewObjects, const Position& clickedPosition, const bool shiftKeyPressed) {
    // check if filter all except junctions
    if (myOnlyJunctions) {
        viewObjects.filterAllExcept(GLO_JUNCTION);
    } else {
        // filter by supermode
        viewObjects.filterBySuperMode();
        // filtger edges if we clicked over a lane
        if (viewObjects.getAttributeCarrierFront() && viewObjects.getAttributeCarrierFront() == viewObjects.getLaneFront()) {
            viewObjects.filterEdges();
        }
    }
    // check if previously we clicked an edge and now we want to inspect their lane
    bool toogleInspectEdgeLane = false;
    if (!myOnlyJunctions && (myOverlappedACs.size() > 0) && (myOverlappedACs.front()->getTagProperty()->getTag() == SUMO_TAG_EDGE) && shiftKeyPressed) {
        toogleInspectEdgeLane = true;
    }
    // in this point, check if we want to iterate over existent overlapped inspection, or we want to inspet a new set of elements
    if (!toogleInspectEdgeLane && (myOverlappedACs.size() > 0) && (myClickedPosition != Position::INVALID) && (myClickedPosition.distanceSquaredTo(clickedPosition) < 0.05) && (myShiftKeyPressed == shiftKeyPressed)) {
        onCmdInspectNextElement(nullptr, 0, nullptr);
    } else {
        myOverlappedACs = viewObjects.getAttributeCarriers();
        myItemIndex = 0;
        myOverlappedElementList->hide();
    }
    // update clicked position and refresh overlapped inspection
    myClickedPosition = clickedPosition;
    myShiftKeyPressed = shiftKeyPressed;
    refreshOverlappedInspection();
}


void
GNEOverlappedInspection::clearOverlappedInspection() {
    myOverlappedACs.clear();
    myItemIndex = 0;
    myOverlappedElementList->hide();
    refreshOverlappedInspection();
}


void
GNEOverlappedInspection::hiderOverlappedInspection() {
    hide();
}

void
GNEOverlappedInspection::refreshOverlappedInspection() {
    // show modul depending of number of overlapped elements
    if (myOverlappedACs.size() > 1) {
        // update text of current index button
        myCurrentIndexButton->setText((toString(myItemIndex + 1) + " / " + toString(myOverlappedACs.size())).c_str());
        // clear and fill list again
        myOverlappedElementList->clearItems();
        for (int i = 0; i < (int)myOverlappedACs.size(); i++) {
            myOverlappedElementList->insertItem(i, myOverlappedACs.at(i)->getID().c_str(), myOverlappedACs.at(i)->getACIcon());
        }
        // select current item
        myOverlappedElementList->getItem(myItemIndex)->setSelected(TRUE);
        // show modul
        show();
        // call selectedOverlappedElement
        myFrameParent->selectedOverlappedElement(myOverlappedACs.at(myItemIndex));
    } else {
        if (myOverlappedACs.size() > 0) {
            myFrameParent->selectedOverlappedElement(myOverlappedACs.front());
        } else {
            myFrameParent->selectedOverlappedElement(nullptr);
        }
        hide();
    }
}


bool
GNEOverlappedInspection::overlappedInspectionShown() const {
    // show GNEOverlappedInspection modul
    return shown();
}


int
GNEOverlappedInspection::getNumberOfOverlappedACs() const {
    return (int)myOverlappedACs.size();
}


GNEAttributeCarrier*
GNEOverlappedInspection::getCurrentAC() const {
    if (myOverlappedACs.size() > 0) {
        return myOverlappedACs.at(myItemIndex);
    } else {
        return nullptr;
    }
}

long
GNEOverlappedInspection::onCmdInspectPreviousElement(FXObject*, FXSelector, void*) {
    // check if there is items
    if (myOverlappedElementList->getNumItems() > 0) {
        // set index (it works as a ring)
        if (myItemIndex > 0) {
            myItemIndex--;
        } else {
            myItemIndex = ((int)myOverlappedACs.size() - 1);
        }
        refreshOverlappedInspection();
    }
    return 1;
}


long
GNEOverlappedInspection::onCmdInspectNextElement(FXObject*, FXSelector, void*) {
    // check if there is items
    if (myOverlappedElementList->getNumItems() > 0) {
        // set index (it works as a ring)
        myItemIndex = (myItemIndex + 1) % myOverlappedACs.size();
        refreshOverlappedInspection();
    }
    return 1;
}


long
GNEOverlappedInspection::onCmdShowList(FXObject*, FXSelector, void*) {
    // show or hide element list
    if (myOverlappedElementList->shown()) {
        myOverlappedElementList->hide();
    } else {
        myOverlappedElementList->show();
    }
    if (myOverlappedElementList->getNumItems() <= 10) {
        myOverlappedElementList->setHeight(23 * myOverlappedElementList->getNumItems());
    } else {
        myOverlappedElementList->setHeight(230);
    }
    myOverlappedElementList->recalc();
    // recalc and update frame
    recalc();
    return 1;
}

long
GNEOverlappedInspection::onCmdListItemSelected(FXObject*, FXSelector, void*) {
    for (int i = 0; i < myOverlappedElementList->getNumItems(); i++) {
        if (myOverlappedElementList->getItem(i)->isSelected()) {
            myItemIndex = i;
            refreshOverlappedInspection();
            return 1;
        }
    }
    return 0;
}


long
GNEOverlappedInspection::onCmdOverlappingHelp(FXObject*, FXSelector, void*) {
    // create text for help dialog
    std::ostringstream help;
    help
            << TL(" - Click in the same position") << "\n"
            << TL("   to inspect next element") << "\n"
            << TL(" - Shift + Click in the same") << "\n"
            << TL("   position to inspect") << "\n"
            << TL("   previous element");
    // create help dialog
    GNEHelpBasicDialog(myFrameParent->getViewNet()->getViewParent()->getGNEAppWindows(),
                       myFrameParent->getViewNet()->getViewParent()->getGNEAppWindows(),
                       TL("GEO attributes Help"), help);
    return 1;
}


GNEOverlappedInspection::GNEOverlappedInspection() {}

/****************************************************************************/
