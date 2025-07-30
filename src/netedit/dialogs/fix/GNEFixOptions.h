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
/// @file    GNEFixOptions.h
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2025
///
// Abstract groupbox module used in GNEFixElementsDialog to fix elements
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/foxtools/MFXGroupBoxModule.h>
#include <utils/gui/images/GUIIcons.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEFixElementsDialog;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEFixOptions : public MFXGroupBoxModule {
    /// @brief FOX-declaration
    FXDECLARE_ABSTRACT(GNEFixOptions)

public:
    /// @brief constructor
    GNEFixOptions(GNEFixElementsDialog *fixElementDialog, FXVerticalFrame* frameParent, const std::string& title, GUIIcon icon);

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
    /// @brief FOX needs this
    FOX_CONSTRUCTOR(GNEFixOptions)

    /// @brief save contents
    bool saveContents() const;

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
    GNEFixOptions(const GNEFixOptions&) = delete;

    /// @brief Invalidated assignment operator.
    GNEFixOptions& operator=(const GNEFixOptions&) = delete;
};