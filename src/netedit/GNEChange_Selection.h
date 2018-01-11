/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEChange_Selection.h
/// @author  Jakob Erdmann
/// @date    Mar 2015
/// @version $Id$
///
// A change to the network selection
/****************************************************************************/
#ifndef GNEChange_Selection_h
#define GNEChange_Selection_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>
#include <utils/foxtools/fxexdefs.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include "GNEChange.h"

// ===========================================================================
// class declarations
// ===========================================================================

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEChange_Selection
 * A change to the network selection
 */
class GNEChange_Selection : public GNEChange {
    FXDECLARE_ABSTRACT(GNEChange_Selection)

public:
    /**@brief Constructor for modifying selection
     * @param[in] net net in which apply selection chagnes
     * @param[in] selected The ids to select
     * @param[in] deselected The ids to deselect
     * @param[in] forward Whether to select or deselect the selected ids
     */
    GNEChange_Selection(GNENet* net, const std::set<GUIGlID>& selected, const std::set<GUIGlID>& deselected, bool forward);

    /// @brief Destructor
    ~GNEChange_Selection();

    /// @name inherited from GNEChange
    /// @{
    /// @brief get undo Name
    FXString undoName() const;

    /// @brief get Redo name
    FXString redoName() const;

    /// @brief undo action
    void undo();

    /// @brief redo action
    void redo();
    /// @}


private:
    /// @brief all ids that were selected in this change
    std::set<GUIGlID> mySelectedIDs;

    /// @brief all ids that were deselected in this change
    std::set<GUIGlID> myDeselectedIDs;
};

#endif
/****************************************************************************/
