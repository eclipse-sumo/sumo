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
/// @file    GNEUndoListDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2021
///
// Dialog for show undo-list
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEDialog.h"

// ===========================================================================
// class declarations
// ===========================================================================

class MFXTextFieldIcon;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEUndoListDialog : public GNEDialog {
    /// @brief FOX-declaration
    FXDECLARE(GNEUndoListDialog)

public:
    /// @brief Constructor
    GNEUndoListDialog(GNEApplicationWindow* applicationWindow);

    /// @brief destructor
    ~GNEUndoListDialog();

    /// @brief run internal test
    void runInternalTest(const InternalTestStep::DialogArgument* dialogArgument);

    /// @name FOX-callbacks
    /// @{

    /// @brief event after select row
    long onCmdSelectRow(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @struct class for keep every row value
    struct UndoListRow {
        /// @brief constructor
        UndoListRow(const int index_, FXIcon* icon_, const std::string description_, const std::string timestamp_);

        /// @brief index uses for count undo/redos
        int index = 0;

        /// @brief icon associated with undo/redo operation
        FXIcon* icon = nullptr;

        /// @brief definition of undo/redo operation
        std::string description;

        /// @brief timestamp
        std::string timestamp;
    };

    /// @brief row used for show GUI row elements
    class GUIRow {

    public:
        /// @brief constructor
        GUIRow(GNEUndoListDialog* undoListDialog, FXVerticalFrame* mainFrame, MFXStaticToolTip* staticToolTip);

        /// @brief destructor
        ~GUIRow();

        /// @brief update row
        void update(const UndoListRow& row);

        /// @brief get index
        int getIndex() const;

        /// @brief get radio button (read only)
        const FXRadioButton* getRadioButton() const;

        /// @brief set red background
        void setRedBackground();

        /// @brief set blue background
        void setBlueBackground();

        /// @brief check row and set background green
        void checkRow();

    private:
        /// @brief radioButton
        FXRadioButton* myRadioButton;

        /// @brief index
        int myIndex = 0;

        /// @brief label with icon
        FXLabel* myIcon = nullptr;

        /// @brief textField description
        MFXTextFieldIcon* myTextFieldDescription = nullptr;

        /// @brief textField timeStamp
        FXTextField* myTextFieldTimeStamp = nullptr;
    };

    /// @brief FOX needs this
    FOX_CONSTRUCTOR(GNEUndoListDialog)

    /// @brief update list destroying and creating rows
    void updateList();

    /// @brief frame for rows
    FXVerticalFrame* myRowFrame = nullptr;

    /// @brief vector with rows
    std::vector<GUIRow*> myGUIRows;

private:
    /// @brief Invalidated copy constructor.
    GNEUndoListDialog(const GNEUndoListDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNEUndoListDialog& operator=(const GNEUndoListDialog&) = delete;
};
