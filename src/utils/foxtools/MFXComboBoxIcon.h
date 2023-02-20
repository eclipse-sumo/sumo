/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
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
/// @file    MFXComboBoxIcon.h
/// @author  Jakob Erdmann
/// @author  Pablo Alvarez Lopez
/// @date    2018-12-19
///
//
/****************************************************************************/

#pragma once
#include <config.h>

#include "MFXTextFieldIcon.h"
#include "MFXListIcon.h"


/// @brief ComboBox with icon
class MFXComboBoxIcon : public FXPacker {
    /// @brief FOX declaration
    FXDECLARE(MFXComboBoxIcon)

public:
    /// @brief enum for ID
    enum {
        ID_LIST = FXPacker::ID_LAST,
        ID_TEXT,
        ID_LAST
    };

    /// @brief Construct a Combo Box widget with room to display cols columns of text
    MFXComboBoxIcon(FXComposite* p, FXint cols, const bool haveIcons, FXObject* tgt = NULL, FXSelector sel = 0, FXuint opts = COMBOBOX_NORMAL, FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0, FXint pl = DEFAULT_PAD, FXint pr = DEFAULT_PAD, FXint pt = DEFAULT_PAD, FXint pb = DEFAULT_PAD);

    /// @brief Destructor
    virtual ~MFXComboBoxIcon();

    /// @brief Create server-side resources
    virtual void create();

    /// @brief Detach server-side resources
    virtual void detach();

    /// @brief Destroy server-side resources
    virtual void destroy();

    /// @brief Enable combo box
    virtual void enable();

    /// @brief Disable combo box
    virtual void disable();

    /// @brief Return default width
    virtual FXint getDefaultWidth();

    /// @brief Return default height
    virtual FXint getDefaultHeight();

    /// @brief Perform layout
    virtual void layout();

    /// @brief Return true if combobox is editable
    FXbool isEditable() const;

    /// @brief Set editable state
    void setEditable(FXbool edit = TRUE);

    /// @brief Get the text
    FXString getText() const;

    /// @brief Set the number of columns
    void setNumColumns(FXint cols);

    /// @brief Get the number of columns
    FXint getNumColumns() const;

    /// @brief Change text justification mode; default is JUSTIFY_LEFT
    void setJustify(FXuint mode);

    /// @brief Return text justification mode
    FXuint getJustify() const;

    /// @brief Return the number of items in the list
    FXint getNumItems() const;

    /// @brief Return the number of visible items
    FXint getNumVisible() const;

    /// @brief Set text
    void setText(FXString text);

    /// @brief Set the number of visible items in the drop down list
    void setNumVisible(FXint nvis);

    /// @brief Return true if current item
    FXbool isItemCurrent(FXint index) const;

    /// @brief Set the current item (index is zero-based)
    void setCurrentItem(FXint index, FXbool notify = FALSE);

    /// @brief Get the current item's index
    FXint getCurrentItem() const;

    /// @brief Return the item at the given index
    FXString getItem(FXint index) const;

    /// @brief Replace the item at index
    FXint setIconItem(FXint index, const FXString& text, FXIcon* icon = nullptr, FXColor bgColor = FXRGB(255, 255, 255), void* ptr = nullptr);

    /// @brief Insert a new item at index
    FXint insertIconItem(FXint index, const FXString& text, FXIcon* icon = nullptr,  FXColor bgColor = FXRGB(255, 255, 255),  void* ptr = nullptr);

    /// @brief append icon
    FXint appendIconItem(const FXString& text, FXIcon* icon = nullptr, FXColor bgColor = FXRGB(255, 255, 255), void* ptr = nullptr);

    /// @brief set Item
    bool setItem(const FXString& text, FXIcon* icon);

    /// @brief set custom text
    void setCustomText(const FXString text);

    /// @brief Prepend an item to the list
    FXint prependItem(const FXString& text, void* ptr = NULL);

    /// @brief Move item from oldindex to newindex
    FXint moveItem(FXint newindex, FXint oldindex);

    /// @brief Remove this item from the list
    void removeItem(FXint index);

    /// @brief Remove all items from the list
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

    /// @brief Get text for specified item
    FXString getItemText(FXint index) const;

    /// @brief Set data pointer for specified item
    void setItemData(FXint index, void* ptr) const;

    /// @brief Get data pointer for specified item
    void* getItemData(FXint index) const;

    /// @brief Is the pane shown
    FXbool isPaneShown() const;

    /// @brief Sort items using current sort function
    void sortItems();

    /// @brief Set text font
    void setFont(FXFont* fnt);

    /// @brief Get text font
    FXFont* getFont() const;

    /// @brief Set the combobox style.
    void setComboStyle(FXuint mode);

    /// @brief Get the combobox style.
    FXuint getComboStyle() const;

    /// @brief Set window background color
    virtual void setBackColor(FXColor clr);

    /// @brief Get background color
    FXColor getBackColor() const;

    /// @brief Change text color
    void setTextColor(FXColor clr);

    /// @brief Return text color
    FXColor getTextColor() const;

    /// @brief Change selected background color
    void setSelBackColor(FXColor clr);

    /// @brief Return selected background color
    FXColor getSelBackColor() const;

    /// @brief Change selected text color
    void setSelTextColor(FXColor clr);

    /// @brief Return selected text color
    FXColor getSelTextColor() const;

    /// @brief Return sort function
    FXListSortFunc getSortFunc() const;

    /// @brief Change sort function
    void setSortFunc(FXListSortFunc func);

    /// @brief Set the combobox help text
    void setHelpText(const FXString& txt);

    /// @brief Get the combobox help text
    const FXString& getHelpText() const;

    /// @brief Set the tool tip message for this combobox
    void setTipText(const FXString& txt);

    /// @brief Get the tool tip message for this combobox
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
    MFXComboBoxIcon();

    /// @brief label for icon
    FXLabel* myIconLabel = nullptr;

    /// @brief textField with icon
    MFXTextFieldIcon* myTextFieldIcon = nullptr;

    /// @brief myButton
    FXMenuButton* myButton = nullptr;

    /// @brief list
    MFXListIcon* myList = nullptr;

    /// @brief popup
    FXPopup* myPane = nullptr;

    /// @brief check if this iconComboBox have icons
    const bool myHaveIcons;

private:
    /// @brief invalidate copy constructor
    MFXComboBoxIcon(const MFXComboBoxIcon&);

    /// @brief invalidate assignment operator
    MFXComboBoxIcon& operator=(const MFXComboBoxIcon&) = delete;
};
