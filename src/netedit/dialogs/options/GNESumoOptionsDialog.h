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
/// @file    GNESumoOptionsDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    May 2023
///
// A Dialog for setting SUMO options
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/dialogs/GNEDialog.h>

// ===========================================================================
// class declaration
// ===========================================================================

class OptionsCont;
class GNEOptionsEditor;

// ===========================================================================
// class definitions
// ===========================================================================

class GNESumoOptionsDialog : public GNEDialog {

public:
    /**@brief Constructor
     *
     * @param[in] applicationWindow netedit application window
     * @param[in] optionsContainer edited option container
     * @param[in] originalOptionsContainer original options container
     */
    GNESumoOptionsDialog(GNEApplicationWindow* applicationWindow, OptionsCont& optionsContainer,
                         const OptionsCont& originalOptionsContainer);

    /// @brief Destructor
    ~GNESumoOptionsDialog();

    /// @brief run internal test
    void runInternalTest(const InternalTestStep::DialogArgument* dialogArgument);

    /// @brief check if option was modified
    bool isOptionModified() const;

    /// @name FOX-callbacks
    /// @{

    /// @brief called when cancel button is pressed (or dialog is closed)
    long onCmdCancel(FXObject*, FXSelector, void*);

    /// @brief called when user press reset button
    long onCmdReset(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief the options editor
    GNEOptionsEditor* myOptionsEditor = nullptr;

private:
    /// @brief Invalidated copy constructor.
    GNESumoOptionsDialog(const GNESumoOptionsDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNESumoOptionsDialog& operator=(const GNESumoOptionsDialog&) = delete;
};
