/****************************************************************************/
/// @file    GUIDialog_GLObjChooser.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// Class for the window that allows to choose a street, junction or vehicle
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
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
#include <utils/gui/globjects/GUIGlObject_AbstractAdd.h>
#include "GUIDialog_GLObjChooser.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GUIDialog_GLObjChooser) GUIDialog_GLObjChooserMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSER_CENTER, GUIDialog_GLObjChooser::onCmdCenter),
    FXMAPFUNC(SEL_COMMAND,  MID_CANCEL,         GUIDialog_GLObjChooser::onCmdClose),
    FXMAPFUNC(SEL_CHANGED,  MID_CHOOSER_TEXT,   GUIDialog_GLObjChooser::onChgText),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSER_TEXT,   GUIDialog_GLObjChooser::onCmdText),
    FXMAPFUNC(SEL_KEYPRESS, MID_CHOOSER_LIST,   GUIDialog_GLObjChooser::onListKeyPress),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSER_FILTER, GUIDialog_GLObjChooser::onCmdFilter),
};

FXIMPLEMENT(GUIDialog_GLObjChooser, FXMainWindow, GUIDialog_GLObjChooserMap, ARRAYNUMBER(GUIDialog_GLObjChooserMap))


// ===========================================================================
// method definitions
// ===========================================================================
GUIDialog_GLObjChooser::GUIDialog_GLObjChooser(
    GUIGlChildWindow* parent,
    FXIcon* icon,
    const FXString& title,
    GUIGlObjectType type,
    const std::vector<GUIGlID>& ids,
    GUIGlObjectStorage& glStorage):
    FXMainWindow(parent->getApp(), title, icon, NULL, DECOR_ALL, 20, 20, 300, 300),
    myObjectType(type),
    myParent(parent) {
    FXHorizontalFrame* hbox = new FXHorizontalFrame(this, LAYOUT_FILL_X | LAYOUT_FILL_Y, 0, 0, 0, 0, 0, 0, 0, 0);
    // build the list
    FXVerticalFrame* layout1 = new FXVerticalFrame(hbox, LAYOUT_FILL_X | LAYOUT_FILL_Y | LAYOUT_TOP, 0, 0, 0, 0, 4, 4, 4, 4);
    myTextEntry = new FXTextField(layout1, 0, this, MID_CHOOSER_TEXT, LAYOUT_FILL_X | FRAME_THICK | FRAME_SUNKEN);
    FXVerticalFrame* style1 = new FXVerticalFrame(layout1, LAYOUT_FILL_X | LAYOUT_FILL_Y | LAYOUT_TOP | FRAME_THICK | FRAME_SUNKEN, 0, 0, 0, 0, 0, 0, 0, 0);
    myList = new FXList(style1, this, MID_CHOOSER_LIST, LAYOUT_FILL_X | LAYOUT_FILL_Y | LIST_SINGLESELECT | FRAME_SUNKEN | FRAME_THICK);
    for (std::vector<GUIGlID>::const_iterator i = ids.begin(); i != ids.end(); ++i) {
        GUIGlObject* o = glStorage.getObjectBlocking(*i);
        if (o == 0) {
            continue;
        }
        const std::string& name = o->getMicrosimID();
        bool selected = myParent->isSelected(o);
        FXIcon* icon = selected ? GUIIconSubSys::getIcon(ICON_FLAG) : 0;
        myIDs.insert(o->getGlID());
        myList->appendItem(name.c_str(), icon, (void*) & (*myIDs.find(o->getGlID())));
        glStorage.unblockObject(*i);
    }
    // build the buttons
    FXVerticalFrame* layout = new FXVerticalFrame(hbox, LAYOUT_TOP, 0, 0, 0, 0, 4, 4, 4, 4);
    myCenterButton = new FXButton(layout, "Center\t\t", GUIIconSubSys::getIcon(ICON_RECENTERVIEW),
                                  this, MID_CHOOSER_CENTER, ICON_BEFORE_TEXT | LAYOUT_FILL_X | FRAME_THICK | FRAME_RAISED,
                                  0, 0, 0, 0, 4, 4, 4, 4);
    new FXHorizontalSeparator(layout, SEPARATOR_GROOVE | LAYOUT_FILL_X);
    new FXButton(layout, "Hide Unselected\t\t", GUIIconSubSys::getIcon(ICON_FLAG),
                 this, MID_CHOOSER_FILTER, ICON_BEFORE_TEXT | LAYOUT_FILL_X | FRAME_THICK | FRAME_RAISED,
                 0, 0, 0, 0, 4, 4, 4, 4);
    new FXHorizontalSeparator(layout, SEPARATOR_GROOVE | LAYOUT_FILL_X);
    new FXButton(layout, "Close\t\t", GUIIconSubSys::getIcon(ICON_NO),
                 this, MID_CANCEL, ICON_BEFORE_TEXT | LAYOUT_FILL_X | FRAME_THICK | FRAME_RAISED,
                 0, 0, 0, 0, 4, 4, 4, 4);

    myParent->getParent()->addChild(this);
}


GUIDialog_GLObjChooser::~GUIDialog_GLObjChooser() {
    myParent->getParent()->removeChild(this);
}


void
GUIDialog_GLObjChooser::show() {
    FXMainWindow::show();
    myTextEntry->setFocus();
}


long
GUIDialog_GLObjChooser::onCmdCenter(FXObject*, FXSelector, void*) {
    int selected = myList->getCurrentItem();
    if (selected >= 0) {
        myParent->setView(*static_cast<GUIGlID*>(myList->getItemData(selected)));
    }
    return 1;
}


long
GUIDialog_GLObjChooser::onCmdClose(FXObject*, FXSelector, void*) {
    close(true);
    return 1;
}


long
GUIDialog_GLObjChooser::onChgText(FXObject*, FXSelector, void*) {
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
GUIDialog_GLObjChooser::onCmdText(FXObject*, FXSelector, void*) {
    int current = myList->getCurrentItem();
    if (current >= 0 && myList->isItemSelected(current)) {
        myParent->setView(*static_cast<GUIGlID*>(myList->getItemData(current)));
    }
    return 1;
}



long
GUIDialog_GLObjChooser::onListKeyPress(FXObject*, FXSelector, void* ptr) {
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
GUIDialog_GLObjChooser::onCmdFilter(FXObject*, FXSelector, void*) {
    FXIcon* flag = GUIIconSubSys::getIcon(ICON_FLAG);
    std::vector<GUIGlID> selectedGlIDs;
    std::vector<FXString> selectedMicrosimIDs;
    const int numItems = myList->getNumItems();
    for (int i = 0; i < numItems; i++) {
        const GUIGlID glID = *static_cast<GUIGlID*>(myList->getItemData(i));
        if (myList->getItemIcon(i) == flag) {
            selectedGlIDs.push_back(glID);
            selectedMicrosimIDs.push_back(myList->getItemText(i));
        }
    }
    myList->clearItems();
    const int numSelected = (const int)selectedGlIDs.size();
    for (int i = 0; i < numSelected; i++) {
        myList->appendItem(selectedMicrosimIDs[i], flag, (void*) & (*myIDs.find(selectedGlIDs[i])));
    }
    myList->update();
    return 1;
}


/****************************************************************************/

