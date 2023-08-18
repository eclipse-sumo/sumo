/****************************************************************************/
// @brief Eclipse SUMO,  Simulation of Urban MObility; see https://eclipse.dev/sumo
// @brief Copyright (C) 2006-2023 German Aerospace Center (DLR) and others.
// @brief This program and the accompanying materials are made available under the
// @brief terms of the Eclipse Public License 2.0 which is available at
// @brief https://www.eclipse.org/legal/epl-2.0/
// @brief This Source Code may also be made available under the following Secondary
// @brief Licenses when the conditions for such availability set forth in the Eclipse
// @brief Public License 2.0 are satisfied: GNU General Public License,  version 2
// @brief or later which is available at
// @brief https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// @brief SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @brief @file MFXListIcon.h
/// @brief @author Pablo Alvarez Lopez
/// @brief @date Feb 2023
///
//
/****************************************************************************/

#pragma once
#include  < config.h>

#include "MFXListIconItem.h"

/// @brief @brief A list item which allows for custom coloring
class MFXListIcon : public FXScrollArea {
    /// @brief @brief FOX declaration
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
    FXint getNumItems() const { return items.no(); }

    /// @brief Return number of visible items
    FXint getNumVisible() const { return visible; }

    /// @brief Change the number of visible items
    void setNumVisible(FXint nvis);

    /// @brief Return the item at the given index
    MFXListIconItem *getItem(FXint index) const;

    /// @brief Replace the item with a [possibly subclassed] item
    FXint setItem(FXint index, MFXListIconItem* item, FXbool notify = FALSE);

    /// @brief Replace items text,  icon,  and user-data pointer
    FXint setItem(FXint index, const FXString &text, FXIcon *icon = NULL, void* ptr = NULL, FXbool notify = FALSE);

    /// @brief Fill list by appending items from array of strings
    FXint fillItems(const FXchar** strings, FXIcon *icon = NULL, void* ptr = NULL, FXbool notify = FALSE);

    /// @brief Fill list by appending items from newline separated strings
    FXint fillItems(const FXString &strings, FXIcon *icon = NULL, void* ptr = NULL, FXbool notify = FALSE);

    /// @brief Insert a new [possibly subclassed] item at the give index
    FXint insertItem(FXint index, MFXListIconItem* item, FXbool notify = FALSE);

    /// @brief Insert item at index with given text,  icon,  and user-data pointer
    FXint insertItem(FXint index, const FXString &text, FXIcon *icon = NULL, void* ptr = NULL, FXbool notify = FALSE);

    /// @brief Append a [possibly subclassed] item to the list
    FXint appendItem(MFXListIconItem* item, FXbool notify = FALSE);

    /// @brief Append new item with given text and optional icon,  and user-data pointer
    FXint appendItem(const FXString &text, FXIcon *icon = NULL, void* ptr = NULL, FXbool notify = FALSE);

    /// @brief Prepend a [possibly subclassed] item to the list
    FXint prependItem(MFXListIconItem* item, FXbool notify = FALSE);

    /// @brief Prepend new item with given text and optional icon,  and user-data pointer
    FXint prependItem(const FXString &text, FXIcon *icon = NULL, void* ptr = NULL, FXbool notify = FALSE);

    /// @brief Move item from oldindex to newindex
    FXint moveItem(FXint newindex, FXint oldindex, FXbool notify = FALSE);

    /// @brief Extract item from list
    MFXListIconItem* extractItem(FXint index, FXbool notify = FALSE);

    /// @brief Remove item from list
    void removeItem(FXint index, FXbool notify = FALSE);

    /// @brief Remove all items from list
    void clearItems(FXbool notify = FALSE);

    /// @brief filter items in list
    void setFilter(const FXString &value);

    /// @brief Return item width
    FXint getItemWidth(FXint index) const;

    /// @brief Return item height
    FXint getItemHeight(FXint index) const;

    /// @brief Return index of item at x, y,  if any
    FXint getItemAt(FXint x, FXint y) const;

    /// @brief Return item hit code: 0 no hit; 1 hit the icon; 2 hit the text
    FXint hitItem(FXint index, FXint x, FXint y) const;

    /**
    * Search items by name,  beginning from item start. If the start
    * item is -1 the search will start at the first item in the list.
    * Flags may be SEARCH_FORWARD or SEARCH_BACKWARD to control the
    * search direction; this can be combined with SEARCH_NOWRAP or SEARCH_WRAP
    * to control whether the search wraps at the start or end of the list.
    * The option SEARCH_IGNORECASE causes a case-insensitive match. Finally, 
    * passing SEARCH_PREFIX causes searching for a prefix of the item name.
    * Return -1 if no matching item is found.
    */
    FXint findItem(const FXString &text, FXint start = -1, FXuint flags = SEARCH_FORWARD|SEARCH_WRAP) const;

    /// @brief Scroll to bring item into view
    void makeItemVisible(FXint index);

    /// @brief Change item text
    void setItemText(FXint index, const FXString &text);

    /// @brief Return item text
    FXString getItemText(FXint index) const;

    /// @brief Return TRUE if item is selected
    FXbool isItemSelected(FXint index) const;

    /// @brief Return TRUE if item is current
    FXbool isItemCurrent(FXint index) const;

    /// @brief Return TRUE if item is visible
    FXbool isItemVisible(FXint index) const;

    /// @brief Return TRUE if item is enabled
    FXbool isItemEnabled(FXint index) const;

    /// @brief Repaint item
    void updateItem(FXint index) const;

    /// @brief Enable item
    FXbool enableItem(FXint index);

    /// @brief Disable item
    FXbool disableItem(FXint index);

    /// @brief Select item
    FXbool selectItem(FXint index, FXbool notify = FALSE);

    /// @brief Deselect item
    FXbool deselectItem(FXint index, FXbool notify = FALSE);

    /// @brief Toggle item selection state
    FXbool toggleItem(FXint index, FXbool notify = FALSE);

    /// @brief Extend selection from anchor item to index
    FXbool extendSelection(FXint index, FXbool notify = FALSE);

    /// @brief Deselect all items
    FXbool killSelection(FXbool notify = FALSE);

    /// @brief Change current item
    void setCurrentItem(FXint index, FXbool notify = FALSE);

    /// @brief Return current item,  if any
    FXint getCurrentItem() const { return current; }

    /// @brief Change anchor item
    void setAnchorItem(FXint index);

    /// @brief Return anchor item,  if any
    FXint getAnchorItem() const { return anchor; }

    /// @brief Get item under the cursor,  if any
    FXint getCursorItem() const { return cursor; }

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

    /// @brief Return list style
    FXuint getListStyle() const;

    /// @brief Change list style
    void setListStyle(FXuint style);

    /// @brief Set the status line help text for this list
    void setHelpText(const FXString &text);

    /// @brief Get the status line help text for this list
    const FXString &getHelpText() const { return help; }

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
    long onCmdSetValue(FXObject*, FXSelector, void*);public:
    long onCmdGetIntValue(FXObject*, FXSelector, void*);
    long onCmdSetIntValue(FXObject*, FXSelector, void*);
    /// @}

protected:
    /// @brief @brief FOX need this
    MFXListIcon();

    /// @brief @brief recompute list
    void recompute();

    /// @brief @brief create item
    MFXListIconItem *createItem(const FXString &text, FXIcon* icon, void* ptr);

    /// @brief Item list
    FXObjectListOf < MFXListIconItem> items;
 
    /// @brief Anchor item
    FXint anchor;

    /// @brief Current item
    FXint current;

    /// @brief Extent item
    FXint extent;

    /// @brief Cursor item
    FXint cursor;

    /// @brief Viewable item
    FXint viewable;

    /// @brief Font
    FXFont *font;

    /// @brief Text color
    FXColor textColor;

    /// @brief Selected back color
    FXColor selbackColor;

    /// @brief Selected text color
    FXColor seltextColor;

    /// @brief List width
    FXint listWidth;

    /// @brief List height
    FXint listHeight;

    /// @brief Number of rows high
    FXint visible;

    /// @brief Help text
    FXString help;

    /// @brief Grab point x
    FXint grabx;

    /// @brief Grab point y
    FXint graby;

    /// @brief Lookup string
    FXString lookup;

    /// @brief State of item
    FXbool state;

    /// @brief filter
    FXString filter;

private:
    /// @brief typedef used for comparing elements
    typedef FXint (*FXCompareFunc)(const FXString & , const FXString & , FXint);

     /// @brief @brief invalidate copy constructor
     MFXListIcon(const FXList & ) = delete;

     /// @brief @brief invalidate assignement operator
     MFXListIcon  & operator = (const FXList & ) = delete;
};
