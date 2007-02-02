/****************************************************************************/
/// @file    GUIColorSetter.h
/// @author  Daniel Krajzewicz
/// @date    2006-01-09
/// @version $Id$
///
// missing_desc
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
#ifndef GUIColorSetter_h
#define GUIColorSetter_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif



// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif


#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include <utils/gfx/RGBColor.h>


inline
void
mglColor(const RGBColor &c)
{
    glColor3d(c.red(), c.green(), c.blue());
}


#endif

/****************************************************************************/

