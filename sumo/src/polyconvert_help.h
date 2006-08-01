#ifndef polyconvert_help_h
#define polyconvert_help_h
/***************************************************************************
                          polyconvert_help.h
			  the help text for polyconvert
                             -------------------
    project              : SUMO
    subproject           : PolyConvert
    begin                : Mon, 05 Dec 2005
    copyright            : (C) 2005 by DLR http://ivf.dlr.de/
    author               : Danilo Boyom
    email                : Danilot.Tete-Boyom@dlr.de
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
// Revision 1.1  2006/08/01 07:20:32  dkrajzew
// polyconvert added
//
// Revision 1.4  2005/12/12 11:58:14  dksumo
// help output patched; cheanges due to correct the output
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
    "  Polygon importer.",
    " ",
    "Usage: sumo-polyconvert [OPTION]*",
    " ",
    "Examples: ",
    " sumo-polyconvert -n net.net.xml --elmar tol_polygons.txt --use-projection",
    " ",
    "Options:",
    " Input files:",
    "   -c, --configuration FILE       FILE will be used as configuration",
    "   -n, --net-file FILE            FILE is a network file",
    "   --elmar FILE                   Import polygons from FILE",
    "   --typemap FILE                 Load types from FILE",
    " ",
    " Output files:",
    "  -o, --output-file FILE         The generated net will be written to FILE",
    "                                  default: 'polygons.xml'",
    " ",
    " Setting Defaults:",
    "  --color RED,GREEN,BLUE         Defines the default color for polygons",
    "  --prefix PREFIX                Prepends PREFIX to polygon names",
    "  --type NAME                    All polygons will have the type NAME",
    "  --layer INT                    Sets INT as the layer for polygons",
    " ",
    " Geocoordinates Projection and Network Geometry:",
    "  --use-projection               Enables reprojection of network coordinates",
    "  --proj PROJ_DEF                Defines the projection used by PROJ",
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
