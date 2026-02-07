/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
/// @file    GNERunNetgenerateDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2023
///
// Dialog for running tools
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNERunDialog.h"

// ===========================================================================
// class declarations
// ===========================================================================

class OptionsCont;

// ===========================================================================
// class definitions
// ===========================================================================

class GNERunNetgenerateDialog : public GNERunDialog {

public:
    /// @brief Constructor
    GNERunNetgenerateDialog(GNEApplicationWindow* applicationWindow, const OptionsCont* netgenerateOptions);

    /// @brief destructor
    ~GNERunNetgenerateDialog();

    /// @brief run internal test
    void runInternalTest(const InternalTestStep::DialogArgument* dialogArgument);

    /// @brief get run command
    std::string getRunCommand() const;

    /// @name FOX-callbacks
    /// @{

    /// @brief event after press back button
    long onCmdBack(FXObject*, FXSelector, void*);

    /// @brief event after press close button
    long onCmdAccept(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief netgenerate options
    const OptionsCont* myNetgenerateOptions = nullptr;

private:
    /// @brief Invalidated copy constructor.
    GNERunNetgenerateDialog(const GNERunNetgenerateDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNERunNetgenerateDialog& operator=(const GNERunNetgenerateDialog&) = delete;
};
