#ifndef help_h
#define help_h
/***************************************************************************
                          help.h
			  the help text
                             -------------------
    project              : OD2TRIPS
    begin                : 11.09.2002
    copyright            : (C) 2002 by DLR/IVF http://ivf.dlr.de/
    author               : Peter Mieth
    email                : Peter.Mieth@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/* =========================================================================
 * definitions
 * ======================================================================= */
/**
 * The list of help strings for the sumo-od2trips module
 */
char *help[] = {
    "Usage: od2trips [OPTION]*",
    "Merges OD matrix and net to trip tables v0.1 - Od2Trip Module",
    "  http://???",
	" ",
    "Examples: ",
    " od2trips -n ./MD_Netz.net..xml -d ./Format_O.fm -o ./triptable.txt -vC",
    " od2trips -c od2trips.cfg",
    " ",
    "Options:",
    " Input files:",
    "   -c, --configuration FILE        FILE will be used as configuration",
    "                                    1. Default: ./sumo-od2trips.cfg",
    "   -n, --net-file FILE             net definitions in XML-format",
    "   -d, --od-file FILE              OD on district basis",
    " ",
    " Output files:",
    "   -o, --output-file FILE         The generated trip table will be written"
	"                                        to FILE",
    " ",
    " Processing Options:",
    "   -b, --begin                    Time to begin with (in s)",
    "   -e, --end                      Time to end with (in s)",
    "   -s, --scale                    Norming factor",
    "                                   The number of trips is =",
    "                                   number of read/factor",
    "   -C, --no-config                No configuration file is used",
    "   -v, --verbose                  od2trips reports what it does",
    "   -p, --print-options            Prints option values before processing",
    "   -h, --help                     This screen",
    0
};

#endif
