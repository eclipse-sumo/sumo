/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNEFixElementsDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2023
///
// Dialog used to fix elements during saving
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/dialogs/GNEDialog.h>
#include <utils/foxtools/MFXGroupBoxModule.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEFixOptions;

// ===========================================================================
// class definitions
// ===========================================================================

template <typename T>
class GNEFixElementsDialog : protected GNEDialog {

public:
    /// @brief Constructor
    GNEFixElementsDialog(GNEApplicationWindow *mainWindow, const std::string title,
                         GUIIcon icon, const int sizeX, const int sizeY):
        GNEDialog(mainWindow, title.c_str(), icon, GNEDialog::Buttons::ACCEPT_CANCEL,
        GUIDesignDialogBoxExplicitStretchable(sizeX, sizeY)) {
        // create left and right frames
        myLeftFrame = new FXVerticalFrame(myContentFrame, GUIDesignAuxiliarFrame);
        myRightFrame = new FXVerticalFrame(myContentFrame, GUIDesignAuxiliarFrame);
    }

    /// @brief destructor
    ~GNEFixElementsDialog() {}

    /// @brief add fix options to the dialog (called automatically during GNEFixOptions constructor)
    void addFixOptions(GNEFixOptions* fixOptions) {
        myFixOptions.push_back(fixOptions);
    }

    /// @brief run internal test
    void runInternalTest(const InternalTestStep::DialogArgument* dialogArgument) {
        // run internal test for each fix option
        for (auto fixOption : myFixOptions) {
            fixOption->runInternalTest(dialogArgument);
        }
    }

    /// @name FOX-callbacks
    /// @{

    /// @brief event after press accept button
    long onCmdAccept(FXObject*, FXSelector, void*) {
        bool abortSaving = false;
        // apply each fix option in their correspond fixOption
        for (auto fixOption : myFixOptions) {
            // if applyFixOption returns false, abort saving (usually for selecting invalid elements)
            if (fixOption->applyFixOption() == false) {
                abortSaving = true;
            }
        }
        // continue depending of abortSaving
        if (abortSaving == false) {
            return closeDialogAccepting();
        } else {
            return closeDialogCanceling();
        }
    }

    /// @}

protected:
    /// @brief vector with all fix options
    std::vector<GNEFixOptions*> myFixOptions;

    /// @brief left frame in which place the FXFixOptions
    FXVerticalFrame* myLeftFrame = nullptr;

    /// @brief right frame in which place the FXFixOptions
    FXVerticalFrame* myRightFrame = nullptr;

private:
    /// @brief Invalidated copy constructor.
    GNEFixElementsDialog(const GNEFixElementsDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNEFixElementsDialog& operator=(const GNEFixElementsDialog&) = delete;
};
