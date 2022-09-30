/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2022 German Aerospace Center (DLR) and others.
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

#include <utils/foxtools/MFXTreeListDynamic.h>


// ===========================================================================
// class declarations
// ===========================================================================

class GNEUndoList;
class GNEApplicationWindow;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEUndoListDialog
 * @brief Dialog for edit rerouters
 */
class GNEUndoListDialog : protected FXTopWindow {
    /// @brief FOX-declaration
    FXDECLARE(GNEUndoListDialog)

public:
    /// @brief Constructor
    GNEUndoListDialog(GNEApplicationWindow* GNEApp);

    /// @brief destructor
    ~GNEUndoListDialog();

    /// @brief show window
    void show();

    /// @brief hide window
    void hide();

    /// @brief check if dialog is shown
    bool shown() const;

    /// @brief Move the focus to this window
    void setFocus();

    /// @name FOX-callbacks
    /// @{
    /// @brief event after press close button
    long onCmdClose(FXObject*, FXSelector, void*);

    /// @brief event after select row
    long onCmdSelectRow(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief FOX needs this
    FOX_CONSTRUCTOR(GNEUndoListDialog)

    /// @brief update list destroying and creating rows
    void updateList();

    /// @brief recalc list destroying and creating rows
    void recalcList();

    /// @struct class for keep every row value
    struct TemporalRow {
        /// @brief constructor
        TemporalRow(const int index_, FXIcon* icon_, const std::string text_);

        /// @brief index uses for count undo/redos
        const int index;

        /// @brief icon associated with undo/redo operation
        FXIcon* icon;

        /// @brief definition of undo/redo operation
        const std::string text;
    };

    /// @brief class row
    class Row {

    public:
        /// @brief constructor
        Row(GNEUndoListDialog* undoListDialog, FXVerticalFrame* mainFrame);

        /// @brief destructor
        ~Row();

        /// @brief update row
        void update(const TemporalRow &row);

        /// @brief get index
        int getIndex() const;

        /// @brief get radio button (read only)
        const FXRadioButton* getRadioButton() const;

        /// @brief set red background
        void setRedBackground();

        /// @brief set blue blackground
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
        
        /// @brief textField
        FXTextField* myTextField = nullptr;
    };

    /// @brief pointer to GNEApplicationWindow
    GNEApplicationWindow* myGNEApp;

    /// @brief frame for rows
    FXVerticalFrame* myRowFrame = nullptr;

    /// @brief vector with rows
    std::vector<Row*> myRows;

private:
    /// @brief Invalidated copy constructor.
    GNEUndoListDialog(const GNEUndoListDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNEUndoListDialog& operator=(const GNEUndoListDialog&) = delete;
};

