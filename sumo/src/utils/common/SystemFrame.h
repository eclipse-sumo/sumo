#ifndef SystemFrame_h
#define SystemFrame_h
//---------------------------------------------------------------------------//
//                        SystemFrame.h -
//  A set of actions common to all applications
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Mon, 23.06.2003
//  copyright            : (C) 2003 by Daniel Krajzewicz
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
// Revision 1.6  2005/07/12 12:43:49  dkrajzew
// code style adapted
//
// Revision 1.5  2005/04/28 09:02:47  dkrajzew
// level3 warnings removed
//
// Revision 1.4  2004/11/23 10:27:45  dkrajzew
// debugging
//
// Revision 1.3  2003/06/24 08:10:23  dkrajzew
// extended by the options sub system; dcumentation added
//
// Revision 1.2  2003/06/24 08:09:29  dkrajzew
// implemented SystemFrame and applied the changes to all applications
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#include <utils/options/OptionsSubSys.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class OptionsCont;
class LogFile;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class SystemFrame
 * As almost all applications within the SUMO-packgae share the same
 * initialisation procedure, it is encapsulated within this class.
 * To leave the command line options variable for each application, they are
 * inserted into an application-global OptionsCont via a method to supply
 * at the call of the "init" method.
 */
class SystemFrame {
public:
    /** @brief Initialises the application's subsystems
        Initialises the xml-subsystem, the options subsystem and the messaging.
        Returns 0 (zero) if everything's ok, -1 if the application shall be quit
        normally and 1 if an error occured */
    static int init(bool gui, int argc, char **argv,
        fill_options *fill_f, check_options *check_f,
        char *help[]);

    /// Closes all of an applications subsystems
    static void close();

private:
    /** Static storage for the logging file used
        A separate storage is needed to delete the log file at application closing */
    static LogFile *myLogFile;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

