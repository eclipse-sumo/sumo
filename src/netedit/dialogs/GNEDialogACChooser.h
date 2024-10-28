/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    GNEDialogACChooser.h
/// @author  Pablo Alvarez Lopez
/// @date    Apr 2018
///
// Class for the window that allows to choose a street, junction or vehicle
/****************************************************************************/

#pragma once
#include <config.h>

#include <utils/gui/windows/GUIDialog_ChooserAbstract.h>


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
class GNEDialogACChooser : public GUIDialog_ChooserAbstract {

public:
    /** @brief Constructor
     * @param[in] viewParent GNEViewParent of Netedit
     * @param[in] icon The icon to use
     * @param[in] title The title to use
     * @param[in] ACs list of choosen ACs
     */
    GNEDialogACChooser(GNEViewParent* viewParent, int messageId, FXIcon* icon, const std::string& title, const std::vector<GNEAttributeCarrier*>& ACs);

    /// @brief Destructor
    ~GNEDialogACChooser();

protected:
    FOX_CONSTRUCTOR(GNEDialogACChooser)

    /// @brief toggle selection
    void toggleSelection(int listIndex) override;

    /// @brief set selection (handled differently in netedit)
    void select(int listIndex) override;

    /// @brief unset selection (handled differently in netedit)
    void deselect(int listIndex) override;

    /// @brief filter ACs
    void filterACs(const std::vector<GUIGlID>& GLIDs) override;

    /// @brief retrieve name for the given object (special case for TLS)
    std::string getObjectName(GUIGlObject* o) const override;

private:
    /// @brief pointer to view parent
    GNEViewParent* myViewParent;

    /// @brief list of displayed ACs
    std::vector<GNEAttributeCarrier*> myACs;

    /// @brief list of filtered ACs
    std::vector<GNEAttributeCarrier*> myFilteredACs;

    /// @brief whether the current locator is for TLS
    bool myLocateTLS;
};
