#ifndef help_h
#define help_h
//---------------------------------------------------------------------------//
//                        netgen_help.h -
//  Help-screen of the gui version
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Wed, 16 Jul 2003
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
// Revision 1.3  2003/12/04 12:51:26  dkrajzew
// documentation added; possibility to use actuated and agentbased junctions added; usage of street types patched
//
// Revision 1.2  2003/10/28 08:35:01  dkrajzew
// random number specification options added
//
// Revision 1.1  2003/07/16 15:41:17  dkrajzew
// network generator added
//
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
char *help[] =
{
    "Usage: netgen --grid-net [grid-network options] -o <OUTPUTFILE>",
    "       netgen --spider-net [spider-network opts] -o <OUTPUTFILE>",
    "       netgen --random-net [random-network opts] -o <OUTPUTFILE>",
    "       netgen -c <CONFIGURATION>",
    " ",
    "Simulation of Urban MObility v0.8 - Simulation Module",
    "  http://sumo.sourceforge.net",
    " ",
    " Either \"--grid-net\", \"--spider-net\" or \"--random-net\" must be",
    "  supplied. In dependance to these switches other options are used",
    " ",
    " Grid network options:",
    "   --grid-number INT              The number of junctions in both dirs",
    "   --grid-length FLOAT            The length of streets in both dirs",
    "   --grid-x-number INT            The number of junctions in x-dir",
    "                                   Overrides --grid-number",
    "   --grid-y-number INT            The number of junctions in y-dir",
    "                                   Overrides --grid-number",
    "   --grid-x-length FLOAT          The length of vertical streets",
    "                                   Overrides --grid-length",
    "   --grid-y-length FLOAT          The length of vertical streets",
    "                                   Overrides --grid-length",
    " ",
    " Spider network options:",
    "   --spider-arm-number INT        The number of axes within the net",
    "   --spider-circle-number         The number of circles of the net",
    "   --spider-space-rad             The distances between the circles",
    " ",
    " Random network options:",
    "   --rand-max-distance FLOAT ",
    "   --rand-min-distance FLOAT",
    "   --rand-min-angle FLOAT",
    "   --rand-num-tries FLOAT",
    "   --rand-connectivity FLOAT",
    "   --rand-neighbor-dist1 FLOAT",
    "   --rand-neighbor-dist2 FLOAT",
    "   --rand-neighbor-dist3 FLOAT",
    "   --rand-neighbor-dist4 FLOAT",
    "   --rand-neighbor-dist5 FLOAT",
    "   --rand-neighbor-dist6 FLOAT",
    "   --rand-iterations INT",
    " ",
    " Output files:",
    "   -o, --output-file FILE         The generated net will be written to FILE",
    " ",
    " Setting Defaults:",
    "   -T, --type NAME                The default name for an edges type",
    "   -L, --lanenumber INT           The default number of lanes in an edge",
    "   -S, --speed DOUBLE             The default speed on an edge (in m/s)",
    "   -P, --priority INT             The default priority of an edge",
    " ",
    "   --srand <INT>                   Initialises the random number generator",
    "                                    with the given value",
    "   --abs-rand                      Set this when the current time shall be",
    "                                    used for random number initialisation",
    " Report options:",
    "   -v, --verbose                   SUMO-NETCONVERT will report what it does",
    "   -W, --suppress-warnings         No warnings will be printed",
    "   -l, --log-file FILE             Writes all messages to the file",
    "   -p, --print-options             Prints option values before processing",
    "   -?, --help                      This screen",
    0
};

#endif

