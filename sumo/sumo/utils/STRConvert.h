#ifndef STRConvert_h
#define STRConvert_H

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>

/* =========================================================================
 * class definitions
 * ======================================================================= */
class STRConvert {
 public:
    /** converts the given 0-terminated xml-string into an int; 
        throws XMLUngivenParameterException when the xml-string is 0 */
     static int _2int(const std::string &str);
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "XMLConvert.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:


