#ifndef help_h
#define help_h
/***************************************************************************
                          help.h
              the help text
                             -------------------
    project              : SUMO
    begin                : Mon, 17 Dec 2001
    copyright            : (C) 2001 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.13  2005/10/07 11:48:00  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.12  2005/09/15 12:27:08  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.11  2004/12/20 15:00:37  dkrajzew
// version patched
//
// Revision 1.10  2004/12/20 14:05:15  dkrajzew
// version patched
//
// Revision 1.9  2004/12/20 13:15:58  dkrajzew
// options output corrected
//
// Revision 1.8  2003/07/21 11:04:06  dkrajzew
// the default duration of green light phases may now be changed on startup
//
// Revision 1.7  2003/07/07 08:40:09  dkrajzew
// included new options into the help-screens
//
// Revision 1.6  2003/06/18 11:26:15  dkrajzew
// new message and error processing: output to user may be a message,
//  warning or an error now; it is reported to a Singleton (MsgHandler);
//  this handler puts it further to output instances.
//  changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.5  2003/06/05 14:41:53  dkrajzew
// further parameter contraining the area of connectors to join (VIssim) added
//
// Revision 1.4  2003/04/04 08:41:47  dkrajzew
// help screen updated; min-decel usage added
//
// Revision 1.3  2003/02/07 10:37:30  dkrajzew
// files updated
//
// Revision 1.1  2002/10/16 14:51:08  dkrajzew
// Moved from ROOT/sumo to ROOT/src; added further help and main files for
//  netconvert, router, od2trips and gui version
//
// Revision 1.6  2002/07/25 08:24:09  dkrajzew
// Visum and Cell import added
//
// Revision 1.5  2002/06/11 15:56:07  dkrajzew
// windows eol removed
//
// Revision 1.4  2002/06/07 14:58:44  dkrajzew
// Bugs on dead ends and junctions with too few outgoing roads fixed; Comments improved
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
 * The help text
 */
char *help[] = {
    "Simulation of Urban MObility v0.8.2 - netconvert application",
    "  Imports networks.",
    "  http://sumo.sourceforge.net",
    "Usage: sumo-netconvert [OPTION]*",
    " ",
    "Examples: ",
    " sumo-netconvert -n ./nodes.xml -e ./edges.xml -v -t ./owntypes.xml",
    " sumo-netconvert -c owntyped.cfg",
    " ",
    "Options:",
    " Input files:",
    "  -c, --configuration FILE       FILE will be used as configuration",
    "                                   1. Default: ./sumo-netcovert.cfg",
    "  -n, --xml-node-files FILE[;+]  node definitions in XML-format",
    "  -e, --xml-edge-files FILE[;+]  edge definitions in XML-format",
    "  -x, --xml-connection-files FILE[;+]",
    "                                 connection definitions in XML-format",
    "  -t, --type-file FILE           FILE contains type definitions (XML)",
    "  --arcview FILE_NO_EXT          The name of arcview files",
    "  --arcview-shp FILE             The name of the arcview-shp file",
    "  --arcview-dbf FILE             The name of the arcview-sbf file",
    "  --artemis PATH                 Artemis path",
    "  --cell-node-file FILE          node definitions in Cell-format",
    "  --cell-edge-file FILE          edge definitions in Cell-format",
    "  --visum FILE                   Visum v7.5 file (.net)",
    "  --vissim FILE                  Vissim v3.6.2+ file (.inp)",
    "  --elmar PATH                   Elmar path and file prefix.",
    " ",
    " Output files:",
    "  -o, --output-file FILE         The generated net will be written to FILE",
    "  --map-output FILE              File to write joined edges information to",
    "  --plain-output FILE            Prefix of files to write node and edge",
    "                                   information to",
    " ",
    " Setting Defaults:",
    "  -T, --type NAME                The default name for an edges type",
    "  -L, --lanenumber INT           The default number of lanes in an edge",
    "  -S, --speed DOUBLE             The default speed on an edge (in m/s)",
    "  -P, --priority INT             The default priority of an edge",
    "   -j, --default-junction-type [traffic_light|priority|actuated|agentbased]",
    "                                    Determines the type of the build junctions",
    " ",
    " Processing Options:",
    "  -N, --capacity-norm FLOAT      The factor for flow to no. lanes conv.",
    "  -D, --min-decel FLOAT          The least vehicle deceleration value used",
    "  --vissim-speed-norm FLOAT      Factor for edge velocity (vissim only)",
    "  --vissim-default-speed FLOAT   Default speed for edges (vissim only)",
    "  --flip-y                       Flips the y-coordinate along zero",
    "  --speed-in-km                  vmax is parsed as given in km/h; XML only",
    "  --use-laneno-as-priority       Uses the number of lanes priority hint",
    "  --omit-corrupt-edges           Continues parsing although a corrupt edge",
    "                                    occured (warning is printed)",
    "  --vissim-offset DOUBLE         Specifies the structure join offset within",
    "                                   vissim-import",
    "  --keep-small-tyellow           Given yellow times are kept even if too ",
    "                                   they are small",
    "  --traffic-light-green INT      Overide the default (20s) of a green phase",
    "                                   duration ",
    "  --edges-min-speed FLOAT        Specifies the minimum speed of edges in order",
    "                                  to be important",
    "  --guess-tls                    Turn on tls guessing",
    "  --guess-tls.min-incoming-speed FLOAT",
    "  --guess-tls.max-incoming-speed FLOAT",
    "  --guess-tls.min-outgoing-speed FLOAT",
    "  --guess-tls.max-outgoing-speed FLOAT",
    "                                 Min/max speeds that incoming/outgoing edges",
    "                                  must allowed in order to make their junction",
    "                                  tls-controlled.",
    "  --tls-guess.no-incoming-min INT",
    "  --tls-guess.no-incoming-max INT",
    "  --tls-guess.no-outgoing-min INT",
    "  --tls-guess.no-outgoing-max INT",
    "                                 Min/max of incoming/outgoing edges a junction",
    "                                  may have in order to be tls-controlled.",
    "  --explicite-tls JID[;JID]*     Specifies which junctions named by their ids",
    "                                  shall be tls-controlled."
    "  --explicite-no-tls JID[;JID]*  Specifies which junctions named by their ids",
    "                                  shall not be tls-controlled."
    " ",
    " Report options:",
    "  -v, --verbose                   SUMO-NETCONVERT will report what it does",
    "  -W, --suppress-warnings         No warnings will be printed",
    "  -l, --log-file FILE             Writes all messages to the file",
    "  -p, --print-options             Prints option values before processing",
    "  -?, --help                      This screen",
    0
};

#endif
