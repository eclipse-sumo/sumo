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
#include <utils/gui/div/GUIDesigns.h>


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

    /// @brief open window
    void open();

    /// @brief hide window
    void hide();

    /// @brief shown
    bool shown() const;

    /// @brief Move the focus to this window
    void setFocus();

    /// @name FOX-callbacks
    /// @{
    /// @brief event after press close button
    long onCmdClose(FXObject*, FXSelector, void*);

    /// @brief event after update command
    long onCmdUpdate(FXObject*, FXSelector, void*);

    /// @}

    /// @brief update data table
    void updateList();

protected:
    /// @brief FOX needs this
    FOX_CONSTRUCTOR(GNEUndoListDialog)

    /// @brief class row
    class Row {

    public:
        /// @brief constructor
        Row(GNEUndoListDialog *undoListDialog, FXVerticalFrame* mainFrame, FXIcon* icon, const std::string &text) {
            FXHorizontalFrame* horizontalFrame = new FXHorizontalFrame(mainFrame, GUIDesignAuxiliarHorizontalFrame);
            // build icon label
            myIcon = new FXLabel(horizontalFrame, "", icon, GUIDesignLabelIconThick);
            // build text label
            myText = new FXLabel(horizontalFrame, text.c_str(), nullptr, GUIDesignLabelLeftThick);
            // create elements
            horizontalFrame->create();
            myIcon->create();
            myText->create();
        }

        /// @brief destructor
        ~Row() {
            delete myIcon;
            delete myText;
        }

    private:
        /// @brief label with icon
        FXLabel* myIcon;
        
        /// @brief label with text
        FXLabel* myText;
    };

    /// @brief pointer to GNEApplicationWindow
    GNEApplicationWindow* myGNEApp;

    /// @brief frame for rows
    FXVerticalFrame* myRowFrame = nullptr;

    /// @brief vector with rows
    std::vector<Row*> myRows;

    /// @brief index for last undo element
    int myLastUndoElement = -1;

private:
    /// @brief Invalidated copy constructor.
    GNEUndoListDialog(const GNEUndoListDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNEUndoListDialog& operator=(const GNEUndoListDialog&) = delete;
};

