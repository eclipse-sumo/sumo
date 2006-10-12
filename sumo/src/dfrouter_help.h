#ifndef help_h
#define help_h
/***************************************************************************
                          dfrouter_help.h
    Help-screen for DFROUTER
                             -------------------
    project              : SUMO
    begin                : Thu, 16.03.2006
    copyright            : (C) 2006 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// $Log$
// Revision 1.4  2006/10/12 10:14:25  dkrajzew
// synchronized with internal CVS (mainly the documentation has changed)
//
// Revision 1.3  2006/02/01 06:10:40  dkrajzew
// applied Eric's changes
//
// Revision 1.2  2005/12/21 12:48:38  ericnicolay
// *** empty log message ***
//
// Revision 1.1  2005/12/08 12:51:15  ericnicolay
// add new workingmap for the dfrouter
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
    "  Routes vehicles between detectors.",
    " ",
    "Usage: (sumo-)dfrouter(.exe) [OPTION]*",
    " ",
    "Examples: ",
    "  dfrouter -c routes_munich.cfg",
    " ",
    "Options:",
    " Input files:",
    "  Configuration",
    "   -c, --configuration FILE    FILE will be used as configuration",
    " ",
    "  Network description:",
    "   -n, --net-file FILE         FILE containing the SUMO-net",
    //"   -w, --weights FILE          FILE containing the weights of the net",
    //"   -S, --supplementary-weights FILE",
    //"                                 FILE containing the supplementary-weights",
    //"                                   of the net",
    " ",
    "  Route descriptions:",
    "   -r, --routes-input FILE       FILE containing SUMO-routes",
    //"   -t, --trip-defs FILE        FILE containing trip definitions",
    //"   -a, --alternatives FILE     FILE containing route alternatives",
    //"   -f, --flow-definition FILE  FILE containing flow definition",
    //"   --cell-input FILE           FILE containing cell routes",
    //"   --artemis-input FILE        FILE containing cell routes",
    " ",
    " Detector files:",
    "   -d, --detector-definitions FILE FILE containing detector definition",
    "   -f, --flow-definitions        FILE containing flow definition",
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
    //"   -R, --random-per-second FLOAT   Builds (additional) random routes",
    //"   --gBeta FLOAT               Gawron-Beta for DUE",
    //"   --gA FLOAT                  Gawron-A for DUE",
    //"   --unsorted                  The routes are assumed to be unsorted",
    //"   --save-cell-rindex          The index to cell-routes will be saved",
    //"   --intel-cell                Set, when reading cell-routes computed",
    //"                                 on an Intel-machine",
    //"   --no-last-cell              Use best, not the last cell-route",
    //"   --continue-on-unbuild       Continue if a route could not be build",
    //"   --move-on-short              Move vehicles on too short edges forward"
    //"   --srand INT                 Initialises the random number generator",
    //"                                 with the given value",
    //"   --abs-rand                  Set this when the current time shall be",
    //"                                 used for random number initialisation",
    " ",
    //" Deafults:",
    //"   -V, --krauss-vmax FLOAT    Default value for krauss' vmax",
    //"   -A, --krauss-a FLOAT       Default value for krauss' max. accel",
    //"   -B, --krauss-b FLOAT       Default value for krauss' max. decel",
    //"   -L, --krauss-length FLOAT  Default value for krauss' vehicle length",
    //"   -E, --krauss-eps FLOAT     Default value for krauss' driver imperf.",
    //" ",
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
