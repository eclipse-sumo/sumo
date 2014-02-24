/****************************************************************************/
/// @file    StringUtils.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    unknown
/// @version $Id$
///
// Some static methods for string processing
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
    static std::string prune(const std::string &str);

    /// Transfers the content to lower case
    static std::string to_lower_case(std::string str);

    /// Transfers from Latin 1 (ISO-8859-1) to UTF-8
    static std::string latin1_to_utf8(std::string str);

    /// Converts german "Umlaute" to their latin-version
    static std::string convertUmlaute(std::string str);

    /** Replaces all occurences of the second string by the third
        string within the first string */
    static std::string replace(std::string str, const char* what,
                               const char* by);

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
    static std::string escapeXML(const std::string& orig);

    /// An empty string
    static std::string emptyString;

};


#endif

/****************************************************************************/

