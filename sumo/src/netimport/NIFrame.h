/****************************************************************************/
/// @file    NIFrame.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Sets and checks options for netimport
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NIFrame_h
#define NIFrame_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif


// ===========================================================================
// class declarations
// ===========================================================================
/**
 * @class NIFrame
 * @brief Sets and checks options for netimport
 */
class NIFrame {
public:
    /** @brief Inserts options used by the network importer and network building modules
     *
     * Calls "NBNetBuilder::insertNetBuildOptions" for inserting network
     *  building options.
     */
    static void fillOptions();


    /** @brief Checks set options from the OptionsCont-singleton for being valid
     * @return Whether all needed options are set
     * @todo Unused currently; repair/fill
     */
    static bool checkOptions();


};


#endif

/****************************************************************************/

