#ifndef help_h
#define help_h
//---------------------------------------------------------------------------//
//                        router_help.h -
//  Help-screen of the router module
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
// Revision 1.11  2003/08/21 13:01:39  dkrajzew
// some bugs patched
//
// Revision 1.10  2003/07/07 08:40:09  dkrajzew
// included new options into the help-screens
//
// Revision 1.9  2003/06/18 11:26:15  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.8  2003/05/20 09:54:45  dkrajzew
// configuration files are no longer set as default
//
// Revision 1.7  2003/04/09 15:45:29  dkrajzew
// router debugging & extension: no routing over sources, random routes added
//
// Revision 1.6  2003/03/31 06:18:16  dkrajzew
// help screen corrected
//
// Revision 1.5  2003/02/07 10:37:30  dkrajzew
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
 * The list of help strings for the sumo-router module
 */
char *help[] = {
    "Usage: sumo-router [OPTION]*",
    "Simulation of Urban MObility v0.7 - Routing Module",
    "  http://sumo.sourceforge.net",
    "Examples: ",
    " sumo-router -c routes_munich.cfg",
    " ",
    "Options:",
    " Input files:",
    "  Configuration",
    "   -c, --configuration FILE       FILE will be used as configuration",
    "                                    1. Default: ./sumo-netcovert.cfg",
    "  Network description:",
    "   -n, --net-files FILE           FILE containing the SUMO-net",
    "   -w, --weights FILE             FILE containing the weights of the net",
    " ",
    "  Route descriptions:",
    "   -s, --sumo-input FILE          FILE containing SUMO-routes",
    "   -t, --trip-defs FILE           FILE containing trip definitions",
    "   -a, --alternatives FILE        FILE containing route alternatives",
    "   --cell-input FILE              FILE containing cell routes",
    "   --artemis-input FILE           FILE containing cell routes",
    " ",
    " Output files:",
    "   -o, --output-file FILE         Generated routes will be written to FILE",
    "                                   Additionally, a file with route",
    "                                   alternatives will be generated as",
    "                                   'FILE.alt'",
    " ",
    " Process Options:",
    "   -b, --begin LONG               The begin time of routing",
    "   -e, --end LONG                 The end time of routing",
    "   -R, --random-per-second FLOAT  Builds (additional) random routes",
    "   --gBeta FLOAT                  Gawron-Beta for DUE",
    "   --gA FLOAT                     Gawron-A for DUE",
    "   --unsorted                     The routes are assumed to be unsorted",
    "   --save-cell-rindex             The index to cell-routes will be saved",
    "   --intel-cell                   Set, when reading cell-routes computed",
    "                                   on an Intel-machine",
    "   --no-last-cell                 Use best, not the last cell-route",
    "   --continue-on-unbuild          Continue if a route could not be build",
    " ",
    " Report options:",
    "   -v, --verbose                   SUMO-ROUTER will report what it does",
    "   -W, --suppress-warnings         No warnings will be printed",
    "   -l, --log-file FILE             Writes all messages to the file",
    "   -p, --print-options             Prints option values before processing",
    "   -?, --help                      This screen",
    0
};

#endif
