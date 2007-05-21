/****************************************************************************/
/// @file    StringUtils.h
/// @author  Daniel Krajzewicz
/// @date    unknown
/// @version $Id$
///
// Some static methods for string processing
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
#ifndef StringUtils_h
#define StringUtils_h
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

#include <string>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class StringUtils
 * @brief Some static methods for string processing
 */
class StringUtils
{
public:
    /// Removes trailing and leading whitechars
    static std::string prune(std::string str);

    /// Transfers the content to lower case
    static std::string to_lower_case(std::string str);

    /// Builds
    static std::string version1(std::string str);

    /// Converts german "Umlate" to their latin-version
    static std::string convertUmlaute(std::string str);

    /** Replaces all occurences of the second string by the third
        string within the first string */
    static std::string replace(std::string str, const char *what,
                               const char *by);

    /// Converts the given string to upper characters
    static void upper(std::string &str);

    /// Builds a time string (hh:mm:ss) from the given seconds
    static std::string toTimeString(int time);

    /// An empty string
    static std::string emptyString;

};


#endif

/****************************************************************************/

