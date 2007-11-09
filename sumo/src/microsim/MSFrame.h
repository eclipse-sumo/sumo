/****************************************************************************/
/// @file    SUMOFrame.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id:SUMOFrame.h 4686 2007-11-09 13:04:13Z dkrajzew $
///
// Some helping methods for usage within sumo and sumo-gui
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef SUMOFrame_h
#define SUMOFrame_h


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
 * @class SUMOFrame
 * This class holds some helping methods needed both by the sumo and the
 * sumo-gui module.
 */
class SUMOFrame
{
public:
    /// Builds the simulation options
    static void fillOptions();

    /** @brief Builds the streams used possibly by the simulation */
    static void buildStreams();

    /**
     * Checks the build settings. The following constraints must be valid:
     * - the network-file was specified (otherwise no simulation is existing)
     * - a junction folder must be given
     *   (otherwise no junctions can be loaded)
     * - the begin and the end of the simulation must be given
     * Returns true when all constraints are valid
     */
    static bool checkOptions();

    /** Sets the global microsim-options */
    static void setMSGlobals(OptionsCont &oc);

};


#endif

/****************************************************************************/

