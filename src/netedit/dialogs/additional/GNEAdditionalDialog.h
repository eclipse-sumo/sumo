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
/// @file    GNEAdditionalDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    April 2016
///
// A abstract class for editing additional elements
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/dialogs/GNEDialog.h>

// ===========================================================================
// class declarations
// ===========================================================================

class GNEAdditional;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEAdditionalDialog : public GNEDialog {

public:
    /// @brief constructor
    GNEAdditionalDialog(GNEAdditional* editedAdditional, const bool updatingElement,
                        const int width, const int height);

    /// @brief destructor
    ~GNEAdditionalDialog();

    /// @brief get edited Additional
    GNEAdditional* getEditedAdditional() const;

    /// @brief run internal test
    virtual void runInternalTest(const InternalTestStep::DialogArgument* dialogArgument) = 0;

    /// @name FOX-callbacks
    /// @{
    /// @brief event after press accept button
    virtual long onCmdAccept(FXObject* sender, FXSelector sel, void* ptr) = 0;

    /// @brief event after press cancel button
    virtual long onCmdCancel(FXObject* sender, FXSelector sel, void* ptr) = 0;

    /// @brief event after press cancel button
    virtual long onCmdReset(FXObject*, FXSelector, void*) = 0;

    /// @}

protected:
    /// @brief default constructor
    GNEAdditionalDialog();

    /// @brief pointer to edited additional
    GNEAdditional* myEditedAdditional = nullptr;

    /// @brief flag to indicate if additional are being created or modified (cannot be changed after open dialog)
    bool myUpdatingElement = false;

    /// @brief change additional dialog header
    void changeAdditionalDialogHeader(const std::string& newHeader);

    /// @brief init a new group of changes that will be do it in dialog
    void initChanges();

    /// @brief Accept changes did in this dialog.
    void acceptChanges();

    /// @brief Cancel changes did in this dialog.
    void cancelChanges();

    /// @brief reset changes did in this dialog.
    void resetChanges();

private:
    /// @brief description of changes did in this additional dialog
    std::string myChangesDescription;

    /// @brief number of GNEChanges_... in dialog
    int myNumberOfChanges;

    /// @brief Invalidated copy constructor
    GNEAdditionalDialog(const GNEAdditionalDialog&) = delete;

    /// @brief Invalidated assignment operator
    GNEAdditionalDialog& operator=(const GNEAdditionalDialog&) = delete;
};
