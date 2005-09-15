#ifndef SUMOFrame_h
#define SUMOFrame_h
//---------------------------------------------------------------------------//
//                        SUMOFrame.h -
//  Some helping methods for usage within sumo and sumo-gui
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.9  2005/09/15 12:06:04  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.8  2004/11/25 16:26:50  dkrajzew
// consolidated and debugged some detectors and their usage
//
// Revision 1.7  2004/11/23 10:27:27  dkrajzew
// debugging
//
// Revision 1.6  2004/08/02 12:48:13  dkrajzew
// using OutputDevices instead of ostreams; first steps towards a lane-change API
//
// Revision 1.5  2004/04/02 11:27:36  dkrajzew
// simulation-wide output files are now handled by MSNet directly
//
// Revision 1.4  2004/02/16 13:44:27  dkrajzew
// dump output generating function renamed in order to add vehicle dump
//  ability in the future
//
// Revision 1.3  2003/06/24 08:09:29  dkrajzew
// implemented SystemFrame and applied the changes to all applications
//
// Revision 1.2  2003/02/07 11:19:37  dkrajzew
// updated
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H


/* =========================================================================
 * class declarations
 * ======================================================================= */
class OptionsCont;
class MSNet;
class OutputDevice;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class SUMOFrame
 * This class holds some helping methods needed both by the sumo and the
 * sumo-gui module.
 */
class SUMOFrame {
public:
    /// Builds teh simulation options
    static void fillOptions(OptionsCont &oc);

    /** @brief Builds the streams used possibly by the simulation
        */
    static std::vector<OutputDevice*> buildStreams(const OptionsCont &oc);


    /// Build the output stream named by the according option
    static OutputDevice *buildStream(const OptionsCont &oc,
        const std::string &optionName);

    /**
     * Checks the build settings. The following constraints must be valid:
     * - the network-file was specified (otherwise no simulation is existing)
     * - a junction folder must be given
     *   (otherwise no junctions can be loaded)
     * - the begin and the end of the simulation must be given
     * Returns true when all constraints are valid
     */
    static bool checkOptions(OptionsCont &oc);

    /** Sets the global microsim-options */
    static void setMSGlobals(OptionsCont &oc);

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

