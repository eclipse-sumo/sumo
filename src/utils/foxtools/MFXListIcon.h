/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2006-2023 German Aerospace Center (DLR) and others.
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
/// @file MFXListIcon.h
/// @author Pablo Alvarez Lopez
/// @date Feb 2023
///
//
/****************************************************************************/

#pragma once
#include <config.h>

#include <string>
#include <vector>

#include "MFXListIconItem.h"

/// @brief A list item which allows for custom coloring
class MFXListIcon : public FXScrollArea {
    /// @brief FOX declaration
    FXDECLARE(MFXListIcon)

public:
    enum {
        ID_LOOKUPTIMER = FXScrollArea::ID_LAST, 
        ID_LAST
    };

    /// @brief Construct a list with initially no items in it
    MFXListIcon(FXComposite *p, FXObject* tgt = NULL, FXSelector sel = 0, FXuint opts = LIST_NORMAL, FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0);

    /// @brief Destructor
    ~MFXListIcon();

    /// @brief Create server-side resources
    void create();

    /// @brief Detach server-side resources
    void detach();

    /// @brief Perform layout
    void layout();

    /// @brief Return default width
    FXint getDefaultWidth();

    /// @brief Return default height
    FXint getDefaultHeight();

    /// @brief Compute and return content width
    FXint getContentWidth();

    /// @brief Return content height
    FXint getContentHeight();

    /// @brief Recalculate layout
    void recalc();

    /// @brief List widget can receive focus
    bool canFocus() const;

    /// @brief Move the focus to this window
    void setFocus();

    /// @brief Remove the focus from this window
    void killFocus();

    /// @brief Return the number of items in the list
    FXint getNumItems() const { return (int)items.size(); }

    /// @brief Return number of visible items
    FXint getNumVisible() const { return visible; }

    /// @brief Change the number of visible items
    void setNumVisible(FXint nvis);

    /// @brief Return the item at the given index
    MFXListIconItem* getItem(FXint index) const;

    /// @brief Replace the item with a [possibly subclassed] item
    FXint setItem(FXint index, MFXListIconItem* item, FXbool notify = FALSE);

    /// @brief Replace items text, icon, and user-data pointer
    FXint editItem(FXint index, const FXString &text, FXIcon *icon = NULL, void* ptr = NULL, FXbool notify = FALSE);

    /// @brief Insert a new [possibly subclassed] item at the give index
    FXint insertItem(FXint index, MFXListIconItem* item, FXbool notify = FALSE);

    /// @brief Insert item at index with given text,  icon,  and user-data pointer
    FXint insertItem(FXint index, const FXString &text, FXIcon *icon = NULL, void* ptr = NULL, FXbool notify = FALSE);

    /// @brief Append a [possibly subclassed] item to the list
    FXint appendItem(MFXListIconItem* item, FXbool notify = FALSE);

    /// @brief Append new item with given text and optional icon,  and user-data pointer
    FXint appendItem(const FXString &text, FXIcon *icon = NULL, void* ptr = NULL, FXbool notify = FALSE);

    /// @brief Remove node from list
    void removeItem(FXint index, FXbool notify = FALSE);

    /// @brief Remove all items from list
    void clearItems(FXbool notify = FALSE);

    /// @brief filter items in list
    void setFilter(const FXString &value, FXLabel *label);

    /// @brief Return item width
    FXint getItemWidth(FXint index) const;

    /// @brief Return item height
    FXint getItemHeight(FXint index) const;

    /// @brief Return index of item at y, if any
    MFXListIconItem *getItemAt(FXint y) const;

    /// @brief Search items by name (In all items)
    int findItem(const FXString& text) const;

    /// @brief Return item hit code: 0 no hit; 1 hit the icon; 2 hit the text
    FXint hitItem(MFXListIconItem* item, FXint x, FXint y) const;

    /// @brief Scroll to bring item into view
    void makeItemVisible(MFXListIconItem* item);

    /// @brief Scroll to bring item into view
    void makeItemVisible(FXint index);

    /// @brief Return TRUE if item is current
    FXbool isItemCurrent(FXint index) const;

    /// @brief Return TRUE if item is visible
    FXbool isItemVisible(MFXListIconItem* item) const;

    /// @brief Repaint item
    void updateItem(MFXListIconItem* item) const;

    /// @brief Select item by index
    FXbool selectItem(MFXListIconItem* item, FXbool notify = FALSE);

    /// @brief Deselect item
    FXbool deselectItem(MFXListIconItem* item, FXbool notify = FALSE);

    /// @brief Toggle item selection state
    FXbool toggleItem(MFXListIconItem* item, FXbool notify = FALSE);

    /// @brief Deselect all items
    FXbool killSelection(FXbool notify = FALSE);

    /// @brief Change current item
    void setCurrentItem(MFXListIconItem* item, FXbool notify = FALSE);

    /// @brief Return current item, if any
    FXint getCurrentItemIndex() const;

    /// @brief Return viewable item, if any
    FXint getViewableItem() const;

    /// @brief Change anchor item
    void setAnchorItem(MFXListIconItem* item);

    /// @brief Return anchor item,  if any
    FXint getAnchorItem() const;

    /// @brief Get item under the cursor, if any
    MFXListIconItem *getCursorItem() const;

    /// @brief Return text font
    FXFont* getFont() const { return font; }

    /// @brief Return normal text color
    FXColor getTextColor() const { return textColor; }

    /// @brief Change normal text color
    void setTextColor(FXColor clr);

    /// @brief Return selected text background
    FXColor getSelBackColor() const { return selbackColor; }

    /// @brief Return selected text color
    FXColor getSelTextColor() const { return seltextColor; }

    /// @brief Set the status line help text for this list
    void setHelpText(const FXString &text);

    /// @brief Get the status line help text for this list
    const FXString &getHelpText() const { return help; }

    /// @brief tolower string
    FXString tolowerString(const FXString &str) const;

    /// @name call functions
    /// @{
    long onPaint(FXObject*, FXSelector, void*);
    long onEnter(FXObject*, FXSelector, void*);
    long onLeave(FXObject*, FXSelector, void*);
    long onUngrabbed(FXObject*, FXSelector, void*);
    long onKeyPress(FXObject*, FXSelector, void*);
    long onKeyRelease(FXObject*, FXSelector, void*);
    long onLeftBtnPress(FXObject*, FXSelector, void*);
    long onLeftBtnRelease(FXObject*, FXSelector, void*);
    long onRightBtnPress(FXObject*, FXSelector, void*);
    long onRightBtnRelease(FXObject*, FXSelector, void*);
    long onMotion(FXObject*, FXSelector, void*);
    long onFocusIn(FXObject*, FXSelector, void*);
    long onFocusOut(FXObject*, FXSelector, void*);
    long onAutoScroll(FXObject*, FXSelector, void*);
    long onClicked(FXObject*, FXSelector, void*);
    long onDoubleClicked(FXObject*, FXSelector, void*);
    long onTripleClicked(FXObject*, FXSelector, void*);
    long onCommand(FXObject*, FXSelector, void*);
    long onQueryTip(FXObject*, FXSelector, void*);
    long onQueryHelp(FXObject*, FXSelector, void*);
    long onTipTimer(FXObject*, FXSelector, void*);
    long onLookupTimer(FXObject*, FXSelector, void*);
    /// @}

protected:
    /// @brief @brief FOX need this
    MFXListIcon();

    /// @brief @brief recompute list
    void recompute();

    /// @brief @brief create item
    MFXListIconItem *createItem(const FXString &text, FXIcon* icon, void* ptr);

    /// @brief Item list
    std::vector<MFXListIconItem*> items;
 
    /// @brief Selected item list
    std::vector<MFXListIconItem*> itemFiltered;

    /// @brief Anchor item
    FXint anchor = -1;

    /// @brief Current item
    MFXListIconItem *currentItem = nullptr;

    /// @brief Extent item
    FXint extent = -1;

    /// @brief Cursor item
    MFXListIconItem* cursor = nullptr;

    /// @brief Viewable item
    MFXListIconItem* viewable = nullptr;

    /// @brief Font
    FXFont *font = nullptr;

    /// @brief Text color
    FXColor textColor = 0;

    /// @brief Selected back color
    FXColor selbackColor = 0;

    /// @brief Selected text color
    FXColor seltextColor = 0;

    /// @brief List width
    FXint listWidth = 0;

    /// @brief List height
    FXint listHeight = 0;

    /// @brief Number of rows high
    FXint visible = 0;

    /// @brief Help text
    FXString help;

    /// @brief Grab point x
    FXint grabx = 0;

    /// @brief Grab point y
    FXint graby = 0;

    /// @brief Lookup string
    FXString lookup;

    /// @brief State of item
    FXbool state = FALSE;

    /// @brief filter
    FXString filter;

private:
    /// @brief typedef used for comparing elements
    typedef FXint (*FXCompareFunc)(const FXString & , const FXString & , FXint);

    /// @brief check if filter element
    bool showItem(const FXString &itemName) const;

    /// @brief @brief invalidate copy constructor
    MFXListIcon(const FXList & ) = delete;

    /// @brief @brief invalidate assignement operator
    MFXListIcon  & operator = (const FXList & ) = delete;
};
