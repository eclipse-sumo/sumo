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
// Revision 1.1  2005/09/23 06:05:45  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.1  2005/09/20 06:13:00  dksumo
// floats and doubles replaced by SUMOReal; warnings removed
//
// Revision 1.4  2005/09/09 12:54:02  dksumo
// complete code rework: debug_new and config added
//
// Revision 1.3  2005/04/26 08:11:57  dksumo
// level3 warnings patched; debugging
//
// Revision 1.2.2.1  2005/04/15 10:08:40  dksumo
// level3 warnings removed
//
// Revision 1.2  2004/11/22 12:52:46  dksumo
// code beautifying
//
// Revision 1.1  2004/10/22 12:50:37  dksumo
// initial checkin into an internal, standalone SUMO CVS
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
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

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

