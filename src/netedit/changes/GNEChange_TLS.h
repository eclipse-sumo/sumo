/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEChange_TLS.h
/// @author  Jakob Erdmann
/// @date    July 2011
/// @version $Id$
///
// A network change in which a traffic light is created or deleted
/****************************************************************************/
#ifndef GNEChange_TLS_h
#define GNEChange_TLS_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <fx.h>
#include <utils/foxtools/fxexdefs.h>
#include "GNEChange.h"

// ===========================================================================
// class declarations
// ===========================================================================
class NBTrafficLightDefinition;
class GNEJunction;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEChange_TLS
 * A network change in which a traffic light is created or deleted
 */
class GNEChange_TLS : public GNEChange {
    FXDECLARE_ABSTRACT(GNEChange_TLS)

public:
    /**@brief Constructor for creating/deleting an edge
     * @param[in] junction The junction to which the traffic light belong
     * @param[in] tlDef The traffic light definition (may be 0 on creation)
     * @param[in] forward Whether to create/delete (true/false)
     * @param[in] tlID The id for the newly created tlDef (set to junction id if * "" is given)
     */
    GNEChange_TLS(GNEJunction* junction, NBTrafficLightDefinition* tlDef, bool forward, bool forceInsert = false, const std::string tlID = "");

    /// @brief Destructor
    ~GNEChange_TLS();

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
    /**@brief we need the junction because it is the target of our change commands
     * @note we assume shared responsiblity for the junction via reference counting
     */
    GNEJunction* myJunction;

    /**@brief the traffic light to be created/deleted. We assume no responsiblity for the pointer
     * @note since it is hard to track by which NBnodes a tlDef is used (may be more than one).
     */
    NBTrafficLightDefinition* myTlDef;

    /// @brief check if forceInsert is enabled
    bool myForceInsert;
};

#endif
/****************************************************************************/
