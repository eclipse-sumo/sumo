/****************************************************************************/
/// @file    OptionsParser.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 17 Dec 2001
/// @version $Id: $
///
// Parses the command line arguments
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef OptionsParser_h
#define OptionsParser_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif


// ===========================================================================
// class declarations
// ===========================================================================
class OptionsCont;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class OptionsParser
 * Used for the parsing of the command line arguments.
 * Usage:
 * The only accessable method returns false when something failed. This may
 * happend when the syntax of the arguments is invalid, a value is tried to
 * be set several times or an unknown option is tried to be set.
 * The class assumes all options are unset or using default values only.
 */
class OptionsParser
{
public:
    /** parses the command line arguments */
    static bool parse(OptionsCont *oc, int argc, char **argv);

private:
    /** parses the last argument */
    static int check(OptionsCont *oc, char *arg1, bool &ok);

    /** parses the previous arguments */
    static int check(OptionsCont *oc, char *arg1, char *arg2, bool &ok);

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
    static bool processNonBooleanSingleSwitch(OptionsCont *oc, char *arg);

};


#endif

/****************************************************************************/

