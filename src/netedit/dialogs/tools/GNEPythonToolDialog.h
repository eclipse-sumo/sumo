/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    GNEPythonToolDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2022
///
// Dialog for tools
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/options/OptionsCont.h>

#include "GNEPythonToolDialogElements.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNEApplicationWindow;
class GNEPythonTool;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEPythonToolDialog
 * @brief Dialog for python tool dialog
 */
class GNEPythonToolDialog : protected FXDialogBox {
    /// @brief FOX-declaration
    FXDECLARE(GNEPythonToolDialog)

    /// @brief declare friend class
    friend class GNEPythonToolDialogElements;

public:
    /// @brief Constructor
    GNEPythonToolDialog(GNEApplicationWindow* GNEApp);

    /// @brief destructor
    ~GNEPythonToolDialog();

    /// @brief open dialog
    long openDialog(GNEPythonTool* tool);

    /// @brief get pointer to GNEApplicationWindow
    GNEApplicationWindow* getGNEApplicationWindow() const;

    /// @brief get python tool
    const GNEPythonTool* getPythonTool() const;

    /// @name FOX-callbacks
    /// @{

    /// @brief save options
    long onCmdSave(FXObject*, FXSelector, void*);

    /// @brief load options
    long onCmdLoad(FXObject*, FXSelector, void*);

    /// @brief set visualization (sorting and grouping)
    long onCmdSetVisualization(FXObject*, FXSelector, void*);

    /// @brief event after press run button
    long onCmdRun(FXObject*, FXSelector, void*);

    /// @brief event after press cancel button
    long onCmdCancel(FXObject*, FXSelector, void*);

    /// @brief event after press reset button
    long onCmdReset(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief internal class used for sorting options by categories
    class CategoryOptions : public std::string {

    public:
        /// @brief constructor
        CategoryOptions(const std::string &category);

        /// @brief add option
        void addOption(const std::string &name, Option* option);

        /// @brief get options
        const std::vector<std::pair<std::string, Option*> > &getOptions() const;

        /// @brief sort options by name
        void sortByName();

    private:
        /// @brief options for this category
        std::vector<std::pair<std::string, Option*> > myOptions;

        /// @brief default constructor
        CategoryOptions() {}
    };

    /// @brief FOX needs this
    GNEPythonToolDialog();

    /// @brief build arguments
    virtual void buildArguments(bool sortByName, bool groupedByCategories);

    /// @brief adjust parameter column
    void adjustParameterColumn();

    /// @brief get options
    std::vector<GNEPythonToolDialog::CategoryOptions> getOptions(OptionsCont& optionsCont) const;

    /// @brief get options sorted by category
    std::vector<CategoryOptions> getOptionsByCategories(OptionsCont& optionsCont) const;

    /// @brief get number of row colums
    int getNumRowColums() const;

    /// @brief get row frame
    FXVerticalFrame* getRowFrame();

    /// @brief list of arguments sorted by categories
    std::vector<GNEPythonToolDialogElements::Argument*> myArguments;

    /// @brief list of categories
    std::vector<GNEPythonToolDialogElements::Category*> myCategories;

private:
    /// @brief pointer to GNEApplicationWindow
    GNEApplicationWindow* myGNEApp;

    /// @brief vertical frames for rows
    std::vector<FXVerticalFrame*> myRowFrames;

    /// @brief options frame (Save, load, sorting and grouping)
    FXHorizontalFrame* myOptionsFrame = nullptr;

    /// @brief check button to enable/diasble sorting
    FXCheckButton* mySortedCheckButton = nullptr;

    /// @brief check button to enable/diasble grouping
    FXCheckButton* myGroupedCheckButton = nullptr;

    /// @brief python tool
    GNEPythonTool* myPythonTool = nullptr;

    /// @brief parameter label
    FXLabel* myParameterLabel = nullptr;

    /// @brief separator
    FXSeparator* mySeparator = nullptr;

    /// @brief buttons frame
    FXHorizontalFrame* myButtonsFrame = nullptr;
    
    /// @brief Invalidated copy constructor.
    GNEPythonToolDialog(const GNEPythonToolDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNEPythonToolDialog& operator=(const GNEPythonToolDialog&) = delete;
};

