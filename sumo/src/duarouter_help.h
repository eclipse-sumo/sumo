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
// Revision 1.8  2005/10/07 11:48:00  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.7  2005/09/15 12:27:08  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.6  2004/12/20 15:00:36  dkrajzew
// version patched
//
// Revision 1.5  2004/12/20 14:05:15  dkrajzew
// version patched
//
// Revision 1.4  2004/12/20 13:15:58  dkrajzew
// options output corrected
//
// Revision 1.3  2004/12/20 10:48:35  dkrajzew
// net-files changed to net-file
//
// Revision 1.2  2004/11/23 10:43:29  dkrajzew
// debugging
//
// Revision 1.1  2004/08/02 13:03:19  dkrajzew
// applied better names
//
// Revision 1.2  2004/04/14 13:53:49  roessel
// Changes and additions in order to implement supplementary-weights.
//
// Revision 1.1  2004/01/26 07:12:12  dkrajzew
// now two routers are available - the dua- and the jp-router
//
// Revision 1.12  2003/10/28 08:35:01  dkrajzew
// random number specification options added
//
// Revision 1.11  2003/08/21 13:01:39  dkrajzew
// some bugs patched
//
// Revision 1.10  2003/07/07 08:40:09  dkrajzew
// included new options into the help-screens
//
// Revision 1.9  2003/06/18 11:26:15  dkrajzew
// new message and error processing: output to user may be a message,
//  warning or an error now; it is reported to a Singleton (MsgHandler);
//  this handler puts it further to output instances.
//  changes: no verbose-parameter needed; messages are exported to singleton
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
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H


/* =========================================================================
 * definitions
 * ======================================================================= */
/**
 * The list of help strings for the sumo-router module
 */
char *help[] = {
    "Simulation of Urban MObility v0.8.2 - duarouter application",
    "  Routes vehicles by shortest path using dua.",
    "  http://sumo.sourceforge.net",
    "Usage: sumo-duarouter [OPTION]*",
    " ",
    "Examples: ",
    "  sumo-duarouter -c routes_munich.cfg",
    " ",
    "Options:",
    " Input files:",
    "  Configuration",
    "   -c, --configuration FILE    FILE will be used as configuration",
    " ",
    "  Network description:",
    "   -n, --net-file FILE         FILE containing the SUMO-net",
    "   -w, --weights FILE          FILE containing the weights of the net",
    "   -S, --supplementary-weights FILE",
    "                                 FILE containing the supplementary-weights",
    "                                   of the net",
    " ",
    "  Route descriptions:",
    "   -s, --sumo-input FILE       FILE containing SUMO-routes",
    "   -t, --trip-defs FILE        FILE containing trip definitions",
    "   -a, --alternatives FILE     FILE containing route alternatives",
    "   -f, --flow-definition FILE  FILE containing flow definition",
    "   --cell-input FILE           FILE containing cell routes",
    "   --artemis-input FILE        FILE containing cell routes",
    " ",
    " Output files:",
    "   -o, --output-file FILE      Generated routes will be written to FILE",
    "                                 Additionally, a file with route",
    "                                 alternatives will be generated as",
    "                                 'FILE.alt'",
    " ",
    " Process Options:",
    "   -b, --begin INT             The begin time of routing",
    "   -e, --end INT               The end time of routing",
    "   -R, --random-per-second FLOAT   Builds (additional) random routes",
    "   --gBeta FLOAT               Gawron-Beta for DUE",
    "   --gA FLOAT                  Gawron-A for DUE",
    "   --unsorted                  The routes are assumed to be unsorted",
    "   --save-cell-rindex          The index to cell-routes will be saved",
    "   --intel-cell                Set, when reading cell-routes computed",
    "                                 on an Intel-machine",
    "   --no-last-cell              Use best, not the last cell-route",
    "   --continue-on-unbuild       Continue if a route could not be build",
    "   --move-on-short              Move vehicles on too short edges forward"
    "   --srand INT                 Initialises the random number generator",
    "                                 with the given value",
    "   --abs-rand                  Set this when the current time shall be",
    "                                 used for random number initialisation",
    " ",
    " Deafults:",
    "   -V, --krauss-vmax FLOAT    Default value for krauss' vmax",
    "   -A, --krauss-a FLOAT       Default value for krauss' max. accel",
    "   -B, --krauss-b FLOAT       Default value for krauss' max. decel",
    "   -L, --krauss-length FLOAT  Default value for krauss' vehicle length",
    "   -E, --krauss-eps FLOAT     Default value for krauss' driver imperf.",
    " ",
    " Report options:",
    "   -v, --verbose              Verbose reports",
    "   -W, -suppress-warnings     No warnings will be printed",
    "   -l, -log-file FILE         Writes all messages to the file",
    "   -p, -print-options         Prints option values before processing",
    "   -?, --help                 This screen",
    "   --stats-period INT         Step number to pass between status output",
    0
};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
//
