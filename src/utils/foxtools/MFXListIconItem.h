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
/// @file    MFXListIconItem.h
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2023
///
//
/****************************************************************************/

#pragma once
#include <config.h>

#include "fxheader.h"

// ===========================================================================
// class declaration
// ===========================================================================

class MFXListIcon;

// ===========================================================================
// class definitions
// ===========================================================================

class MFXListIconItem : public FXObject {
    /// @brief FOX declaration
    FXDECLARE(MFXListIconItem)

    /// @brief declare friend class
    friend class MFXListIcon;

public:
    enum {
        SELECTED  = 1,      /// Selected
        FOCUS     = 2,      /// Focus
        DISABLED  = 4,      /// Disabled
        DRAGGABLE = 8,      /// Draggable
        ICONOWNED = 16      /// Icon owned by item
    };

    /// @brief Construct new item with given text, icon, and user-data
    MFXListIconItem(const FXString& text, FXIcon* ic = NULL, FXColor backGroundColor = 0, void* ptr = NULL);

    /// @brief Destroy item and free icons if owned
    ~MFXListIconItem();

    /// @brief Change item's text label
    void setText(const FXString& txt);

    /// @brief Return item's text label
    const FXString& getText() const;

    /// @brief Return item's icon
    FXIcon* getIcon() const;

    /// @brief get background color
    const FXColor& getBackGroundColor() const;

    /// @brief Make item draw as focused
    void setFocus(FXbool focus);

    /// @brief Return true if item has focus
    FXbool hasFocus() const;

    /// @brief Select item
    void setSelected(FXbool selected);

    /// @brief Return true if this item is selected
    FXbool isSelected() const;

    /// @brief Enable or disable item
    void setEnabled(FXbool enabled);

    /// @brief Return true if this item is enabled
    FXbool isEnabled() const;

    /// @brief Make item draggable
    void setDraggable(FXbool draggable);

    /// @brief Return true if this item is draggable
    FXbool isDraggable() const;

    /// @brief Return width of item as drawn in list
    FXint getWidth(const MFXListIcon* list) const;

    /// @brief Return height of item as drawn in list
    FXint getHeight(const MFXListIcon* list) const;

    /// @brief Create server-side resources
    void create();

    /// @brief Detach server-side resources
    void detach();

    /// @brief Destroy server-side resources
    void destroy();

protected:
    /// @brief FOX need this
    MFXListIconItem();

    /// @brief daraw
    void draw(const MFXListIcon* list, FXDC&   dc, FXint x, FXint y, FXint w, FXint h);

    /// @brief hit item
    FXint hitItem(const MFXListIcon* list, FXint x, FXint y) const;

    /// @brief label
    FXString label;

    /// @brief icon
    FXIcon* icon = nullptr;

    /// @brief data
    void* data = nullptr;

    /// @brief state
    FXuint state = 0;

    /// @brief position
    FXint x = 0;
    FXint y = 0;

    /// @brief backGround color
    FXColor myBackGroundColor = 0;

    /// @brief flag for show/hidde element
    bool show = true;

private:
    /// @brief invalidate copy constructor
    MFXListIconItem(const MFXListIconItem&) = delete;

    /// @brief invalidate assign constructor
    MFXListIconItem&   operator = (const MFXListIconItem&) = delete;
};
