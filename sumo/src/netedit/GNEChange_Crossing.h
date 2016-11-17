/****************************************************************************/
/// @file    GNEChange_Crossing.h
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2016
/// @version $Id$
///
// A network change in which a single crossing is created or deleted
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2016 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GNEChange_Crossing_h
#define GNEChange_Crossing_h


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
class GNECrossing;
class GNEEdge;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEChange_Crossing
 * A network change in which a single crossing is created or deleted
 */
class GNEChange_Crossing : public GNEChange {
    // @brief FOX Declaration
    FXDECLARE_ABSTRACT(GNEChange_Crossing)

public:
    /**@brief Constructor for creating/deleting an crossing
     * @param[in] net The net on which to apply changes
     * @param[in] crossing The crossing to be created/deleted
     * @param[in] forward Whether to create/delete (true/false)
     */
    GNEChange_Crossing(GNENet* net, GNECrossing* crossing, bool forward);

    /// @brief Destructor
    ~GNEChange_Crossing();

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
    /**@brief full information regarding the crossing that is to be created/deleted
     * @note we assume shared responsibility for the pointer (via reference counting)
     */
    GNECrossing* myCrossing;
};

#endif
/****************************************************************************/
