/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    GUIDialog_ChooserAbstract.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
///
// Class for the window that allows to choose a street, junction or vehicle
/****************************************************************************/
#include <config.h>

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

#include "GUIDialog_ChooserAbstract.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GUIDialog_ChooserAbstract) GUIDialog_ChooserAbstractMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSER_CENTER,         GUIDialog_ChooserAbstract::onCmdCenter),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSER_TRACK,          GUIDialog_ChooserAbstract::onCmdTrack),
    FXMAPFUNC(SEL_COMMAND,  MID_CANCEL,                 GUIDialog_ChooserAbstract::onCmdClose),
    FXMAPFUNC(SEL_CHANGED,  MID_CHOOSER_TEXT,           GUIDialog_ChooserAbstract::onChgText),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSER_TEXT,           GUIDialog_ChooserAbstract::onCmdText),
    FXMAPFUNC(SEL_KEYPRESS, MID_CHOOSER_LIST,           GUIDialog_ChooserAbstract::onListKeyPress),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSER_FILTER,         GUIDialog_ChooserAbstract::onCmdFilter),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSER_FILTER_SUBSTR,  GUIDialog_ChooserAbstract::onCmdFilterSubstr),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_INVERT,         GUIDialog_ChooserAbstract::onCmdToggleSelection),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_NAME,           GUIDialog_ChooserAbstract::onCmdLocateByName),
    FXMAPFUNC(SEL_COMMAND,  MID_UPDATE,                 GUIDialog_ChooserAbstract::onCmdUpdate),
};

FXIMPLEMENT(GUIDialog_ChooserAbstract, FXMainWindow, GUIDialog_ChooserAbstractMap, ARRAYNUMBER(GUIDialog_ChooserAbstractMap))


// ===========================================================================
// method definitions
// ===========================================================================
GUIDialog_ChooserAbstract::GUIDialog_ChooserAbstract(GUIGlChildWindow* windowsParent, int messageId,
        FXIcon* icon, const FXString& title, const std::vector<GUIGlID>& ids, GUIGlObjectStorage& /*glStorage*/) :
    FXMainWindow(windowsParent->getApp(), title, icon, nullptr, GUIDesignChooserDialog),
    myWindowsParent(windowsParent),
    myMessageId(messageId),
    myLocateByName(false),
    myHaveFilteredSubstring(false) {
    FXHorizontalFrame* hbox = new FXHorizontalFrame(this, GUIDesignAuxiliarFrame);
    // build the list
    FXVerticalFrame* layoutLeft = new FXVerticalFrame(hbox, GUIDesignChooserLayoutLeft);
    myTextEntry = new FXTextField(layoutLeft, 0, this, MID_CHOOSER_TEXT, GUIDesignChooserTextField);
    FXVerticalFrame* layoutList = new FXVerticalFrame(layoutLeft, GUIDesignChooserLayoutList);
    myList = new FXList(layoutList, this, MID_CHOOSER_LIST, GUIDesignChooserListSingle);
    refreshList(ids);
    // build the buttons
    FXVerticalFrame* layoutRight = new FXVerticalFrame(hbox, GUIDesignChooserLayoutRight);
    myCenterButton = new FXButton(layoutRight, "Center\t\t", GUIIconSubSys::getIcon(GUIIcon::RECENTERVIEW), this, MID_CHOOSER_CENTER, GUIDesignChooserButtons);
    myTrackButton = new FXButton(layoutRight, "Track\t\t", GUIIconSubSys::getIcon(GUIIcon::RECENTERVIEW), this, MID_CHOOSER_TRACK, GUIDesignChooserButtons);
    // only enable Track Button if we're locating vehicles
    if (title.text() != std::string("Vehicle Chooser")) {
        myTrackButton->disable();
        myTrackButton->hide();
    }
    new FXHorizontalSeparator(layoutRight, GUIDesignHorizontalSeparator);
    new FXButton(layoutRight, "&Hide Unselected\t\t", GUIIconSubSys::getIcon(GUIIcon::FLAG), this, MID_CHOOSER_FILTER, GUIDesignChooserButtons);
    new FXButton(layoutRight, "&Filter substring\t\t", nullptr, this, MID_CHOOSER_FILTER_SUBSTR, GUIDesignChooserButtons);
    new FXButton(layoutRight, "&Select/deselect\t\tSelect/deselect current object", GUIIconSubSys::getIcon(GUIIcon::FLAG), this, MID_CHOOSEN_INVERT, GUIDesignChooserButtons);
    new FXButton(layoutRight, "By &Name\tLocate item by name\t", nullptr, this, MID_CHOOSEN_NAME, GUIDesignChooserButtons);
    new FXButton(layoutRight, "&Update\t\tReload all ids", GUIIconSubSys::getIcon(GUIIcon::RELOAD), this, MID_UPDATE, GUIDesignChooserButtons);
    new FXHorizontalSeparator(layoutRight, GUIDesignHorizontalSeparator);
    new FXButton(layoutRight, "&Close\t\t", GUIIconSubSys::getIcon(GUIIcon::NO), this, MID_CANCEL, GUIDesignChooserButtons);
    // add child in windowsParent
    myWindowsParent->getParent()->addChild(this);
    // create and show dialog
    create();
    show();
}


GUIDialog_ChooserAbstract::~GUIDialog_ChooserAbstract() {
    // remove child from windowsParent
    myWindowsParent->getParent()->removeChild(this);
}


GUIGlObject*
GUIDialog_ChooserAbstract::getObject() const {
    return static_cast<GUIGlObject*>(mySelected);
}


void
GUIDialog_ChooserAbstract::show() {
    FXMainWindow::show();
    myTextEntry->setFocus();
}


long
GUIDialog_ChooserAbstract::onCmdCenter(FXObject*, FXSelector, void*) {
    int selected = myList->getCurrentItem();
    if (selected >= 0) {
        myWindowsParent->getView()->stopTrack();
        myWindowsParent->setView(*static_cast<GUIGlID*>(myList->getItemData(selected)));
    }
    return 1;
}


long
GUIDialog_ChooserAbstract::onCmdTrack(FXObject*, FXSelector, void*) {
    int selected = myList->getCurrentItem();
    if (selected >= 0) {
        myWindowsParent->setView(*static_cast<GUIGlID*>(myList->getItemData(selected)));
        GUIGlID id = *static_cast<GUIGlID*>(myList->getItemData(selected));
        GUIGlObject* o = GUIGlObjectStorage::gIDStorage.getObjectBlocking(id);
        if (o->getType() == GLO_VEHICLE) {
            myWindowsParent->getView()->startTrack(o->getGlID());
        }
        GUIGlObjectStorage::gIDStorage.unblockObject(id);
    }
    return 1;
}


long
GUIDialog_ChooserAbstract::onCmdClose(FXObject*, FXSelector, void*) {
    close(true);
    return 1;
}


long
GUIDialog_ChooserAbstract::onChgText(FXObject*, FXSelector, void*) {
    int id = -1;
    if (myLocateByName || myHaveFilteredSubstring) {
        // findItem does not support substring search
        const int numItems = myList->getNumItems();
        FXString t = myTextEntry->getText().lower();
        for (int i = 0; i < numItems; i++) {
            if (myList->getItemText(i).lower().find(t) >= 0) {
                id = i;
                break;
            }
        }
    } else {
        id = myList->findItem(myTextEntry->getText(), -1, SEARCH_PREFIX);
    }
    if (id < 0) {
        if (myList->getNumItems() > 0) {
            myList->deselectItem(myList->getCurrentItem());
        }
        myCenterButton->disable();
        myTrackButton->disable();
        return 1;
    }
    myList->deselectItem(myList->getCurrentItem());
    myList->makeItemVisible(id);
    myList->selectItem(id);
    myList->setCurrentItem(id, true);
    myCenterButton->enable();
    myTrackButton->enable();
    return 1;
}


long
GUIDialog_ChooserAbstract::onCmdText(FXObject*, FXSelector, void*) {
    int current = myList->getCurrentItem();
    if (current >= 0 && myList->isItemSelected(current)) {
        myWindowsParent->setView(*static_cast<GUIGlID*>(myList->getItemData(current)));
    }
    return 1;
}



long
GUIDialog_ChooserAbstract::onListKeyPress(FXObject*, FXSelector, void* ptr) {
    FXEvent* event = (FXEvent*)ptr;
    switch (event->code) {
        case KEY_Return:
            onCmdText(nullptr, 0, nullptr);
            break;
        default:
            break;
    }
    return 1;
}


long
GUIDialog_ChooserAbstract::onCmdFilter(FXObject*, FXSelector, void*) {
    FXIcon* flag = GUIIconSubSys::getIcon(GUIIcon::FLAG);
    std::vector<GUIGlID> selectedGlIDs;
    const int numItems = myList->getNumItems();
    for (int i = 0; i < numItems; i++) {
        const GUIGlID glID = *static_cast<GUIGlID*>(myList->getItemData(i));
        if (myList->getItemIcon(i) == flag) {
            selectedGlIDs.push_back(glID);
        }
    }
    refreshList(selectedGlIDs);
    return 1;
}


long
GUIDialog_ChooserAbstract::onCmdFilterSubstr(FXObject*, FXSelector, void*) {
    std::vector<GUIGlID> selectedGlIDs;
    const int numItems = myList->getNumItems();
    FXString t = myTextEntry->getText().lower();
    for (int i = 0; i < numItems; i++) {
        if (myList->getItemText(i).lower().find(t) >= 0) {
            const GUIGlID glID = *static_cast<GUIGlID*>(myList->getItemData(i));
            selectedGlIDs.push_back(glID);
        }
    }
    refreshList(selectedGlIDs);
    // filter ACs in NETEDIT
    filterACs(selectedGlIDs);
    myHaveFilteredSubstring = true;
    onChgText(nullptr, 0, nullptr);
    return 1;
}


std::string
GUIDialog_ChooserAbstract::getObjectName(GUIGlObject* o) const {
    if (myLocateByName) {
        return o->getOptionalName();
    } else {
        return o->getMicrosimID();
    }
}

void
GUIDialog_ChooserAbstract::refreshList(const std::vector<GUIGlID>& ids) {
    myList->clearItems();
    for (auto i : ids) {
        GUIGlObject* o = GUIGlObjectStorage::gIDStorage.getObjectBlocking(i);
        if (o == nullptr) {
            continue;
        }
        const std::string& name = getObjectName(o);
        const bool selected = myWindowsParent->isSelected(o);
        FXIcon* const ico = selected ? GUIIconSubSys::getIcon(GUIIcon::FLAG) : nullptr;
        myIDs.insert(o->getGlID());
        myList->appendItem(name.c_str(), ico, (void*) & (*myIDs.find(o->getGlID())));
        GUIGlObjectStorage::gIDStorage.unblockObject(i);
    }
    myList->update();
}


long
GUIDialog_ChooserAbstract::onCmdToggleSelection(FXObject*, FXSelector, void*) {
    FXIcon* flag = GUIIconSubSys::getIcon(GUIIcon::FLAG);
    int i = myList->getCurrentItem();
    if (i >= 0) {
        toggleSelection(i);
        if (myList->getItemIcon(i) == flag) {
            myList->setItemIcon(i, nullptr);
        } else {
            myList->setItemIcon(i, flag);
        }
    }
    myList->update();
    myWindowsParent->getView()->update();
    return 1;
}


long
GUIDialog_ChooserAbstract::onCmdLocateByName(FXObject*, FXSelector, void*) {
    std::vector<std::pair<std::string, GUIGlID> > namesAndIDs;
    myLocateByName = true;
    const int numItems = myList->getNumItems();
    for (int i = 0; i < numItems; i++) {
        GUIGlID glID = *static_cast<GUIGlID*>(myList->getItemData(i));
        GUIGlObject* o = GUIGlObjectStorage::gIDStorage.getObjectBlocking(glID);
        if (o != 0) {
            const std::string& name = getObjectName(o);
            if (name != "") {
                namesAndIDs.push_back(std::make_pair(name, glID));
            }
        }
        GUIGlObjectStorage::gIDStorage.unblockObject(glID);
    }
    std::sort(namesAndIDs.begin(), namesAndIDs.end());
    std::vector<GUIGlID> selectedGlIDs;
    for (const auto& item : namesAndIDs) {
        selectedGlIDs.push_back(item.second);
    }
    refreshList(selectedGlIDs);
    myTextEntry->setFocus();
    return 1;
}

long
GUIDialog_ChooserAbstract::onCmdUpdate(FXObject*, FXSelector, void*) {
    refreshList(myWindowsParent->getObjectIDs(myMessageId));
    return 1;
}

void
GUIDialog_ChooserAbstract::toggleSelection(int listIndex) {
    GUIGlID* glID = static_cast<GUIGlID*>(myList->getItemData(listIndex));
    gSelected.toggleSelection(*glID);
}


void
GUIDialog_ChooserAbstract::filterACs(const std::vector<GUIGlID>& /*GLIDs*/) {
    // overrided in GNEDialogACChooser
}

/****************************************************************************/
