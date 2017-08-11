/****************************************************************************/
/// @file    GNEChange_Poly.h
/// @author  Jakob Erdmann
/// @date    Mar 2011
/// @version $Id$
///
// A network change in which a single poly is created or deleted
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
#ifndef GNEChange_Poly_h
#define GNEChange_Poly_h


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
class GNEPoly;

// ===========================================================================
// class definitions
// ===========================================================================
/**
* @class GNEChange_Poly
* A network change in which a single poly is created or deleted
*/
class GNEChange_Poly : public GNEChange {
    FXDECLARE_ABSTRACT(GNEChange_Poly)

public:
    /**@brief Constructor
    * @param[in] net net in which polygon will be inserted
    * @param[in] poly The name of the polygon
    * @param[in] forward Whether to create/delete (true/false)
    */
    GNEChange_Poly(GNENet *net, GNEPoly *poly, bool forward);

    /// @brief Destructor
    ~GNEChange_Poly();

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

    /// @brief pointer to polygon
    GNEPoly *myPoly;
    
};

#endif
/****************************************************************************/
