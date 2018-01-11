/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEChange_Edge.h
/// @author  Jakob Erdmann
/// @date    Mar 2011
/// @version $Id$
///
// A network change in which a single edge is created or deleted
/****************************************************************************/
#ifndef GNEChange_Edge_h
#define GNEChange_Edge_h


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
#include <vector>
#include <map>
#include "GNEChange.h"

// ===========================================================================
// class declarations
// ===========================================================================
class GNENet;
class GNEEdge;
class GNELane;
class GNEAdditional;
class GNERerouter;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEChange_Edge
 * A network change in which a single edge is created or deleted
 */
class GNEChange_Edge : public GNEChange {
    // @brief FOX Declaration
    FXDECLARE_ABSTRACT(GNEChange_Edge)

public:
    /**@brief Constructor for creating/deleting an edge
     * @param[in] edge The edge to be created/deleted
     * @param[in] forward Whether to create/delete (true/false)
     */
    GNEChange_Edge(GNEEdge* edge, bool forward);

    /// @brief Destructor
    ~GNEChange_Edge();

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
    /**@brief full information regarding the edge that is to be created/deleted
     * @note we assume shared responsibility for the pointer (via reference counting)
     */
    GNEEdge* myEdge;
};

#endif
/****************************************************************************/
