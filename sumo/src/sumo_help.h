#ifndef help_h
#define help_h
//---------------------------------------------------------------------------//
//                        sumo_help.h -
//  Help-screen of the simulation module
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
// Revision 1.8  2003/12/12 12:32:13  dkrajzew
// continuing on accidents is now meant to be the default behaviour
//
// Revision 1.7  2003/10/28 08:35:01  dkrajzew
// random number specification options added
//
// Revision 1.6  2003/07/07 08:40:09  dkrajzew
// included new options into the help-screens
//
// Revision 1.5  2003/06/18 11:26:15  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.4  2003/03/31 06:18:18  dkrajzew
// help screen corrected
//
// Revision 1.3  2003/02/07 10:37:30  dkrajzew
// files updated
//
//

/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H


/* =========================================================================
 * definitions
 * ======================================================================= */
/**
 * The list of help strings for the sumo (simulation) module
 */
char *help[] = {
    "Usage: sumo [OPTION]+",
    "Simulation of Urban MObility v0.8 - Simulation Module",
    "  http://sumo.sourceforge.net",
    "Examples: ",
    "sumo -b 0 -e 1000 -n net.xml -r routes.xml -C",
    "sumo -c munich_config.cfg",
    " ",
    "Options:",
    " Input files:",
    "   -n, --net-files FILE[;FILE]*    FILE is a network file",
    "   -r, --route-files FILE[;FILE]*  FILE is a route file",
    "   -a, --additional-files FILE...  FILE is a detector file",
    "   -c, --configuration FILE        FILE will be used as configuration",
    "                                    1. Default: ./sumo.cfg",
    " ",
    " Output files:",
    "   -o, --output-file FILE          FILE is the raw output destination file",
    "                                    Default: stdout",
    "   --dump-basename PATH            PATH is the name and path prefix that",
    "                                    specifies where to save the net loads",
    "   --dump-intervals UINT[;UINT]*   UINT is a positive integer time interval",
    "                                    for which a net load will generated",
    " ",
    " Simulation timing:",
    "   -b, --begin INT                 First time step of the simulation",
    "   -e, --end INT                   Last time step of the simulation",
    "   -s, --route-steps INT           The number of steps to read routes in",
    "                                   forward (default=0: read all routes)",
    " Simulation options:",
    "   --quit-on-accident              Quits when an accident occures",
    "   --srand <INT>                   Initialises the random number generator",
    "                                    with the given value",
    "   --abs-rand                      Set this when the current time shall be",
    "                                    used for random number initialisation",
    " ",
    " Report Options:",
    "   -v, --verbose                   SUMO will report what it does",
    "   -W, --suppress-warnings         No warnings will be printed",
    "   -l, --log-file FILE             Writes all messages to the file",
    "   -p, --print-options             Prints option values before processing",
    "   -?, --help                      This screen",
    0
};

#endif
