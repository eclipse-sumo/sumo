#ifndef StringUtils_h
#define StringUtils_h
/***************************************************************************
                          StringUtils.h  -
                          Some helping function for string processing
                             -------------------
    begin                : unknown
    copyright            : (C) 2003 by Daniel Krajzewicz
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
// Revision 1.4  2003/06/18 11:37:48  dkrajzew
// documentation patched
//
// Revision 1.3  2003/05/20 09:49:43  dkrajzew
// further work and debugging
//
// Revision 1.2  2003/04/01 15:28:13  dkrajzew
// some further functions added
//
// Revision 1.1  2003/03/20 17:31:45  dkrajzew
// StringUtils moved from utils/importio to utils/common
//
// Revision 1.2  2003/03/20 16:41:10  dkrajzew
// periodical car emission implemented; windows eol removed
//
//

/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class StringUtils
 * Holds some static methods for string processing
 */
class StringUtils {
public:
    /// Removes trailing and leading whitechars
    static std::string prune(std::string str);

    /// Transfers the content to lower case
    static std::string to_lower_case(std::string str);

    /// Builds
    static std::string version1(std::string str);

    /// Converts german "Umlate" to theri HTML-version
    static std::string to_html(std::string str);

    /** Replaces all occurences of the second string by the third
        string within the first string */
    static std::string replace(std::string str, const char *what,
        const char *by);

    /** Returns the given double as a string with as many digits
        after the dot as defined by the second parameter */
    static std::string trim(double val, size_t to);

};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

//#ifndef DISABLE_INLINE
//#include "StringUtils.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
