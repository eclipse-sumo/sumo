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
/// @file    GNEOptionsDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    May 2023
///
// A Dialog for setting options (see OptionsCont)
/****************************************************************************/
#pragma once
#include <config.h>

#include <set>
#include <utils/gui/images/GUIIcons.h>
#include <utils/foxtools/MFXCheckableButton.h>
#include <utils/foxtools/MFXCheckButtonTooltip.h>
#include <utils/foxtools/MFXTextFieldSearch.h>

#include "GNEOptionsDialogElements.h"

// ===========================================================================
// class declaration
// ===========================================================================

class GUIMainWindow;
class OptionsCont;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEOptionsDialog
 * @briefA Dialog for setting options (see OptionsCont)
 */
class GNEOptionsDialog : public FXDialogBox {
    /// @brief FOX-declaration
    FXDECLARE(GNEOptionsDialog)

    /// @brief declare friend class
    friend class GNEOptionsDialogElements;

public:
    /**@brief Constructor for options dialogs
     *
     * @param[in] parent The parent window
     * @param[in] optionsContainer edited option container
     * @param[in] icon windows icon
     * @param[in] name The title to show
     * @return pair with int (TRUE, FALSE) depending of execution, and bool for check if options were modified
     */
    static std::pair<int, bool> Options(GUIMainWindow* windows, GUIIcon icon, OptionsCont* optionsContainer, const char* titleName);

    /**@brief Constructor for run dialogs
     *
     * @param[in] parent The parent window
     * @param[in] optionsContainer edited option container
     * @param[in] icon windows icon
     * @param[in] name The title to show
     * @return pair with int (TRUE, FALSE) depending of execution, and bool for check if options were modified
     */
    static std::pair<int, bool> Run(GUIMainWindow* windows, GUIIcon icon, OptionsCont* optionsContainer, const char* titleName);

    /// @brief Destructor
    ~GNEOptionsDialog();

    /// @brief called when user press run netgenerate button
    long onCmdRunNetgenerate(FXObject*, FXSelector, void*);

    /// @brief called when user searchs
    long onCmdSearch(FXObject*, FXSelector, void*);

    /// @brief called when user press checkbox "use description"
    long onCmdUseDescription(FXObject*, FXSelector, void*);

protected:
    /// @brief FOX needs this
    FOX_CONSTRUCTOR(GNEOptionsDialog)

    /// @brief pointer to main window parent
    GUIMainWindow* myMainWindowParent;

    /// @brief refecente to edited Option container
    OptionsCont* myOptionsContainer;

    /// @brief flag for check if value was modified
    bool myModified = false;

private:
    /// @brief checkable button for show toolTips
    MFXCheckableButton* myShowToolTipsMenu = nullptr; 

    /// @brief Topics elements tree
    FXTreeList* myTopicsTreeList = nullptr;

    /// @brief vertical farme for entries
    FXVerticalFrame* myEntriesFrame = nullptr;

    /// @brief checkbox for enable/disable search by description
    MFXCheckButtonTooltip* myDescriptionSearchCheckButton = nullptr;

    /// @brief search button
    MFXTextFieldSearch* mySearchButton = nullptr;

    /// @brief Input option entries
    std::vector<GNEOptionsDialogElements::InputOption*> myInputOptionEntries;

    /// @brief ignores topics
    const std::set<std::string> myIgnoredTopics = {"Configuration"};

    /// @brief ignores entrys
    const std::set<std::string> myIgnoredEntries = {"geometry.remove", "edges.join", "geometry.split", "ramps.guess", "ramps.set"};

    /// @brief update visible entries by selected topic
    void updateVisibleEntriesByTopic();

    /// @brief update visible entries by search
    void updateVisibleEntriesBySearch(std::string searchText);

    /**@brief Constructor
     *
     * @param[in] parent The parent window
     * @param[in] optionsContainer edited option container
     * @param[in] titleName The title to show
     * @param[in] icon windows icon
     * @param[in] runDialog check if this is a run dialog
     */
    GNEOptionsDialog(GUIMainWindow* parent, GUIIcon icon, OptionsCont* optionsContainer, const char* titleName, const bool runDialog);

    /// @brief Invalidated copy constructor.
    GNEOptionsDialog(const GNEOptionsDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNEOptionsDialog& operator=(const GNEOptionsDialog&) = delete;
};
