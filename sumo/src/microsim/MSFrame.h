/****************************************************************************/
/// @file    MSFrame.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// Sets and checks options for microsim; inits global outputs and settings
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
#ifndef MSFrame_h
#define MSFrame_h


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
class MSNet;
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSFrame
 * @brief Sets and checks options for microsim; inits global outputs and settings
 *
 * In addition to setting and checking options, this frame also sets global
 *  values via "setMSGlobals". They are stored in MSGlobals.
 *
 * Also, global output streams are initialised within "buildStreams".
 *
 * @see MSGlobals
 */
class MSFrame {
public:
    /** @brief Inserts options used by the simulation into the OptionsCont-singleton
     *
     * Device-options are inserted by calling the device's "insertOptions"
     *  -methods.
     */
    static void fillOptions();


    /** @brief Builds the streams used possibly by the simulation */
    static void buildStreams();


    /** @brief Checks the set options.
     *
     * The following constraints must be valid:
     * @arg the network-file was specified (otherwise no simulation is existing)
     * @arg the begin and the end of the simulation must be given
     * @arg The default lane change model must be known
     * If one is not, false is returned.
     *
     * @return Whether the settings are valid
     * @todo Rechek usage of the lane change model
     * @todo probably, more things should be checked...
     */
    static bool checkOptions();


    /** @brief Sets the global microsim-options
     *
     * @param[in] oc The options container to get the values from
     * @see MSGlobals
     */
    static void setMSGlobals(OptionsCont& oc);

};


#endif

/****************************************************************************/

