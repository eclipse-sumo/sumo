/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2004-2022 German Aerospace Center (DLR) and others.
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
/// @file    FXTreeListDinamic.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2021
///
//
/****************************************************************************/

#include "FXTreeListDinamic.h"

#include <utils/common/UtilExceptions.h>

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(FXTreeListDinamic) FXTreeListDinamicMap[] = {
    FXMAPFUNC(SEL_LEFTBUTTONPRESS, 0, FXTreeListDinamic::onLeftBtnPress),
};

// Object implementation
FXIMPLEMENT(FXTreeListDinamic, FXTreeList, FXTreeListDinamicMap, ARRAYNUMBER(FXTreeListDinamicMap))

// ===========================================================================
// member method definitions
// ===========================================================================

FXTreeListDinamic::FXTreeListDinamic(FXComposite* p, FXObject* tgt, FXSelector sel, FXuint opts) :
    FXTreeList(p, tgt, sel, opts, 0, 0, 0, 200) {
}


void
FXTreeListDinamic::show() {
    // update height
    setHeight(getContentHeight() + 20);
    // show
    FXTreeList::show();
}


void
FXTreeListDinamic::hide() {
    // hide
    FXTreeList::hide();
}


void 
FXTreeListDinamic::clearItems() {
    myFXTreeItems.clear();
    return FXTreeList::clearItems();
}


FXint 
FXTreeListDinamic::getNumItems() {
    return FXTreeList::getNumItems();
}


FXint 
FXTreeListDinamic::getSelectedIndex() {
    return mySelectedItem;
}


FXTreeItem* 
FXTreeListDinamic::insertItem(FXTreeItem* father, const FXString& text, FXIcon* oi) {
    auto newItem = FXTreeList::insertItem(nullptr, father, text, oi, oi, nullptr, false);
    if (newItem != nullptr) {
        myFXTreeItems.push_back(newItem);
        return newItem;
    } else {
        throw ProcessError("New item cannot be NULL");
    }
}


FXWindow* 
FXTreeListDinamic::getFXWindow() {
    return this;
}


FXTreeItem* 
FXTreeListDinamic::getItemAt(FXint x,FXint y) const {
    return FXTreeList::getItemAt(x, y);
}


FXTreeItem* 
FXTreeListDinamic::getItem(FXint index) const {
    return myFXTreeItems.at(index);
}


void
FXTreeListDinamic::resetSelectedItem() {
    if (mySelectedItem != -1) {
        myFXTreeItems.at(mySelectedItem)->setSelected(false);
        mySelectedItem = -1;
    }
}


long
FXTreeListDinamic::onLeftBtnPress(FXObject* obj, FXSelector sel, void* ptr) {
    FXTreeList::onLeftBtnPress(obj, sel, ptr);
    // update selected item
    mySelectedItem = -1;
    for (int i = 0; i < (int)myFXTreeItems.size(); i++) {
        if (myFXTreeItems.at(i)->isSelected()) {
            mySelectedItem = i;
        }
    }
    // update height
    setHeight(getContentHeight() + 20);
    return 1;
}


FXTreeListDinamic::FXTreeListDinamic() :
    FXTreeList::FXTreeList() {
}
