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

#define SIDE_SPACING        4   // Spacing between side and item
#define ICON_SPACING        4   // Spacing between parent and child in x direction

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
    register FXIcon* icon = (state & OPENED) ? openIcon : closedIcon;
    register FXFont* font = list->getFont();
    register FXint th = 0, tw = 0, ih = 0, iw = 0;
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

// FXTreeListDinamic

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
FXTreeListDinamic::update() {
    // update
    FXTreeList::update();
}


void 
FXTreeListDinamic::clearItems() {
    myFXTreeItemDynamicItems.clear();
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
    FXTreeItemDynamic* newItem = dynamic_cast<FXTreeItemDynamic*>(FXTreeList::insertItem(nullptr, father, new FXTreeItemDynamic(text, oi, oi, nullptr), false));
    if (newItem != nullptr) {
        myFXTreeItemDynamicItems.push_back(newItem);
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


FXTreeItemDynamic* 
FXTreeListDinamic::getItem(FXint index) const {
    return myFXTreeItemDynamicItems.at(index);
}


void
FXTreeListDinamic::resetSelectedItem() {
    if (mySelectedItem != -1) {
        myFXTreeItemDynamicItems.at(mySelectedItem)->setSelected(false);
        mySelectedItem = -1;
    }
}


long
FXTreeListDinamic::onLeftBtnPress(FXObject* obj, FXSelector sel, void* ptr) {
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


FXTreeListDinamic::FXTreeListDinamic() :
    FXTreeList::FXTreeList() {
}