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
// Revision 1.3  2002/06/11 14:38:22  dkrajzew
// windows eol removed
//
// Revision 1.2  2002/06/11 13:43:37  dkrajzew
// Windows eol removed
//
// Revision 1.1.1.1  2002/04/08 07:21:25  traffic
// new project name
//
// Revision 2.0  2002/02/14 14:43:28  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.1  2002/02/13 15:48:20  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
//
/* =========================================================================
 * class declarations
 * ======================================================================= */
class OptionsCont;

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * OptionsParser
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
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "OptionsParser.h.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:


