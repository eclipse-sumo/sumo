#ifndef TOSTRING_H
#define TOSTRING_H

//---------------------------------------------------------------------------//
//                        ToString.h  -  Converter to std::string
//                           -------------------
//  begin                : Wed, 23 Sep 2002
//  copyright            : (C) 2002 by Christian Roessel
//  organisation         : DLR/IVF http://ivf.dlr.de
//  email                : roessel@zpr.uni-koeln.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//

// $Log$
// Revision 1.2  2004/11/23 10:28:36  dkrajzew
// debugging
//
// Revision 1.1  2003/02/07 10:48:00  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 14:44:45  dkrajzew
// ROOT/sumo moved to ROOT/src
//
// Revision 1.1  2002/09/25 16:57:31  roessel
// Initial commit.
//

/* =========================================================================
 * included modules
 * ======================================================================= */
#include <sstream>
#include <string>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * Template for conversions from origin format to string representation
 * (when supplied by c++/the stl)
 */
template <class T>
inline std::string toString( const T& t ) {
    std::ostringstream oss;
    oss << t;
    return oss.str();
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

