/****************************************************************************/
/// @file    GNEChange_POI.h
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2017
/// @version $Id$
///
// A network change in which a single POI is created or deleted
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
#ifndef GNEChange_POI_h
#define GNEChange_POI_h


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
class GNEPOI;

// ===========================================================================
// class definitions
// ===========================================================================
/**
* @class GNEChange_POI
* A network change in which a single POI is created or deleted
*/
class GNEChange_POI : public GNEChange {
    FXDECLARE_ABSTRACT(GNEChange_POI)

public:
    /**@brief Constructor
    * @param[in] net net in which POIgon will be inserted
    * @param[in] POI The name of the POIgon
    * @param[in] forward Whether to create/delete (true/false)
    */
    GNEChange_POI(GNENet *net, GNEPOI *POI, bool forward);

    /// @brief Destructor
    ~GNEChange_POI();

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

    /// @brief pointer to POI
    GNEPOI *myPOI;

};

#endif
/****************************************************************************/
