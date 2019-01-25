/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEChange_Junction.h
/// @author  Jakob Erdmann
/// @date    Mar 2011
/// @version $Id$
///
// A network change in which a single junction is created or deleted
/****************************************************************************/
#ifndef GNEChange_Junction_h
#define GNEChange_Junction_h


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
class GNENet;
class GNEJunction;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEChange_Junction
 * A network change in which a single junction is created or deleted
 */
class GNEChange_Junction : public GNEChange {
    FXDECLARE_ABSTRACT(GNEChange_Junction)

public:
    /**@brief Constructor for creating/deleting a junction
     * @param[in] junction The junction to be created/deleted
     * @param[in] forward Whether to create/delete (true/false)
     */
    GNEChange_Junction(GNEJunction* junction, bool forward);

    /// @brief Destructor
    ~GNEChange_Junction();

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
    /**@brief full information regarding the junction that is to be created/deleted
     * @note we assume shared responsibility for the pointer (via reference counting)
     */
    GNEJunction* myJunction;
};

#endif
/****************************************************************************/
