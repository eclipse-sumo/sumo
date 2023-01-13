/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    GUISaveDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2022
///
// Dialog for ask user about saving elements
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/foxtools/fxheader.h>


/**
* A Message Box is a convenience class which provides a dialog for
* very simple common yes/no type interactions with the user.
* The message box has an optional icon, a title string, and the question
* which is presented to the user.  It also has up to three buttons which
* furnish standard responses to the question.
* Message boxes are usually ran modally: the question must be answered
* before the program may continue.
*/
class GUISaveDialog : public FXDialogBox {
    FXDECLARE(GUISaveDialog)

public:
    /// @brief enums
    enum {
        CLICKED_SAVE = FXDialogBox::ID_LAST,
        CLICKED_DISCARD,
        CLICKED_ABORT,
    };

    /// @brief Construct free floating message box with given caption, icon, and message text
    GUISaveDialog(FXApp* app, const FXString& caption, const FXString& text, FXIcon* ic);

    /// @brief called when user click over button
    long onCmdClicked(FXObject*, FXSelector, void*);

    /// @brief called when user press cancel
    long onCmdCancel(FXObject*, FXSelector, void*);

    /// @brief Show modal question message, in free floating window.
    static FXuint question(FXApp* app, const char* caption, const char* message, ...);

protected:
    /// @brief FOX need this
    GUISaveDialog() {}

private:
    /// @brief initialize save dialog
    void initialize(const FXString& text, FXIcon* ic);

    /// @brief question icon
    static const unsigned char myQuestionIcon[];

    /// @brief invalidate default constructor
    GUISaveDialog(const GUISaveDialog&) = delete;

    /// @brief invalidate copy operator
    GUISaveDialog& operator=(const GUISaveDialog&) = delete;
};
