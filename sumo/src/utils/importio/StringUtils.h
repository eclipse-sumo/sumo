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
};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

//#ifndef DISABLE_INLINE
//#include "StringUtils.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
