/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2006-2021 German Aerospace Center (DLR) and others.
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
/// @file    MFXIconComboBox.h
/// @author  Jakob Erdmann
/// @author  Pablo Alvarez Lopez
/// @date    2018-12-19
///
//
/****************************************************************************/

#pragma once
#include <config.h>

#include "MFXTextFieldIcon.h"


/// @brief A list item which allows for custom coloring
class MFXListItem : public FXListItem {
    /// @brief FOX declaration
    FXDECLARE(MFXListItem)

public:
    /// @brief Construct new item with given text, icon, and user-data
    MFXListItem(const FXString& text, FXIcon* ic, FXColor backGroundColor, void* ptr = NULL);

    /// @brief draw MFXListItem
    void draw(const FXList* list, FXDC& dc, FXint x, FXint y, FXint w, FXint h);

    /// @brief get background color
    const FXColor& getBackGroundColor() const;

protected:
    /// @brief fox need this
    MFXListItem();

    /// @brief backGround color
    FXColor myBackGroundColor;
};


/// @brief ComboBox with icon
class MFXIconComboBox : public FXPacker {
    /// @brief FOX declaration
    FXDECLARE(MFXIconComboBox)

public:
    /// @brief enum for ID
    enum {
        ID_LIST = FXPacker::ID_LAST,
        ID_TEXT,
        ID_LAST
    };

    /// Construct a Combo Box widget with room to display cols columns of text
    MFXIconComboBox(FXComposite* p, FXint cols, FXObject* tgt = NULL, FXSelector sel = 0, FXuint opts = COMBOBOX_NORMAL, FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0, FXint pl = DEFAULT_PAD, FXint pr = DEFAULT_PAD, FXint pt = DEFAULT_PAD, FXint pb = DEFAULT_PAD);

    /// Destructor
    virtual ~MFXIconComboBox();

    /// Create server-side resources
    virtual void create();

    /// Detach server-side resources
    virtual void detach();

    /// Destroy server-side resources
    virtual void destroy();

    /// Enable combo box
    virtual void enable();

    /// Disable combo box
    virtual void disable();

    /// Return default width
    virtual FXint getDefaultWidth();

    /// Return default height
    virtual FXint getDefaultHeight();

    /// Perform layout
    virtual void layout();

    /// Return true if combobox is editable
    FXbool isEditable() const;

    /// Set editable state
    void setEditable(FXbool edit = TRUE);

    /// Get the text
    FXString getText() const;

    /// Set the number of columns
    void setNumColumns(FXint cols);

    /// Get the number of columns
    FXint getNumColumns() const;

    /// Change text justification mode; default is JUSTIFY_LEFT
    void setJustify(FXuint mode);

    /// Return text justification mode
    FXuint getJustify() const;

    /// Return the number of items in the list
    FXint getNumItems() const;

    /// Return the number of visible items
    FXint getNumVisible() const;

    /// Set the number of visible items in the drop down list
    void setNumVisible(FXint nvis);

    /// Return true if current item
    FXbool isItemCurrent(FXint index) const;

    /// Set the current item (index is zero-based)
    void setCurrentItem(FXint index, FXbool notify = FALSE);

    /// Get the current item's index
    FXint getCurrentItem() const;

    /// Return the item at the given index
    FXString getItem(FXint index) const;

    /// Replace the item at index
    FXint setIconItem(FXint index, const FXString& text, FXIcon* icon = nullptr, FXColor bgColor = FXRGB(255, 255, 255), void* ptr = nullptr);

    /// Insert a new item at index
    FXint insertIconItem(FXint index, const FXString& text, FXIcon* icon = nullptr,  FXColor bgColor = FXRGB(255, 255, 255),  void* ptr = nullptr);

    /// @brief append icon
    FXint appendIconItem(const FXString& text, FXIcon* icon = nullptr, FXColor bgColor = FXRGB(255, 255, 255), void* ptr = nullptr);

    /// @brief set Item
    bool setItem(const FXString& text, FXIcon* icon);

    /// Prepend an item to the list
    FXint prependItem(const FXString& text, void* ptr = NULL);

    /// Move item from oldindex to newindex
    FXint moveItem(FXint newindex, FXint oldindex);

    /// Remove this item from the list
    void removeItem(FXint index);

    /// Remove all items from the list
    void clearItems();

    /**
    * Search items by name,  beginning from item start.  If the start item
    * is -1 the search will start at the first item in the list.  Flags
    * may be SEARCH_FORWARD or SEARCH_BACKWARD to control the search
    * direction; this can be combined with SEARCH_NOWRAP or SEARCH_WRAP
    * to control whether the search wraps at the start or end of the list.
    * The option SEARCH_IGNORECASE causes a case-insensitive match.  Finally,
    * passing SEARCH_PREFIX causes searching for a prefix of the item name.
    * Return -1 if no matching item is found.
    */
    FXint findItem(const FXString& text, FXint start = -1, FXuint flags = SEARCH_FORWARD | SEARCH_WRAP) const;

    /**
    * Search items by associated user data,  beginning from item start. If the
    * start item is -1 the search will start at the first item in the list.
    * Flags may be SEARCH_FORWARD or SEARCH_BACKWARD to control the
    * search direction; this can be combined with SEARCH_NOWRAP or SEARCH_WRAP
    * to control whether the search wraps at the start or end of the list.
    */
    FXint findItemByData(const void* ptr, FXint start = -1, FXuint flags = SEARCH_FORWARD | SEARCH_WRAP) const;

    /// Get text for specified item
    FXString getItemText(FXint index) const;

    /// Set data pointer for specified item
    void setItemData(FXint index, void* ptr) const;

    /// Get data pointer for specified item
    void* getItemData(FXint index) const;

    /// Is the pane shown
    FXbool isPaneShown() const;

    /// Sort items using current sort function
    void sortItems();

    /// Set text font
    void setFont(FXFont* fnt);

    /// Get text font
    FXFont* getFont() const;

    /// Set the combobox style.
    void setComboStyle(FXuint mode);

    /// Get the combobox style.
    FXuint getComboStyle() const;

    /// Set window background color
    virtual void setBackColor(FXColor clr);

    /// Get background color
    FXColor getBackColor() const;

    /// Change text color
    void setTextColor(FXColor clr);

    /// Return text color
    FXColor getTextColor() const;

    /// Change selected background color
    void setSelBackColor(FXColor clr);

    /// Return selected background color
    FXColor getSelBackColor() const;

    /// Change selected text color
    void setSelTextColor(FXColor clr);

    /// Return selected text color
    FXColor getSelTextColor() const;

    /// Return sort function
    FXListSortFunc getSortFunc() const;

    /// Change sort function
    void setSortFunc(FXListSortFunc func);

    /// Set the combobox help text
    void setHelpText(const FXString& txt);

    /// Get the combobox help text
    const FXString& getHelpText() const;

    /// Set the tool tip message for this combobox
    void setTipText(const FXString& txt);

    /// Get the tool tip message for this combobox
    const FXString& getTipText() const;

    /// @brief Commands
    /// @{
    long onFocusUp(FXObject*, FXSelector, void*);
    long onFocusDown(FXObject*, FXSelector, void*);
    long onFocusSelf(FXObject*, FXSelector, void*);
    long onMouseWheel(FXObject*, FXSelector, void*);
    long onTextButton(FXObject*, FXSelector, void*);
    long onTextChanged(FXObject*, FXSelector, void*);
    long onTextCommand(FXObject*, FXSelector, void*);
    long onListClicked(FXObject*, FXSelector, void*);
    long onFwdToText(FXObject*, FXSelector, void*);
    long onUpdFmText(FXObject*, FXSelector, void*);
    /// @}

protected:
    /// @brief FOX need this
    MFXIconComboBox();

    /// @brief label for icon
    FXLabel* myIconLabel = nullptr;

    /// @brief textField with icon
    MFXTextFieldIcon* myTextFieldIcon = nullptr;

    /// @brief myButton
    FXMenuButton* myButton = nullptr;

    /// @brief list
    FXList* myList = nullptr;

    /// @brief popup
    FXPopup* myPane = nullptr;

private:
    /// @brief invalidate copy constructor
    MFXIconComboBox(const MFXIconComboBox&);

    /// @brief invalidate assignment operator
    MFXIconComboBox& operator=(const MFXIconComboBox&) = delete;
};
