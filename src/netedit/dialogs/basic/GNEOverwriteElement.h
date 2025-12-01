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
/// @file    GNEOverwriteElement.h
/// @author  Pablo Alvarez Lopez
/// @date    March 2019
///
// Dialog used to ask user if overwrite elements during loading
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/dialogs/GNEDialog.h>

// ===========================================================================
// class declarations
// ===========================================================================

class CommonHandler;
class GNEAttributeCarrier;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEOverwriteElement : public GNEDialog {

public:
    /// @brief Constructor
    GNEOverwriteElement(CommonHandler* commonHandler, const GNEAttributeCarrier* AC, FXWindow* restoringFocusWindow);

    /// @brief destructor
    ~GNEOverwriteElement();

    /// @brief run internal test
    void runInternalTest(const InternalTestStep::DialogArgument* dialogArgument);

    /// @name FOX-callbacks
    /// @{

    /// @brief called when accept or yes button is pressed (can be reimplemented in children)
    long onCmdAccept(FXObject*, FXSelector, void*);

    /// @brief called when cancel or no button is pressed (can be reimplemented in children)
    long onCmdCancel(FXObject*, FXSelector, void*);

    /// @brief called when abort is called either closing dialog or pressing abort button (can be reimplemented in children)
    long onCmdAbort(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief pointer to common handler
    CommonHandler* myCommonHandler = nullptr;

    /// @brief apply solpution to all checkButton
    FXCheckButton* myApplySolutionToAllCheckButon = nullptr;

private:
    /// @brief Invalidated copy constructor.
    GNEOverwriteElement(const GNEOverwriteElement&) = delete;

    /// @brief Invalidated assignment operator.
    GNEOverwriteElement& operator=(const GNEOverwriteElement&) = delete;
};
