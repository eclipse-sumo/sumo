/****************************************************************************/
/// @file    GfxConvHelper.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2003
/// @version $Id$
///
// Some helper functions to help when parsing graphical information
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GfxConvHelper_h
#define GfxConvHelper_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/gfx/RGBColor.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GfxConvHelper
 * This class holds some helping functions for the graphical information
 *  parsing
 */
class GfxConvHelper
{
public:
    /** @brief parses a color information
        It is assumed that the color is stored as "<RED>,<GREEN>,<BLUE>"
        And each color is represented as a SUMOReal. */
    static RGBColor parseColor(const std::string &coldef);

};


#endif

/****************************************************************************/

