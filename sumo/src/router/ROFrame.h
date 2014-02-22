/****************************************************************************/
/// @file    ROFrame.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @date    Sept 2002
/// @version $Id$
///
// Sets and checks options for routing
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef ROFrame_h
#define ROFrame_h


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
class OptionsCont;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROFrame
 * @brief Sets and checks options for routing
 *
 * Normally, these methods are called from another frame (ROJTRFrame, RODUAFrame)...
 */
class ROFrame {
public:
    /** @brief Inserts options used by routing applications into the OptionsCont-singleton
     * @param[in] oc The options container to fill
     * @param[in] forDuarouter whether duarouter-specific options shall be added
     * @todo Why is the container given? Everywhere else not...
     */
    static void fillOptions(OptionsCont& oc, bool forDuarouter);


    /** @brief Checks whether options are valid
     *
     * To be valid,
     * @arg an output file must be given
     * @arg max-alternatives must not be lower than 2
     *
     * @param[in] oc The options container to fill
     * @return Whether all needed options are set
     * @todo Why is the container given? Everywhere else not...
     * @todo probably, more things should be checked...
     */
    static bool checkOptions(OptionsCont& oc);

};


#endif

/****************************************************************************/

