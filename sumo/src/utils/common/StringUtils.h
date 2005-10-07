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
// Revision 1.11  2005/10/07 11:43:30  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.10  2005/09/23 06:05:45  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.9  2005/09/15 12:13:08  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.8  2005/04/28 09:02:46  dkrajzew
// level3 warnings removed
//
// Revision 1.7  2004/11/23 10:27:45  dkrajzew
// debugging
//
// Revision 1.6  2004/01/12 14:38:19  dkrajzew
// to-upper method added
//
// Revision 1.5  2003/09/05 15:26:39  dkrajzew
// umlaute conversion added
//
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
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


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

    /// Converts german "Umlate" to their latin-version
    static std::string convertUmlaute(std::string str);

    /** Replaces all occurences of the second string by the third
        string within the first string */
    static std::string replace(std::string str, const char *what,
        const char *by);

    /** Returns the given SUMOReal as a string with as many digits
        after the dot as defined by the second parameter */
    static std::string trim(SUMOReal val, size_t to);

    /// Converts the given string to upper characters
    static void upper(std::string &str);

};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
