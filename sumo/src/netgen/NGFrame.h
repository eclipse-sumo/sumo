/****************************************************************************/
/// @file    NGFrame.h
/// @author  Daniel Krajzewicz
/// @date    06.05.2011
/// @version $Id$
///
// Sets and checks options for netgen
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
#ifndef NGFrame_h
#define NGFrame_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NGFrame
 * @brief Sets and checks options for netgen
 */
class NGFrame {
public:
    /** @brief Inserts options used by the network generator
     */
    static void fillOptions();


    /** @brief Checks set options from the OptionsCont-singleton for being valid
     * @return Whether needed (netgen) options are set and have proper values
     */
    static bool checkOptions();


};


#endif

/****************************************************************************/

