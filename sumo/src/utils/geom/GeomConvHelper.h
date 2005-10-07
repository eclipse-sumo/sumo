#ifndef GeomConvHelper_h
#define GeomConvHelper_h
//---------------------------------------------------------------------------//
//                        GeomConvHelper.h -
//  Some helping functions for geometry parsing
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2003
//  copyright            : (C) 2003 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
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
// Revision 1.6  2005/10/07 11:44:16  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.5  2005/09/23 06:07:01  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.4  2005/09/15 12:18:19  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2004/11/23 10:34:46  dkrajzew
// debugging
//
// Revision 1.2  2003/12/09 11:32:50  dkrajzew
// documentation added
//
//
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
#include <utils/geom/Position2DVector.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class GeomConvHelper
 * This class holds some helping functions for the parsing of geometries
 */
class GeomConvHelper {
public:
    /** @brief This method builds a Position2DVector from a string representation
        It is assumed, the vector is stored as "x,y{ x,y}*" where x and y are
        SUMOReals. */
    static Position2DVector parseShape(const std::string &shpdef);

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

