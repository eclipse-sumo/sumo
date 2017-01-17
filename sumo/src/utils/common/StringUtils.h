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
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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
    static std::string prune(const std::string& str);

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

    /// Checks whether a given string starts with the prefix
    static bool startsWith(const std::string& str, const std::string prefix);

    /// Checks whether a given string ends with the suffix
    static bool endsWith(const std::string& str, const std::string suffix);

    /**
     * @brief Replaces the standard escapes by their XML entities.
     *
     * The strings &, <, >, ", and ' are replaced by &amp;, &lt;, &gt;, &quot;, and &apos;
     *
     * @param[in] orig The original string
     * @param[in] maskDoubleHyphen Whether -- in input shall be converted to &#45;&#45; (semantically equivalent but allowed in XML comments)
     * @return the string with the escaped sequences
     */
    static std::string escapeXML(const std::string& orig, const bool maskDoubleHyphen = false);

    /// An empty string
    static std::string emptyString;

    // the following methods stem from http://bogomip.net/blog/cpp-url-encoding-and-decoding/

    static std::string urlEncode(const std::string& url, const std::string encodeWhich = "");
    static std::string urlDecode(const std::string& encoded);

    static std::string charToHex(unsigned char c);
    static unsigned char hexToChar(const std::string& str);

};


#endif

/****************************************************************************/

