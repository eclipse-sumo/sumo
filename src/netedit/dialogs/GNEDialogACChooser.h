/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDialogACChooser.h
/// @author  Pablo Alvarez Lopez
/// @date    Apr 2018
/// @version $Id$
///
// Class for the window that allows to choose a street, junction or vehicle
/****************************************************************************/

#ifndef GNEDialogACChooser_h
#define GNEDialogACChooser_h


// ===========================================================================
// included modules
// ===========================================================================

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <set>
#include <fx.h>
#include <utils/gui/globjects/GUIGlObject.h>


// ===========================================================================
// class declarations
// ===========================================================================

class GNEAttributeCarrier;
class GNEViewParent;

// ===========================================================================
// class definition
// ===========================================================================
/**
 * @class GNEDialogACChooser
 * Instances of this class are windows that display the list of instances
 * from a given artifact like vehicles, edges or junctions and allow
 * one of their items
 */
class GNEDialogACChooser : public FXMainWindow {
    // FOX-declarations
    FXDECLARE(GNEDialogACChooser)

public:
    /** @brief Constructor
     * @param[in] viewParent GNEViewParent of Netedit
     * @param[in] icon The icon to use
     * @param[in] title The title to use
     * @param[in] ACs list of choosen ACs
     */
    GNEDialogACChooser(GNEViewParent* viewParent, FXIcon* icon, const std::string& title, const std::vector<GNEAttributeCarrier*>& ACs);

    /// @brief Destructor
    ~GNEDialogACChooser();

    /// @name FOX-callbacks
    /// @{

    /// @brief Callback: The selected item shall be centered within the calling view
    long onCmdCenter(FXObject*, FXSelector, void*);

    /// @brief Callback: The dialog shall be closed
    long onCmdClose(FXObject*, FXSelector, void*);

    /// @brief Callback: Something has been typed into the the field
    long onChgText(FXObject*, FXSelector, void*);

    /// @brief Callback: Selects to current item if enter is pressed
    long onCmdText(FXObject*, FXSelector, void*);

    /// @brief Callback: Selects to current item if enter is pressed
    long onListKeyPress(FXObject*, FXSelector, void*);

    /// @brief Callback: Hides unselected items if pressed
    long onCmdFilter(FXObject*, FXSelector, void*);

    /// @brief Callback: Toggle selection status of current object
    long onCmdToggleSelection(FXObject*, FXSelector, void*);
    /// @}

    /// @brief sets the focus after the window is created to work-around bug in libfox
    void show();

protected:
    /// FOX needs this
    GNEDialogACChooser() {}

private:
    /// @brief pointer to GNEViewParent
    GNEViewParent *myViewParent;

    /// @brief The list that holds the ids
    FXList* myList;

    /// @brief The button that triggers centering on the select object
    FXButton* myCenterButton;

    /// @brief The chosen id
    GUIGlObject* mySelected;

    /// @brief The text field
    FXTextField* myTextEntry;

    /// @brief map for saving ACs sorted by ID
    std::set<std::pair<std::string, GNEAttributeCarrier*> > myACsByID;

    /// @brief map for vinculate item list and Attribute carriers
    std::map<int, GNEAttributeCarrier*> myACs;
};


#endif

/****************************************************************************/

