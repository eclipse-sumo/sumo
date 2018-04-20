/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDialogACChooser.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Apr 2018
/// @version $Id$
///
// Class for the window that allows to choose a street, junction or vehicle
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <fxkeys.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUIGlChildWindow.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/globjects/GUIGlObject_AbstractAdd.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEAttributeCarrier.h>
#include <netedit/GNEViewParent.h>

#include "GNEDialogACChooser.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GNEDialogACChooser) GNEDialogACChooserMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSER_CENTER, GNEDialogACChooser::onCmdCenter),
    FXMAPFUNC(SEL_COMMAND,  MID_CANCEL,         GNEDialogACChooser::onCmdClose),
    FXMAPFUNC(SEL_CHANGED,  MID_CHOOSER_TEXT,   GNEDialogACChooser::onChgText),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSER_TEXT,   GNEDialogACChooser::onCmdText),
    FXMAPFUNC(SEL_KEYPRESS, MID_CHOOSER_LIST,   GNEDialogACChooser::onListKeyPress),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSER_FILTER, GNEDialogACChooser::onCmdToogleShowOnlySelected),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_INVERT, GNEDialogACChooser::onCmdToggleSelection),
};

FXIMPLEMENT(GNEDialogACChooser, FXMainWindow, GNEDialogACChooserMap, ARRAYNUMBER(GNEDialogACChooserMap))

// ===========================================================================
// method definitions
// ===========================================================================

GNEDialogACChooser::GNEDialogACChooser(GNEViewParent* viewParent, FXIcon* icon, const std::string& title, const std::vector<GNEAttributeCarrier*>& ACs):
    FXMainWindow(viewParent->getApp(), title.c_str(), icon, NULL, GUIDesignChooserDialog),
    myViewParent(viewParent),
    myShowOnlySelectedElements(false) {
    FXHorizontalFrame* hbox = new FXHorizontalFrame(this, GUIDesignAuxiliarFrame);
    // build the list
    FXVerticalFrame* layoutLeft = new FXVerticalFrame(hbox, GUIDesignChooserLayoutLeft);
    myTextEntry = new FXTextField(layoutLeft, 0, this, MID_CHOOSER_TEXT, GUIDesignChooserTextField);
    FXVerticalFrame* layoutList = new FXVerticalFrame(layoutLeft, GUIDesignChooserLayoutList);
    myList = new FXList(layoutList, this, MID_CHOOSER_LIST, GUIDesignChooserListSingle);
    // first fill myACsByID to sort ACs by tags
    for (auto i : ACs) {
        myACsByID.insert(std::pair<std::string, GNEAttributeCarrier*>(i->getID(), i));
    }
    // iterate over ACsByID and fill list
    for (auto i : myACsByID) {
        // set icon
        FXIcon* selectIcon = i.second->isAttributeCarrierSelected() ? GUIIconSubSys::getIcon(ICON_FLAG) : nullptr;
        myACs[myList->appendItem(i.first.c_str(), selectIcon)] = i.second;
    }
    // build the buttons
    FXVerticalFrame* layoutRight = new FXVerticalFrame(hbox, GUIDesignChooserLayoutRight);
    myCenterButton = new FXButton(layoutRight, "Center\t\t", GUIIconSubSys::getIcon(ICON_RECENTERVIEW), this, MID_CHOOSER_CENTER, GUIDesignChooserButtons);
    new FXHorizontalSeparator(layoutRight, GUIDesignHorizontalSeparator);
    myHideUnselectedButton = new FXButton(layoutRight, "&Hide Unselected\t\t", GUIIconSubSys::getIcon(ICON_FLAG), this, MID_CHOOSER_FILTER, GUIDesignChooserButtons);
    myToogleSelectionButton = new FXButton(layoutRight, "&Select/deselect\tSelect/deselect current object\t", GUIIconSubSys::getIcon(ICON_FLAG), this, MID_CHOOSEN_INVERT, GUIDesignChooserButtons);
    new FXHorizontalSeparator(layoutRight, GUIDesignHorizontalSeparator);
    new FXButton(layoutRight, "&Close\t\t", GUIIconSubSys::getIcon(ICON_NO), this, MID_CANCEL, GUIDesignChooserButtons);
    // create and show dialog
    create();
    show();
}


GNEDialogACChooser::~GNEDialogACChooser() {
    myViewParent->eraseACChooserDialog(this);
}


void 
GNEDialogACChooser::refreshACChooser(const std::vector<GNEAttributeCarrier*>& ACs) {
    // clear myACsByID
    myACsByID.clear();
    // first fill myACsByID to sort ACs by tags
    for (auto i : ACs) {
        myACsByID.insert(std::pair<std::string, GNEAttributeCarrier*>(i->getID(), i));
    }
    // clear list and myACs
    myList->clearItems();
    myACs.clear();
    // iterate over ACsByID and fill list
    for (auto i : myACsByID) {
        // set icon
        if(!myShowOnlySelectedElements || i.second->isAttributeCarrierSelected()) {
            FXIcon* selectIcon = i.second->isAttributeCarrierSelected() ? GUIIconSubSys::getIcon(ICON_FLAG) : 0;
            myACs[myList->appendItem(i.first.c_str(), selectIcon)] = i.second;
        }
    }
}


void
GNEDialogACChooser::show() {
    FXMainWindow::show();
    myTextEntry->setFocus();
}


long
GNEDialogACChooser::onCmdCenter(FXObject*, FXSelector, void*) {
    int selected = myList->getCurrentItem();
    if (selected >= 0) {
        myViewParent->setView(dynamic_cast<GUIGlObject*>(myACs[selected])->getGlID());
    }
    return 1;
}


long
GNEDialogACChooser::onCmdClose(FXObject*, FXSelector, void*) {
    close(true);
    return 1;
}


long
GNEDialogACChooser::onChgText(FXObject*, FXSelector, void*) {
    int id = myList->findItem(myTextEntry->getText(), -1, SEARCH_PREFIX);
    if (id < 0) {
        if (myList->getNumItems() > 0) {
            myList->deselectItem(myList->getCurrentItem());
        }
        myCenterButton->disable();
        return 1;
    }
    myList->deselectItem(myList->getCurrentItem());
    myList->makeItemVisible(id);
    myList->selectItem(id);
    myList->setCurrentItem(id, true);
    myCenterButton->enable();
    return 1;
}


long
GNEDialogACChooser::onCmdText(FXObject*, FXSelector, void*) {
    int current = myList->getCurrentItem();
    if (current >= 0 && myList->isItemSelected(current)) {
        myViewParent->setView(dynamic_cast<GUIGlObject*>(myACs[current])->getGlID());
    }
    return 1;
}


long
GNEDialogACChooser::onListKeyPress(FXObject*, FXSelector, void* ptr) {
    FXEvent* event = (FXEvent*)ptr;
    switch (event->code) {
        case KEY_Return:
            onCmdText(0, 0, 0);
            break;
        default:
            break;
    }
    return 1;
}


long
GNEDialogACChooser::onCmdToogleShowOnlySelected(FXObject*, FXSelector, void*) {
    std::set<std::pair<std::string, GNEAttributeCarrier*> > ACsToShow;
    // iterate over myACsByID to check what has the selected flag
    if(myShowOnlySelectedElements) {
        // get all ACs 
        for (auto i : myACsByID) {
            ACsToShow.insert(std::pair<std::string, GNEAttributeCarrier*>(i.second->getID(), i.second));
        }
        myShowOnlySelectedElements = false;
        myHideUnselectedButton->setText("&Hide unselected\t\t");
    } else {
        // get only selected ACs (with flag) 
        for (auto i : myACsByID) {
            if (i.second->isAttributeCarrierSelected()) {
                ACsToShow.insert(std::pair<std::string, GNEAttributeCarrier*>(i.second->getID(), i.second));
            }
        }
        myShowOnlySelectedElements = true;
        myHideUnselectedButton->setText("S&how all\t\t");
    }
    // clear list and myACs
    myList->clearItems();
    myACs.clear();
    // iterate over ACsByID and fill list again only with the selected elements
    for (auto i : ACsToShow) {
        // set icon
        FXIcon* selectIcon = GNEAttributeCarrier::parse<bool>(i.second->getAttribute(GNE_ATTR_SELECTED)) ? GUIIconSubSys::getIcon(ICON_FLAG) : 0;
        myACs[myList->appendItem(i.first.c_str(), selectIcon)] = i.second;
    }
    myList->update();
    return 1;
}


long
GNEDialogACChooser::onCmdToggleSelection(FXObject*, FXSelector, void*) {
    FXIcon* flag = GUIIconSubSys::getIcon(ICON_FLAG);
    int i = myList->getCurrentItem();
    if (i >= 0 && myList->getItem(i)->isSelected()) {
        // select or unselect attribute carrier without possibility of undo
        if(myACs[i]->isAttributeCarrierSelected()) {
            myACs[i]->unselectAttributeCarrier();
            myList->setItemIcon(i, 0);
        } else {
            myACs[i]->selectAttributeCarrier();
            myList->setItemIcon(i, flag);
        }
    }
    myList->update();
    myViewParent->getView()->update();
    return 1;
}


/****************************************************************************/

