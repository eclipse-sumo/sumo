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
/// @file    MFXCheckableButton.h
/// @author  Daniel Krajzewicz
/// @date    2004-03-19
///
// Checkable button similar to a FXButton but maintain the check
/****************************************************************************/
#pragma once
#include <config.h>

#include <string>

#include "fxheader.h"
#include "MFXStaticToolTip.h"

/**
 * @class MFXCheckableButton
 */
class MFXCheckableButton : public FXButton {
    /// @brief fox declaration
    FXDECLARE(MFXCheckableButton)

public:
    /// @brief constructor (Very similar to the FXButton constructor)
    MFXCheckableButton(bool amChecked, FXComposite* p, MFXStaticToolTip* staticToolTip, const std::string& text,
                       FXIcon* ic = NULL, FXObject* tgt = NULL, FXSelector sel = 0, FXuint opts = BUTTON_NORMAL,
                       FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0,
                       FXint pl = DEFAULT_PAD, FXint pr = DEFAULT_PAD, FXint pt = DEFAULT_PAD, FXint pb = DEFAULT_PAD);

    /// @brief destructor (Called automatically)
    ~MFXCheckableButton();

    /// @brief check if this MFXCheckableButton is checked
    bool amChecked() const;

    /// @brief check or uncheck this MFXCheckableButton
    void setChecked(bool val, const bool inform = false);

    /// @name FOX callbacks
    /// @{
    /// @brief called when this MFXCheckableButton is painted
    long onPaint(FXObject*, FXSelector, void*);

    /// @brief called when this MFXCheckableButton is updated
    long onUpdate(FXObject*, FXSelector, void*);

    /// @brief called when mouse enter in MFXCheckableButton
    long onEnter(FXObject*, FXSelector, void*);

    /// @brief called when mouse leaves in MFXCheckableButton
    long onLeave(FXObject*, FXSelector, void*);

    /// @brief called when mouse motion in MFXCheckableButton
    long onMotion(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief FOX need this
    FOX_CONSTRUCTOR(MFXCheckableButton)

    /// @brief flag to indicate if this MFXCheckableButton is checked
    bool myAmChecked;

    /// @brief colors of this MFXCheckableButton
    FXColor myBackColor,
            myDarkColor,
            myHiliteColor,
            myShadowColor;

    /// @brief check if this MFXCheckableButton is initialised
    bool myAmInitialised;

    /// @brief static tool tip
    MFXStaticToolTip* myStaticToolTip = nullptr;

    /// @brief build color of this MFXCheckableButton
    void buildColors();

    /// @brief set colors of this MFXCheckableButton
    void setColors();

private:
    /// @brief Invalidated copy constructor.
    MFXCheckableButton(const MFXCheckableButton&) = delete;

    /// @brief Invalidated assignment operator.
    MFXCheckableButton& operator=(const MFXCheckableButton&) = delete;
};
