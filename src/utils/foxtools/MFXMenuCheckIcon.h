/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2004-2023 German Aerospace Center (DLR) and others.
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
/// @file    MFXMenuCheckIcon.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2021
///
//
/****************************************************************************/

#pragma once
#include <config.h>


#include "fxheader.h"
#include <string>

/**
* The menu check widget is used to change a state in the
* application from a menu.  Menu checks may reflect
* the state of the application by graying out, becoming hidden,
* or by a check mark.
* When activated, a menu check sends a SEL_COMMAND to its target;
* the void* argument of the message contains the new state.
*/
class MFXMenuCheckIcon : public FXMenuCommand {
    /// @brief FOX-declaration
    FXDECLARE(MFXMenuCheckIcon)

public:
    /// @brief Construct a menu check
    MFXMenuCheckIcon(FXComposite* p, const std::string& text, const std::string& shortcut, const std::string& info, const FXIcon* icon, FXObject* tgt = NULL, FXSelector sel = 0, FXuint opts = 0);

    /// @brief Return default width
    virtual FXint getDefaultWidth();

    /// @brief Return default height
    virtual FXint getDefaultHeight();

    /// @brief Set check state (TRUE, FALSE or MAYBE)
    void setCheck(FXbool s = TRUE);

    /// @brief Get check state (TRUE, FALSE or MAYBE)
    FXbool getCheck() const;

    /// @brief Get the box background color
    FXColor getBoxColor() const;

    /// @brief Set the box background color
    void setBoxColor(FXColor clr);

    /// @brief Save menu to a stream
    virtual void save(FXStream& store) const;

    /// @brief Load menu from a stream
    virtual void load(FXStream& store);

    /// @name FOX calls
    /// @{
    long onPaint(FXObject*, FXSelector, void*);
    long onButtonPress(FXObject*, FXSelector, void*);
    long onButtonRelease(FXObject*, FXSelector, void*);
    long onKeyPress(FXObject*, FXSelector, void*);
    long onKeyRelease(FXObject*, FXSelector, void*);
    long onHotKeyPress(FXObject*, FXSelector, void*);
    long onHotKeyRelease(FXObject*, FXSelector, void*);
    long onCheck(FXObject*, FXSelector, void*);
    long onUncheck(FXObject*, FXSelector, void*);
    long onUnknown(FXObject*, FXSelector, void*);
    long onCmdSetValue(FXObject*, FXSelector, void*);
    long onCmdSetIntValue(FXObject*, FXSelector, void*);
    long onCmdGetIntValue(FXObject*, FXSelector, void*);
    long onCmdAccel(FXObject*, FXSelector, void*);
    /// @}

protected:
    /// @brief default constructor
    MFXMenuCheckIcon();

    /// @brief Icon
    const FXIcon* myIcon;

    /// @brief State of menu
    FXuchar myCheck;

    /// @brief Box color
    FXColor myBoxColor;

private:
    /// @brief Invalidated copy constructor.
    MFXMenuCheckIcon(const MFXMenuCheckIcon&) = delete;

    /// @brief Invalidated assignment operator.
    MFXMenuCheckIcon& operator=(const MFXMenuCheckIcon&) = delete;
};
