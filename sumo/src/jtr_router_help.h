#ifndef help_h
#define help_h
//---------------------------------------------------------------------------//
//                        jp_router_help.h -
//		Help-screen of the junction percentage router module
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 20 Jan 2004
//  subproject           : junction percentage router
//  copyright            : (C) 2004 by Daniel Krajzewicz
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
// Revision 1.1  2004/02/06 08:56:44  dkrajzew
// _INC_MALLOC definition removed (does not work on MSVC7.0)
//
// Revision 1.1  2004/01/26 07:12:12  dkrajzew
// now two routers are available - the dua- and the jp-router
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
 * The list of help strings for the sumo-junction percentage router module
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
    "   --srand <INT>                  Initialises the random number generator",
    "                                   with the given value",
    "   --abs-rand                     Set this when the current time shall be",
    "                                   used for random number initialisation",
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
