/****************************************************************************/
/// @file    GNEDeleteFrame.h
/// @author  Pablo Alvarez Lopex
/// @date    Dec 2016
/// @version $Id: GNEDeleteFrame.h 22020 2016-11-16 20:22:20Z palcraft $
///
// The Widget for remove network-elements
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
#ifndef GNEDeleteFrame_h
#define GNEDeleteFrame_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GNEFrame.h"

// ===========================================================================
// class declarations
// ===========================================================================
class GNEAttributeCarrier;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEDeleteFrame
 * The Widget for deleting elements
 */
class GNEDeleteFrame : public GNEFrame{
    /// @brief FOX-declaration
    //FXDECLARE(GNEDeleteFrame)

public:

    /**@brief Constructor
     * @brief parent FXHorizontalFrame in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNEDeleteFrame(FXHorizontalFrame *horizontalFrameParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNEDeleteFrame();


protected:
    /// @brief FOX needs this
    GNEDeleteFrame() {}

private:
   
};


#endif

/****************************************************************************/

