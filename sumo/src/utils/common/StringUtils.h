/****************************************************************************/
/// @file    StringUtils.h
/// @author  Daniel Krajzewicz
/// @date    unknown
/// @version $Id$
///
// Some static methods for string processing
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
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
class StringUtils {
public:
    /// Removes trailing and leading whitechars
    static std::string prune(std::string str);

    /// Transfers the content to lower case
    static std::string to_lower_case(std::string str);

    /// Converts german "Umlaute" to their latin-version
    static std::string convertUmlaute(std::string str);

    /** Replaces all occurences of the second string by the third
        string within the first string */
    static std::string replace(std::string str, const char *what,
                               const char *by);

    /// Builds a time string (hh:mm:ss) from the given seconds
    static std::string toTimeString(int time);

    /**
     * @brief Replaces the standard escapes by their XML entities.
     *
     * The strings &, <, >, ", and ' are replaced by &amp;, &lt;, &gt;, &quot;, and &apos;
     *
     * @param[in] orig The original string
     * @return the string with the escaped sequences
     */
    static std::string escapeXML(const std::string &orig) throw();

    /// An empty string
    static std::string emptyString;

};


#endif

/****************************************************************************/

