#ifndef OptionsParser_h
#define OptionsParser_h
/***************************************************************************
                          OptionsParser.h
              Parses the command line arguments
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
// Revision 1.4  2005/05/04 09:28:01  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.3  2004/11/23 10:36:02  dkrajzew
// debugging
//
// Revision 1.2  2003/02/07 10:51:59  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 14:58:18  dkrajzew
// initial release for utilities that handle program options
//
// Revision 1.4  2002/07/31 17:30:07  roessel
// Changes since sourceforge cvs request.
//
// Revision 1.5  2002/07/18 07:07:06  dkrajzew
// The options should now be POSIX-compliant
//
// Revision 1.4  2002/07/11 07:42:59  dkrajzew
// Usage of relative pathnames within configuration files implemented
//
// Revision 1.4  2002/06/11 15:58:25  dkrajzew
// windows eol removed
//
// Revision 1.3  2002/05/14 04:45:50  dkrajzew
// Bresenham added; some minor changes; windows eol removed
//
// Revision 1.2  2002/04/26 10:08:39  dkrajzew
// Windows eol removed
//
// Revision 1.1.1.1  2002/04/09 14:18:27  dkrajzew
// new version-free project name (try2)
//
// Revision 1.1.1.1  2002/04/09 13:22:01  dkrajzew
// new version-free project name
//
// Revision 1.1.1.1  2002/02/19 15:33:04  traffic
// Initial import as a separate application.
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * class declarations
 * ======================================================================= */
class OptionsCont;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class OptionsParser
 * Used for the parsing of the command line arguments.
 * Usage:
 * The only accessable method returns false when something failed. This may
 * happend when the syntax of the arguments is invalid, a value is tried to
 * be set several times or an unknown option is tried to be set.
 * The class assumes all options are unset or using default values only.
 */
class OptionsParser {
public:
    /** parses the command line arguments */
    static bool parse(OptionsCont *oc, int argc, char **argv);

private:
    /** parses the last argument */
    static int check(OptionsCont *oc, char *arg1);

    /** parses the previous arguments */
    static int check(OptionsCont *oc, char *arg1, char *arg2);

    /** returns the information whether the given argument is an option
        (begins with '-') */
    static bool checkParameter(char *arg1);

    /** returns the information whether the given argument consists of
        abbreviations (single leading '-') */
    static bool isAbbreviation(char *arg1);

    /** converts char* to string */
    static std::string convert(char *arg);

    /** converts char to string */
    static std::string convert(char abbr);

    /** extracts the parameter directly attached to an option */
    static int processNonBooleanSingleSwitch(OptionsCont *oc, char *arg);

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
