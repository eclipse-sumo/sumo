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


// ===========================================================================
// class definitions
// ===========================================================================

template <typename T>
class GNEFixElementsDialog : protected GNEDialog {

public:
    /// @brief GNEFixOptions module
    class FixOptions : public MFXGroupBoxModule {

    public:
        /// @brief constructor
        FixOptions(GNEFixElementsDialog<T> *fixElementDialog, FXVerticalFrame* frameParent, const std::string& title, GUIIcon icon) :
            MFXGroupBoxModule(frameParent, title, MFXGroupBoxModule::Options::NOTHING),
            myFixElementDialogParent(fixElementDialog) {
            // add this fix option to list of fix options
            fixElementDialog->addFixOptions(this);
            // Create table
            myTable = new FXTable(frameParent, this, MID_TABLE, GUIDesignTableFixElements);
            // create frames for options
            FXHorizontalFrame* optionsFrame = new FXHorizontalFrame(frameParent, GUIDesignAuxiliarFrame);
            myLeftFrameOptions = new FXVerticalFrame(optionsFrame, GUIDesignAuxiliarFrame);
            myRightFrameOptions = new FXVerticalFrame(optionsFrame, GUIDesignAuxiliarFrame);
        }

        /// @brief run internal test
        virtual void runInternalTest(const InternalTestStep::DialogArgument* dialogArgument) = 0;

        /// @brief apply fix option
        virtual bool applyFixOption() = 0;

        /// @name FOX-callbacks
        /// @{

        /// @brief called when user select a option
        virtual long onCmdSelectOption(FXObject*, FXSelector, void*) = 0;

        /// @}
    
    protected:
        /// @brief pointer to the parent dialog
        GNEFixElementsDialog *myFixElementDialogParent = nullptr; 

        /// @brief Table with the demand elements
        FXTable* myTable = nullptr;

        /// @brief vertical left frame for options
        FXVerticalFrame* myLeftFrameOptions = nullptr;

        /// @brief vertical right frame for options
        FXVerticalFrame* myRightFrameOptions = nullptr;

    private:
        /// @brief enable options
        virtual void enableOptions() = 0;

        /// @brief disable options
        virtual void disableOptions() = 0;

        /// @brief Invalidated copy constructor.
        FixOptions(const FixOptions&) = delete;

        /// @brief Invalidated assignment operator.
        FixOptions& operator=(const FixOptions&) = delete;
    };

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

    /// @brief open fix additional dialog
    virtual GNEDialog::Result openFixDialog(const std::vector<T*>& invalidElements) = 0;

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
    std::vector<GNEFixElementsDialog::FixOptions<T> > myFixOptions;

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
