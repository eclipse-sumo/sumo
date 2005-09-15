#ifndef GfxConvHelper_h
#define GfxConvHelper_h
//---------------------------------------------------------------------------//
//                        GfxConvHelper.h -
//  Some helper functions to help when parsing graphical information
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
// Revision 1.5  2005/09/15 12:18:33  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2004/11/23 10:35:01  dkrajzew
// debugging
//
// Revision 1.3  2003/12/11 06:20:15  dkrajzew
// cvs-log meta-info added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <string>
#include <utils/gfx/RGBColor.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class GfxConvHelper
 * This class holds some helping functions for the graphical information
 *  parsing
 */
class GfxConvHelper {
public:
    /** @brief parses a color information
        It is assumed that the color is stored as "<RED>,<GREEN>,<BLUE>"
        And each color is represented as a float. */
    static RGBColor parseColor(const std::string &coldef);

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
