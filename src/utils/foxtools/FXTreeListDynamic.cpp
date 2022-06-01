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
/// @file    FXTreeListDynamic.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2021
///
//
/****************************************************************************/

#include "FXTreeListDynamic.h"

#include <utils/common/UtilExceptions.h>

#define SIDE_SPACING        4   // Spacing between side and item
#define ICON_SPACING        4   // Spacing between parent and child in x direction

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(FXTreeListDynamic) FXTreeListDynamicMap[] = {
    FXMAPFUNC(SEL_LEFTBUTTONPRESS, 0, FXTreeListDynamic::onLeftBtnPress),
};

// Object implementation
FXIMPLEMENT(FXTreeListDynamic, FXTreeList, FXTreeListDynamicMap, ARRAYNUMBER(FXTreeListDynamicMap))

// ===========================================================================
// member method definitions
// ===========================================================================

// FXTreeItemDynamic

FXTreeItemDynamic::FXTreeItemDynamic(const FXString& text, FXIcon* oi, FXIcon* ci, void* ptr) :
    FXTreeItem(text, oi, ci, ptr) {
}


void 
FXTreeItemDynamic::setTextColor(FXColor clr) {
    myTextColor = clr;
}


void
FXTreeItemDynamic::draw(const FXTreeList* list, FXDC& dc, FXint xx, FXint yy, FXint, FXint hh) const {
    FXIcon* icon = (state & OPENED) ? openIcon : closedIcon;
    FXFont* font = list->getFont();
    FXint th = 0, tw = 0, ih = 0, iw = 0;
    xx += SIDE_SPACING / 2;
    if (icon) {
        iw = icon->getWidth();
        ih = icon->getHeight();
        dc.drawIcon(icon, xx, yy + (hh - ih) / 2);
        xx += ICON_SPACING + iw;
    }
    if (!label.empty()) {
        tw = 4 + font->getTextWidth(label.text(), label.length());
        th = 4 + font->getFontHeight();
        yy += (hh - th) / 2;
        if (isSelected()) {
            dc.setForeground(list->getSelBackColor());
            dc.fillRectangle(xx, yy, tw, th);
        }
        if (hasFocus()) {
            dc.drawFocusRectangle(xx + 1, yy + 1, tw - 2, th - 2);
        }
        if (!isEnabled()) {
            dc.setForeground(makeShadowColor(list->getBackColor()));
        } else if (isSelected()) {
            dc.setForeground(list->getSelTextColor());
        } else {
            dc.setForeground(myTextColor);
        }
        dc.drawText(xx + 2, yy + font->getFontAscent() + 2, label);
    }
}

// FXTreeListDynamic

FXTreeListDynamic::FXTreeListDynamic(FXComposite* p, FXObject* tgt, FXSelector sel, FXuint opts) :
    FXTreeList(p, tgt, sel, opts, 0, 0, 0, 200) {
}


void
FXTreeListDynamic::show() {
    // update height
    setHeight(getContentHeight() + 20);
    // show
    FXTreeList::show();
}


void
FXTreeListDynamic::hide() {
    // hide
    FXTreeList::hide();
}


void
FXTreeListDynamic::update() {
    // update
    FXTreeList::update();
}


void 
FXTreeListDynamic::clearItems() {
    myFXTreeItemDynamicItems.clear();
    return FXTreeList::clearItems();
}


FXint 
FXTreeListDynamic::getNumItems() {
    return FXTreeList::getNumItems();
}


FXint 
FXTreeListDynamic::getSelectedIndex() {
    return mySelectedItem;
}


FXTreeItem* 
FXTreeListDynamic::prependItem(FXTreeItem* father, const FXString& text, FXIcon* oi, FXColor textColor) {
    FXTreeItemDynamic* newItem = dynamic_cast<FXTreeItemDynamic*>(FXTreeList::prependItem(father, new FXTreeItemDynamic(text, oi, oi, nullptr), false));
    if (newItem != nullptr) {
        myFXTreeItemDynamicItems.insert(myFXTreeItemDynamicItems.begin(), newItem);
        newItem->setTextColor(textColor);
        return newItem;
    } else {
        throw ProcessError("New item cannot be NULL");
    }
}


FXTreeItem* 
FXTreeListDynamic::appendItem(FXTreeItem* father, const FXString& text, FXIcon* oi, FXColor textColor) {
    FXTreeItemDynamic* newItem = dynamic_cast<FXTreeItemDynamic*>(FXTreeList::appendItem(father, new FXTreeItemDynamic(text, oi, oi, nullptr), false));
    if (newItem != nullptr) {
        myFXTreeItemDynamicItems.push_back(newItem);
        newItem->setTextColor(textColor);
        return newItem;
    } else {
        throw ProcessError("New item cannot be NULL");
    }
}


FXWindow* 
FXTreeListDynamic::getFXWindow() {
    return this;
}


FXTreeItem* 
FXTreeListDynamic::getItemAt(FXint x,FXint y) const {
    return FXTreeList::getItemAt(x, y);
}


FXTreeItemDynamic* 
FXTreeListDynamic::getItem(FXint index) const {
    return myFXTreeItemDynamicItems.at(index);
}


void
FXTreeListDynamic::resetSelectedItem() {
    if (mySelectedItem != -1) {
        myFXTreeItemDynamicItems.at(mySelectedItem)->setSelected(false);
        mySelectedItem = -1;
    }
}


long
FXTreeListDynamic::onLeftBtnPress(FXObject* obj, FXSelector sel, void* ptr) {
    FXTreeList::onLeftBtnPress(obj, sel, ptr);
    // update selected item
    mySelectedItem = -1;
    for (int i = 0; i < (int)myFXTreeItemDynamicItems.size(); i++) {
        if (myFXTreeItemDynamicItems.at(i)->isSelected()) {
            mySelectedItem = i;
        }
    }
    // update height
    setHeight(getContentHeight() + 20);
    return 1;
}


FXTreeListDynamic::FXTreeListDynamic() :
    FXTreeList::FXTreeList() {
}