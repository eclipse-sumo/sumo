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
// Revision 1.4  2002/11/04 08:42:31  dkrajzew
// in-line documentation updated; unwished includion of non-existing .icc-files removed; help screen updated
//
//

/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

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
    "   --gBeta FLOAT                  Gawron-Beta for DUE",
    "   --gA FLOAT                     Gawron-A for DUE",
    "   --unsorted                     The routes are assumed to be unsorted",
    "   --save-cell-rindex             The index to cell-routes will be saved",
    "   --intel-cell                   Set, when reading cell-routes computed",
    "                                   on an Intel-machine",
    "   --no-last-cell                 Use best, not the last cell-route",
    " ",
    " Processing Options:",
    "   -C, --no-config                No configuration file is used",
    "   -v, --verbose                  SUMO-Netconvert reports what it does",
    "   -p, --print-options            Prints option values before processing",
    "   --help                         This screen",
    0 
};

#endif
