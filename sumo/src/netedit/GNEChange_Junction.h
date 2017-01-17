/****************************************************************************/
/// @file    GNEChange_Junction.h
/// @author  Jakob Erdmann
/// @date    Mar 2011
/// @version $Id$
///
// A network change in which a single junction is created or deleted
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GNEChange_Junction_h
#define GNEChange_Junction_h


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
