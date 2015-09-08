/****************************************************************************/
/// @file    GNEChange_Edge.h
/// @author  Jakob Erdmann
/// @date    Mar 2011
/// @version $Id$
///
// A network change in which a single edge is created or deleted
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
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
#include "GNEChange.h"

// ===========================================================================
// class declarations
// ===========================================================================
class GNENet;
class GNEEdge;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEChange_Edge
 * A network change in which a single edge is created or deleted
 */
class GNEChange_Edge : public GNEChange {
    FXDECLARE_ABSTRACT(GNEChange_Edge)

public:
    /** @brief Constructor for creating/deleting an edge
     * @param[in] net The net on which to apply changes
     * @param[in] edge The edge to be created/deleted
     * @param[in] forward Whether to create/delete (true/false)
     */
    GNEChange_Edge(GNENet* net, GNEEdge* edge, bool forward);

    /// @brief Destructor
    ~GNEChange_Edge();

    FXString undoName() const;
    FXString redoName() const;
    void undo();
    void redo();


private:
    /** @brief full information regarding the edge that is to be created/deleted
     * we assume shared responsibility for the pointer (via reference counting)
     */
    GNEEdge* myEdge;
};

#endif
/****************************************************************************/
