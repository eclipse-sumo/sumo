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
/// @file    GNEOptionsEditor.h
/// @author  Pablo Alvarez Lopez
/// @date    May 2023
///
// A Dialog for setting options (see OptionsCont)
/****************************************************************************/
#pragma once
#include <config.h>

#include <map>
#include <set>
#include <vector>

#include "GNEOptionsEditorRow.h"

// ===========================================================================
// class declaration
// ===========================================================================

class GNEDialog;
class MFXCheckableButton;
class MFXTextFieldSearch;
class MFXCheckButtonTooltip;
class OptionsCont;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEOptionsEditor : public FXVerticalFrame {
    /// @brief FOX-declaration
    FXDECLARE(GNEOptionsEditor)

    /// @brief declare friend class
    friend class GNEOptionsEditorRow;

public:
    /**@brief Constructor
     *
     * @param[in] dialog GNEDialog in which this editor is shown
     * @param[in] titleName The title to show
     * @param[in] optionsContainer edited option container
     * @param[in] originalOptionsContainer original options container
     */
    GNEOptionsEditor(GNEDialog* dialog, const std::string& titleName, OptionsCont& optionsContainer,
                     const OptionsCont& originalOptionsContainer);

    /// @brief Destructor
    ~GNEOptionsEditor();

    /// @brief check if option was modified
    bool isOptionModified() const;

    /// @brief reset options
    void reset();

    /// @name FOX-callbacks
    /// @{

    /// @brief called when user select a topic in the list
    long onCmdSelectTopic(FXObject*, FXSelector, void*);

    /// @brief called when user searches
    long onCmdSearch(FXObject*, FXSelector, void*);

    /// @brief enable/disable show toolTip
    long onCmdShowToolTipsMenu(FXObject*, FXSelector, void*);

    /// @brief save options
    long onCmdSaveOptions(FXObject*, FXSelector, void*);

    /// @brief load options
    long onCmdLoadOptions(FXObject*, FXSelector, void*);

    /// @brief reset default
    long onCmdResetDefault(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief FOX needs this
    GNEOptionsEditor();

    /// @brief reference to dialog
    GNEDialog* myDialog = nullptr;

    /// @brief reference to edited Option container
    OptionsCont& myOptionsContainer;

    /// @brief reference to original Option container
    const OptionsCont& myOriginalOptionsContainer;

    /// @brief flag for check if options was modified
    bool myOptionsModified = false;

private:
    /// @brief checkable button for show toolTips
    MFXCheckableButton* myShowToolTipsMenu = nullptr;

    /// @brief Topics elements tree
    FXTreeList* myTopicsTreeList = nullptr;

    /// @brief root item
    FXTreeItem* myRootItem = nullptr;

    /// @brief vertical frame for entries
    FXVerticalFrame* myEntriesFrame = nullptr;

    /// @brief checkbox for enable/disable search by description
    MFXCheckButtonTooltip* myDescriptionSearchCheckButton = nullptr;

    /// @brief search button
    MFXTextFieldSearch* mySearchButton = nullptr;

    /// @brief map with topics and their associated FXTreeItem
    std::map<FXTreeItem*, std::string> myTreeItemTopics;

    /// @brief Input option entries
    std::vector<GNEOptionsEditorRow::OptionRow*> myOptionRowEntries;

    /// @brief ignores topics
    const std::set<std::string> myIgnoredTopics = {"Configuration"};

    /// @brief ignores entries
    const std::set<std::string> myIgnoredEntries = {"geometry.remove", "edges.join", "geometry.split", "ramps.guess", "ramps.set"};

    /// @brief update visible entries by selected topic
    bool updateVisibleEntriesByTopic();

    /// @brief update visible entries by search
    void updateVisibleEntriesBySearch(std::string searchText);

    /// @brief load configuration
    bool loadConfiguration(const std::string& file);

    /// @brief Invalidated copy constructor.
    GNEOptionsEditor(const GNEOptionsEditor&) = delete;

    /// @brief Invalidated assignment operator.
    GNEOptionsEditor& operator=(const GNEOptionsEditor&) = delete;
};
