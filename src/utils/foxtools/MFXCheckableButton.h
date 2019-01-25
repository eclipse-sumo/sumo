/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2004-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MFXCheckableButton.h
/// @author  Daniel Krajzewicz
/// @date    2004-03-19
/// @version $Id$
///
// Checkable button similar to a FXButton but mainntain the check
/****************************************************************************/
#ifndef MFXCheckableButton_h
#define MFXCheckableButton_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <fx.h>

/**
 * @class MFXCheckableButton
 */
class MFXCheckableButton : public FXButton {
    /// @brief fox declaration
    FXDECLARE(MFXCheckableButton)

public:
    /// @brief constructor (Very similar to the FXButton constructor)
    MFXCheckableButton(bool amChecked, FXComposite* p, const FXString& text,
                       FXIcon* ic = NULL, FXObject* tgt = NULL, FXSelector sel = 0,
                       FXuint opts = BUTTON_NORMAL,
                       FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0,
                       FXint pl = DEFAULT_PAD, FXint pr = DEFAULT_PAD, FXint pt = DEFAULT_PAD, FXint pb = DEFAULT_PAD);

    /// @brief destructor (Called automatically)
    ~MFXCheckableButton();

    /// @brief check if this MFXCheckableButton is checked
    bool amChecked() const;

    /// @brief check or uncheck this MFXCheckableButton
    void setChecked(bool val);

    /// @name FOX callbacks
    /// @{
    /// @brief called when this MFXCheckableButton is painted
    long onPaint(FXObject*, FXSelector, void*);

    /// @brief called when this MFXCheckableButton is updated
    long onUpdate(FXObject*, FXSelector, void*);
    /// @}

protected:
    /// @brief fox need this
    MFXCheckableButton() {}

private:
    /// @brief build color of this MFXCheckableButton
    void buildColors();

    /// @brief set colors of this MFXCheckableButton
    void setColors();

private:
    /// @brief flag to indicate if this MFXCheckableButton is checked
    bool myAmChecked;

    /// @brief colors of this MFXCheckableButton
    FXColor myBackColor,
            myDarkColor,
            myHiliteColor,
            myShadowColor;

    /// @brief check if this MFXCheckableButton is initialised
    bool myAmInitialised;
};


#endif

/****************************************************************************/

