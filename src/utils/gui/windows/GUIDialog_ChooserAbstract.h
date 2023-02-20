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
/// @file    GUIDialog_ChooserAbstract.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
///
// Class for the window that allows to choose a street, junction or vehicle
/****************************************************************************/
#pragma once
#include <config.h>

#include <string>
#include <vector>
#include <set>
#include <utils/foxtools/fxheader.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/div/GUIPersistentWindowPos.h>
#include "GUIAppEnum.h"


// ===========================================================================
// class declarations
// ===========================================================================
class GUIGlChildWindow;
class GUIGlObjectStorage;
class GUIGlObject;


// ===========================================================================
// class definition
// ===========================================================================
/**
 * @class GUIDialog_ChooserAbstract
 * Instances of this class are windows that display the list of instances
 * from a given artifact like vehicles, edges or junctions and allow
 * one of their items
 */
class GUIDialog_ChooserAbstract : public FXMainWindow, public GUIPersistentWindowPos {
    // FOX-declarations
    FXDECLARE(GUIDialog_ChooserAbstract)

public:
    /** @brief Constructor
     * @param[in] windowsParent The calling view
     * @param[in] viewParent The calling view (netedit)
     * @param[in] icon The icon to use
     * @param[in] title The title to use
     * @param[in] glStorage The storage to retrieve ids from
     */
    GUIDialog_ChooserAbstract(GUIGlChildWindow* windowsParent, int messageId,
                              FXIcon* icon, const FXString& title, const std::vector<GUIGlID>& ids,
                              GUIGlObjectStorage& glStorage);

    /// @brief Destructor
    virtual ~GUIDialog_ChooserAbstract();

    /** @brief Returns the chosen (selected) object
     * @return The selected object
     */
    GUIGlObject* getObject() const;

    /// @name FOX-callbacks
    /// @{

    /// @brief Callback: The selected item shall be centered within the calling view
    long onCmdCenter(FXObject*, FXSelector, void*);

    /// @brief Callback: The selected vehicle shall be tracked within the calling view
    long onCmdTrack(FXObject*, FXSelector, void*);

    /// @brief Callback: The dialog shall be closed
    long onCmdClose(FXObject*, FXSelector, void*);

    /// @brief Callback: Something has been typed into the the field
    long onChgText(FXObject*, FXSelector, void*);

    /// @brief Callback: Selects to current item if enter is pressed
    long onCmdText(FXObject*, FXSelector, void*);

    /// @brief Callback: Selects to current item if enter is pressed
    long onListKeyPress(FXObject*, FXSelector, void*);

    /// @brief Callback: Current list item has changed
    long onChgList(FXObject*, FXSelector, void*);

    /// @brief Callback: Current list item selection has changed
    long onChgListSel(FXObject*, FXSelector, void*);

    /// @brief Callback: Hides unselected items if pressed
    long onCmdFilter(FXObject*, FXSelector, void*);

    /// @brief Callback: Hides unmatched items if pressed
    long onCmdFilterSubstr(FXObject*, FXSelector, void*);

    /// @brief Callback: Toggle selection status of current object / list
    long onCmdToggleSelection(FXObject*, FXSelector, void*);
    long onCmdAddListSelection(FXObject*, FXSelector, void*);
    long onCmdClearListSelection(FXObject*, FXSelector, void*);

    /// @brief Callback: Toggle locator by name
    long onCmdLocateByName(FXObject*, FXSelector, void*);

    /// @brief Callback: Update list
    long onCmdUpdate(FXObject*, FXSelector, void*);
    /// @}

    /// @brief sets the focus after the window is created to work-around bug in libfox
    void show();

    int getMessageId() const {
        return myMessageId;
    }

protected:
    /// @brief fox need this
    FOX_CONSTRUCTOR(GUIDialog_ChooserAbstract)

    /// @brief toggle selection (handled differently in netedit)
    virtual void toggleSelection(int listIndex);

    /// @brief set selection (handled differently in netedit)
    virtual void select(int listIndex);

    /// @brief unset selection (handled differently in netedit)
    virtual void deselect(int listIndex);

    /// @brief filter ACs (needed in netedit)
    virtual void filterACs(const std::vector<GUIGlID>& GLIDs);

    /// update the list with the given ids
    void refreshList(const std::vector<GUIGlID>& ids);

    /// @bbrief retrieve name for the given object
    virtual std::string getObjectName(GUIGlObject* o) const;

private:
    /// @brief window parent
    GUIGlChildWindow* myWindowsParent;

    /// @brief the object type being chosen
    int myMessageId;

    /// @brief The list that holds the ids
    FXList* myList;

    /// @brief The button that triggers centering on the select object
    FXButton* myCenterButton;

    /// @brief The button that triggers tracking on the select vehicle
    FXButton* myTrackButton;

    /// @brief The chosen id
    GUIGlObject* mySelected;

    /// @brief The text field
    FXTextField* myTextEntry;

    /// @brief myList contains (void) pointers to elements of myIDs instead of the more volatile pointers to GUIGlObject
    std::set<GUIGlID> myIDs;

    /// @brief whether to locate by object name instead of id
    bool myLocateByName;

    /// @brief whether the list was filter by substring
    bool myHaveFilteredSubstring;

    /// @brief label for declaring list size
    FXLabel* myCountLabel;

    /// @brief Whether search is case sensitive
    FXCheckButton* myCaseSensitive;

    /// @brief Whether each change in the list should re-center the view
    FXCheckButton* myInstantCenter;

};
