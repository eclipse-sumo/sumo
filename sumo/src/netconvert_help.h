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
// Revision 1.6  2003/06/18 11:26:15  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
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
// Moved from ROOT/sumo to ROOT/src; added further help and main files for netconvert, router, od2trips and gui version
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
//
/**
 * The help text
 */
char *help[] = {
    "Usage: sumo-netconvert [OPTION]*",
    "Simulation of Urban MObility v0.7 - Netconverter Module",
    "  http://sumo.sourceforge.net",
    "Examples: ",
    " sumo-netconvert -n ./nodes.xml -e ./edges.xml -v -t ./owntypes.xml",
    " sumo-netconvert -c owntyped.cfg",
    " ",
    "Options:",
    " Input files:",
    "   -c, --configuration FILE       FILE will be used as configuration",
    "                                    1. Default: ./sumo-netcovert.cfg",
    "   -n, --xml-node-files FILE[;+]  node definitions in XML-format",
    "   -e, --xml-edge-files FILE[;+]  edge definitions in XML-format",
    "   -x, --xml-connection-files FILE[;+]",
    "                                  connection definitions in XML-format",
    "   -t, --type-file FILE           FILE contains type definitions (XML)",
    "   --arcview FILE_NO_EXT          The name of arcview files",
    "   --arcview-shp FILE             The name of the arcview-shp file",
    "   --arcview-dbf FILE             The name of the arcview-sbf file",
    "   --artemis PATH                 Artemis path",
    "   --cell-node-file FILE          node definitions in Cell-format",
    "   --cell-edge-file FILE          edge definitions in Cell-format",
    "   --visum FILE                   Visum v7.5 file (.net)",
    "   --vissim FILE                  Vissim v3.6.2+ file (.inp)",
    " ",
    " Output files:",
    "   -o, --output-file FILE         The generated net will be written to FILE",
    " ",
    " Setting Defaults:",
    "   -T, --type NAME                The default name for an edges type",
    "   -L, --lanenumber INT           The default number of lanes in an edge",
    "   -S, --speed DOUBLE             The default speed on an edge (in m/s)",
    "   -P, --priority INT             The default priority of an edge",
    "   -N, --capacity-norm DOUBLE     The factor for flow to no. lanes conv.",
    "   -D, --min-decel DOUBLE         The least vehicle deceleration value used",
    " ",
    " Processing Options:",
    "   -C, --no-config                No configuration file is used",
    "   -v, --verbose                  SUMO-Netconvert reports what it does",
    "   -W, --suppress-warnings        No warnings will be printed",
    "   -p, --print-options            Prints option values before processing",
    "   --flip-y                       Flips the y-coordinate along zero",
    "   --speed-in-km                  vmax is parsed as given in km/h; XML only",
    "   --use-laneno-as-priority       Uses the number of lanes priority hint",
    "   --omit-corrupt-edges           Continues parsing although a corrupt edge",
    "                                     occured (warning is printed)",
	"   --vissim-offset DOUBLE         Specifies the structure join offset within",
	"                                     vissim-import",
    "   --help                         This screen",
    0
};

#endif
