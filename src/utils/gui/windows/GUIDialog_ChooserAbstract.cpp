/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
#include <utils/common/MsgHandler.h>
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
    FXMAPFUNC(SEL_KEYPRESS, 0,                          GUIDialog_ChooserAbstract::onKeyPress),
    FXMAPFUNC(SEL_CHANGED,  MID_CHOOSER_LIST,           GUIDialog_ChooserAbstract::onChgList),
    FXMAPFUNC(SEL_DESELECTED, MID_CHOOSER_LIST,         GUIDialog_ChooserAbstract::onChgListSel),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSER_FILTER,         GUIDialog_ChooserAbstract::onCmdFilter),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSER_FILTER_SUBSTR,  GUIDialog_ChooserAbstract::onCmdFilterSubstr),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_INVERT,         GUIDialog_ChooserAbstract::onCmdToggleSelection),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_SELECT,         GUIDialog_ChooserAbstract::onCmdAddListSelection),
    FXMAPFUNC(SEL_UPDATE,   MID_CHOOSEN_SELECT,         GUIDialog_ChooserAbstract::onUpdAddListSelection),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_CLEAR,          GUIDialog_ChooserAbstract::onCmdClearListSelection),
    FXMAPFUNC(SEL_UPDATE,   MID_CHOOSEN_CLEAR,          GUIDialog_ChooserAbstract::onUpdClearListSelection),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_NAME,           GUIDialog_ChooserAbstract::onCmdLocateByName),
    FXMAPFUNC(SEL_COMMAND,  MID_UPDATE,                 GUIDialog_ChooserAbstract::onCmdUpdate),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSER_CASESENSITIVE,  GUIDialog_ChooserAbstract::onCmdCaseSensitive),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSER_AUTOCENTER,     GUIDialog_ChooserAbstract::onCmdAutoCenter),
};

FXIMPLEMENT(GUIDialog_ChooserAbstract, FXMainWindow, GUIDialog_ChooserAbstractMap, ARRAYNUMBER(GUIDialog_ChooserAbstractMap))


// ===========================================================================
// method definitions
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4355) // mask warning about "this" in initializers
#endif
GUIDialog_ChooserAbstract::GUIDialog_ChooserAbstract(GUIGlChildWindow* windowsParent, int messageId,
        FXIcon* icon, const FXString& title, const std::vector<GUIGlID>& ids, GUIGlObjectStorage& /*glStorage*/) :
    FXMainWindow(windowsParent->getApp(), title, icon, nullptr, GUIDesignChooserDialog),
    GUIPersistentWindowPos(this, "LOCATOR", true, 20, 40, 300, 350),
    myWindowsParent(windowsParent),
    myMessageId(messageId),
    myList(nullptr),
    myCenterButton(nullptr),
    myTrackButton(nullptr),
    mySelectAllButton(nullptr),
    myDeselectAllButton(nullptr),
    mySelected(nullptr),
    myTextEntry(nullptr),
    myNumSelectedItems(0),
    myHideUnselected(nullptr),
    myLocateByName(nullptr),
    myFilterSubstring(nullptr),
    myCountLabel(nullptr),
    myCaseSensitive(nullptr),
    myInstantCenter(nullptr) {
    FXHorizontalFrame* hbox = new FXHorizontalFrame(this, GUIDesignAuxiliarFrame);
    // build the list
    FXVerticalFrame* layoutLeft = new FXVerticalFrame(hbox, GUIDesignChooserLayoutLeft);
    myTextEntry = new FXTextField(layoutLeft, 0, this, MID_CHOOSER_TEXT, TEXTFIELD_ENTER_ONLY | GUIDesignChooserTextField);
    FXVerticalFrame* layoutList = new FXVerticalFrame(layoutLeft, GUIDesignChooserLayoutList);
    myList = new FXList(layoutList, this, MID_CHOOSER_LIST, GUIDesignChooserListSingle);
    // build the buttons
    FXVerticalFrame* layoutRight = new FXVerticalFrame(hbox, GUIDesignChooserLayoutRight);
    myCenterButton = GUIDesigns::buildFXButton(layoutRight, TL("Center"), "", "", GUIIconSubSys::getIcon(GUIIcon::RECENTERVIEW), this, MID_CHOOSER_CENTER, GUIDesignChooserButtons);
    myTrackButton = GUIDesigns::buildFXButton(layoutRight, TL("Track"), "", "", GUIIconSubSys::getIcon(GUIIcon::RECENTERVIEW), this, MID_CHOOSER_TRACK, GUIDesignChooserButtons);
    // only enable Track Button if we're locating vehicles
    if (title.text() != std::string(TL("Vehicle Chooser"))) {
        myTrackButton->disable();
        myTrackButton->hide();
    }
    new FXHorizontalSeparator(layoutRight, GUIDesignHorizontalSeparator);
    GUIDesigns::buildFXButton(layoutRight, TL("&Select/deselect"), "", TL("Select/deselect current object"), GUIIconSubSys::getIcon(GUIIcon::FLAG), this, MID_CHOOSEN_INVERT, GUIDesignChooserButtons);
    mySelectAllButton = GUIDesigns::buildFXButton(layoutRight, TL("Select &all"), "", TL("Select all items in list"), GUIIconSubSys::getIcon(GUIIcon::FLAG), this, MID_CHOOSEN_SELECT, GUIDesignChooserButtons);
    myDeselectAllButton = GUIDesigns::buildFXButton(layoutRight, TL("&Deselect all"), "", TL("Deselect all items in list"), GUIIconSubSys::getIcon(GUIIcon::FLAG), this, MID_CHOOSEN_CLEAR, GUIDesignChooserButtons);
    GUIDesigns::buildFXButton(layoutRight, TL("&Update"), "", TL("Reload all ids"), GUIIconSubSys::getIcon(GUIIcon::RELOAD), this, MID_UPDATE, GUIDesignChooserButtons);
    new FXHorizontalSeparator(layoutRight, GUIDesignHorizontalSeparator);
    GUIDesigns::buildFXButton(layoutRight, TL("&Close"), "", "", GUIIconSubSys::getIcon(GUIIcon::NO), this, MID_CANCEL, GUIDesignChooserButtons);
    new FXHorizontalSeparator(layoutRight, GUIDesignHorizontalSeparator);
    myHideUnselected = new FXCheckButton(layoutRight, TL("&Hide Unselected"), this, MID_CHOOSER_FILTER);
    myHideUnselected->setTipText(TL("Hide unselected items"));
    myHideUnselected->setHelpText(TL("Hide unselected items"));
    myLocateByName = new FXCheckButton(layoutRight, TL("By &Name"), this, MID_CHOOSEN_NAME);
    myLocateByName->setTipText(TL("Locate item by name"));
    myLocateByName->setHelpText(TL("Locate item by name"));
    myFilterSubstring = new FXCheckButton(layoutRight, TL("&Filter substring"), this, MID_CHOOSER_FILTER_SUBSTR);
    myFilterSubstring->setTipText(TL("Filter list by substring"));
    myFilterSubstring->setHelpText(TL("Filter list by substring"));
    myCaseSensitive = new FXCheckButton(layoutRight, TL("case-sensitive search"), this, MID_CHOOSER_CASESENSITIVE);
    myCaseSensitive->setCheck(getApp()->reg().readIntEntry("LOCATOR", "caseSensitive", 0) == 1);
    myInstantCenter = new FXCheckButton(layoutRight, TL("auto-center"), this, MID_CHOOSER_AUTOCENTER);
    myInstantCenter->setCheck(getApp()->reg().readIntEntry("LOCATOR", "autoCenter", 0) == 1);
    myCountLabel = new FXLabel(layoutRight, "placeholder", nullptr, LAYOUT_BOTTOM | LAYOUT_FILL_X | JUSTIFY_LEFT);
    refreshList(ids);
    // add child in windowsParent
    myWindowsParent->getGUIMainWindowParent()->addChild(this);
    loadWindowPos();
    // create and show dialog
    create();
    show();
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif


GUIDialog_ChooserAbstract::~GUIDialog_ChooserAbstract() {
    // remove child from windowsParent
    myWindowsParent->getGUIMainWindowParent()->removeChild(this);
    getApp()->reg().writeIntEntry("LOCATOR", "autoCenter", myInstantCenter->getCheck());
    getApp()->reg().writeIntEntry("LOCATOR", "caseSensitive", myCaseSensitive->getCheck());
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
        if (o != nullptr && o->getType() == GLO_VEHICLE) {
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
GUIDialog_ChooserAbstract::onChgList(FXObject*, FXSelector, void*) {
    // mouse-click toggles item selection but changing current item with
    // keyboard does not affect select
    // Enabling the line below toggles the behavior (which must be fixed via onChgListSel)
    int current = myList->getCurrentItem();
    if (current >= 0 && current < myList->getNumItems()) {
        myList->selectItem(current);
        myCenterButton->enable();
        if (myTrackButton->shown()) {
            myTrackButton->enable();
        }
        if (myInstantCenter != nullptr && myInstantCenter->getCheck() == TRUE) {
            onCmdCenter(nullptr, 0, nullptr);
        }
    } else {
        myCenterButton->disable();
        myTrackButton->disable();
    }
    return 1;
}

long
GUIDialog_ChooserAbstract::onChgListSel(FXObject*, FXSelector, void*) {
    int current = myList->getCurrentItem();
    if (current >= 0 && current < myList->getNumItems()) {
        myList->selectItem(current);
    }
    return 1;
}

long
GUIDialog_ChooserAbstract::onChgText(FXObject*, FXSelector, void*) {
    if (myFilterSubstring != nullptr && myFilterSubstring->getCheck() == TRUE) {
        applyFilters();
        return 1;
    }
    const bool caseSensitive = (myCaseSensitive != nullptr && myCaseSensitive->getCheck() == TRUE);
    int id = -1;
    if (locateByName()) {
        // findItem does not support substring search
        const int numItems = myList->getNumItems();
        FXString t = myTextEntry->getText();
        if (!caseSensitive) {
            t = t.lower();
        }
        for (int i = 0; i < numItems; i++) {
            FXString t2 = myList->getItemText(i);
            if (!caseSensitive) {
                t2 = t2.lower();
            }
            if (t2.find(t) >= 0) {
                id = i;
                break;
            }
        }
    } else {
        const int caseOpt = caseSensitive ? 0 : SEARCH_IGNORECASE;
        id = myList->findItem(myTextEntry->getText(), -1, SEARCH_PREFIX | caseOpt);
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
    if (myTrackButton->shown()) {
        myTrackButton->enable();
    }
    return 1;
}


long
GUIDialog_ChooserAbstract::onCmdText(FXObject*, FXSelector, void*) {
    int current = myList->getCurrentItem();
    if (current >= 0 && current < myList->getNumItems() && myList->isItemSelected(current) && myList->getItemData(current) != nullptr) {
        myWindowsParent->setView(*static_cast<GUIGlID*>(myList->getItemData(current)));
    }
    return 1;
}



long
GUIDialog_ChooserAbstract::onKeyPress(FXObject* o, FXSelector sel, void* ptr) {
    FXEvent* event = (FXEvent*)ptr;
    if (event->code == KEY_Return) {
        onCmdText(nullptr, 0, nullptr);
        if ((event->state & CONTROLMASK) != 0) {
            close(true);
        }
        return 1;
    } else if (event->code == KEY_Left || (event->code == KEY_Up && myList->getCurrentItem() == 0)) {
        myTextEntry->setFocus();
        return 1;
    } else if (event->code == KEY_Escape) {
        close(true);
        return 1;
    }
    return FXMainWindow::onKeyPress(o, sel, ptr);
}


long
GUIDialog_ChooserAbstract::onCmdFilter(FXObject*, FXSelector, void*) {
    applyFilters();
    return 1;
}


long
GUIDialog_ChooserAbstract::onCmdFilterSubstr(FXObject*, FXSelector, void*) {
    applyFilters();
    return 1;
}


long
GUIDialog_ChooserAbstract::onCmdLocateByName(FXObject*, FXSelector, void*) {
    applyFilters();
    myTextEntry->setFocus();
    return 1;
}


long
GUIDialog_ChooserAbstract::onCmdUpdate(FXObject*, FXSelector, void*) {
    const std::vector<GUIGlID> ids = myWindowsParent->getObjectIDs(myMessageId);
    if (!ids.empty()) {
        refreshList(ids);
    } else {
        applyFilters();
    }
    return 1;
}


long
GUIDialog_ChooserAbstract::onCmdCaseSensitive(FXObject*, FXSelector, void*) {
    if (myFilterSubstring != nullptr && myFilterSubstring->getCheck() == TRUE) {
        applyFilters();
    } else {
        onChgText(nullptr, 0, nullptr);
    }
    return 1;
}


long
GUIDialog_ChooserAbstract::onCmdAutoCenter(FXObject*, FXSelector, void*) {
    if (myInstantCenter != nullptr && myInstantCenter->getCheck() == TRUE) {
        onCmdCenter(nullptr, 0, nullptr);
    }
    return 1;
}


long
GUIDialog_ChooserAbstract::onUpdAddListSelection(FXObject* sender, FXSelector, void* ptr) {
    const int numItems = myList ? myList->getNumItems() : 0;
    const bool enable = (numItems > 0 && myNumSelectedItems < numItems);
    sender->handle(this, enable ? FXSEL(SEL_COMMAND, ID_ENABLE) : FXSEL(SEL_COMMAND, ID_DISABLE), ptr);
    return 1;
}


long
GUIDialog_ChooserAbstract::onUpdClearListSelection(FXObject* sender, FXSelector, void* ptr) {
    const bool enable = (myNumSelectedItems > 0);
    sender->handle(this, enable ? FXSEL(SEL_COMMAND, ID_ENABLE) : FXSEL(SEL_COMMAND, ID_DISABLE), ptr);
    return 1;
}


bool
GUIDialog_ChooserAbstract::locateByName() const {
    return myLocateByName != nullptr && myLocateByName->getCheck() == TRUE;
}


std::string
GUIDialog_ChooserAbstract::getObjectName(GUIGlObject* o) const {
    if (locateByName()) {
        return o->getOptionalName();
    } else {
        return o->getMicrosimID();
    }
}


void
GUIDialog_ChooserAbstract::refreshList(const std::vector<GUIGlID>& ids) {
    myAllIDs = ids;
    myIDs.clear();
    for (auto id : ids) {
        myIDs.insert(id);
    }
    applyFilters();
}


void
GUIDialog_ChooserAbstract::applyFilters() {
    const bool hideUnselected = (myHideUnselected != nullptr && myHideUnselected->getCheck() == TRUE);
    const bool byName = locateByName();
    const bool filterSubstring = (myFilterSubstring != nullptr && myFilterSubstring->getCheck() == TRUE);
    const bool caseSensitive = (myCaseSensitive != nullptr && myCaseSensitive->getCheck() == TRUE);

    FXString searchPattern = myTextEntry ? myTextEntry->getText() : "";
    if (!caseSensitive) {
        searchPattern = searchPattern.lower();
    }
    const bool haveSearchPattern = filterSubstring && (searchPattern.length() > 0);

    std::vector<std::pair<std::string, GUIGlID>> filteredItems;
    filteredItems.reserve(myAllIDs.size());

    for (const auto glID : myAllIDs) {
        GUIGlObject* o = GUIGlObjectStorage::gIDStorage.getObjectBlocking(glID);
        if (o == nullptr) {
            continue;
        }
        const bool selected = myWindowsParent->isSelected(o);
        if (hideUnselected && !selected) {
            GUIGlObjectStorage::gIDStorage.unblockObject(glID);
            continue;
        }
        const std::string name = getObjectName(o);
        if (byName && name.empty()) {
            GUIGlObjectStorage::gIDStorage.unblockObject(glID);
            continue;
        }
        if (haveSearchPattern) {
            FXString itemText(name.c_str());
            if (!caseSensitive) {
                itemText = itemText.lower();
            }
            if (itemText.find(searchPattern) < 0) {
                GUIGlObjectStorage::gIDStorage.unblockObject(glID);
                continue;
            }
        }
        filteredItems.push_back(std::make_pair(name, glID));
        GUIGlObjectStorage::gIDStorage.unblockObject(glID);
    }

    if (byName) {
        std::sort(filteredItems.begin(), filteredItems.end());
    }

    myList->clearItems();
    std::vector<GUIGlID> displayedGlIDs;
    displayedGlIDs.reserve(filteredItems.size());

    for (const auto& item : filteredItems) {
        const std::string& name = item.first;
        const GUIGlID glID = item.second;
        GUIGlObject* o = GUIGlObjectStorage::gIDStorage.getObjectBlocking(glID);
        const bool selected = (o != nullptr) && myWindowsParent->isSelected(o);
        if (o != nullptr) {
            GUIGlObjectStorage::gIDStorage.unblockObject(glID);
        }
        FXIcon* const ico = selected ? GUIIconSubSys::getIcon(GUIIcon::FLAG) : nullptr;
        myIDs.insert(glID);
        myList->appendItem(name.c_str(), ico, (void*)&(*myIDs.find(glID)));
        displayedGlIDs.push_back(glID);
    }
    myList->update();
    if (myCountLabel) {
        myCountLabel->setText(TLF("% objects", toString(displayedGlIDs.size())).c_str());
    }

    // notify Netedit to update filtered ACs
    filterACs(displayedGlIDs);

    // Update item selection
    if (filterSubstring) {
        if (myList->getNumItems() > 0) {
            myList->makeItemVisible(0);
            myList->selectItem(0);
            myList->setCurrentItem(0, true);
            myCenterButton->enable();
            if (myTrackButton->shown()) {
                myTrackButton->enable();
            }
            if (myInstantCenter != nullptr && myInstantCenter->getCheck() == TRUE) {
                onCmdCenter(nullptr, 0, nullptr);
            }
        } else {
            myCenterButton->disable();
            myTrackButton->disable();
        }
    } else {
        if (myTextEntry && myTextEntry->getText().length() > 0) {
            onChgText(nullptr, 0, nullptr);
        } else if (myList->getNumItems() > 0) {
            myList->makeItemVisible(0);
            myList->selectItem(0);
            myList->setCurrentItem(0, true);
            myCenterButton->enable();
            if (myTrackButton->shown()) {
                myTrackButton->enable();
            }
        } else {
            myCenterButton->disable();
            myTrackButton->disable();
        }
    }
    updateSelectionButtons();
}


void
GUIDialog_ChooserAbstract::updateSelectionButtons() {
    const int numItems = myList ? myList->getNumItems() : 0;
    int numSelected = 0;
    if (myList != nullptr) {
        for (int i = 0; i < numItems; i++) {
            if (myList->getItemIcon(i) != nullptr) {
                numSelected++;
            }
        }
    }
    myNumSelectedItems = numSelected;
    const bool enableSelectAll = (numItems > 0 && numSelected < numItems);
    const bool enableDeselectAll = (numSelected > 0);
    if (mySelectAllButton != nullptr) {
        if (enableSelectAll) {
            mySelectAllButton->enable();
        } else {
            mySelectAllButton->disable();
        }
    }
    if (myDeselectAllButton != nullptr) {
        if (enableDeselectAll) {
            myDeselectAllButton->enable();
        } else {
            myDeselectAllButton->disable();
        }
    }
}


long
GUIDialog_ChooserAbstract::onCmdToggleSelection(FXObject*, FXSelector, void*) {
    FXIcon* flag = GUIIconSubSys::getIcon(GUIIcon::FLAG);
    int i = myList->getCurrentItem();
    if (i >= 0 && i < myList->getNumItems()) {
        toggleSelection(i);
        if (myList->getItemIcon(i) == flag) {
            myList->setItemIcon(i, nullptr);
            if (myHideUnselected != nullptr && myHideUnselected->getCheck() == TRUE) {
                applyFilters();
                return 1;
            }
        } else {
            myList->setItemIcon(i, flag);
        }
    }
    myList->update();
    myWindowsParent->getView()->update();
    updateSelectionButtons();
    return 1;
}


long
GUIDialog_ChooserAbstract::onCmdAddListSelection(FXObject*, FXSelector, void*) {
    FXIcon* flag = GUIIconSubSys::getIcon(GUIIcon::FLAG);
    const int numItems = myList->getNumItems();
    for (int i = 0; i < numItems; i++) {
        select(i);
        myList->setItemIcon(i, flag);
    }
    myList->update();
    myWindowsParent->getView()->update();
    updateSelectionButtons();
    return 1;
}


long
GUIDialog_ChooserAbstract::onCmdClearListSelection(FXObject*, FXSelector, void*) {
    const int numItems = myList->getNumItems();
    for (int i = 0; i < numItems; i++) {
        deselect(i);
        myList->setItemIcon(i, nullptr);
    }
    if (myHideUnselected != nullptr && myHideUnselected->getCheck() == TRUE) {
        applyFilters();
    } else {
        myList->update();
        myWindowsParent->getView()->update();
        updateSelectionButtons();
    }
    return 1;
}


void
GUIDialog_ChooserAbstract::toggleSelection(int listIndex) {
    GUIGlID* glID = static_cast<GUIGlID*>(myList->getItemData(listIndex));
    gSelected.toggleSelection(*glID);
}

void
GUIDialog_ChooserAbstract::select(int listIndex) {
    GUIGlID* glID = static_cast<GUIGlID*>(myList->getItemData(listIndex));
    gSelected.select(*glID);
}

void
GUIDialog_ChooserAbstract::deselect(int listIndex) {
    GUIGlID* glID = static_cast<GUIGlID*>(myList->getItemData(listIndex));
    gSelected.deselect(*glID);
}


void
GUIDialog_ChooserAbstract::filterACs(const std::vector<GUIGlID>& /*GLIDs*/) {
    // overrided in GNEACChooserDialog
}

/****************************************************************************/
