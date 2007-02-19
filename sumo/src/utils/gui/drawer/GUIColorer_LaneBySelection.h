/****************************************************************************/
/// @file    GUIColorer_LaneBySelection.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
//
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
#ifndef GUIColorer_LaneBySelection_h
#define GUIColorer_LaneBySelection_h
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

#include "GUIBaseColorer.h"
#include <utils/gfx/RGBColor.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <utils/gui/div/GUIGlobalSelection.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
template<class _T>
class GUIColorer_LaneBySelection : public GUIBaseColorer<_T>
{
public:
    GUIColorer_LaneBySelection()
    { }

    virtual ~GUIColorer_LaneBySelection()
    { }

    void setGlColor(const _T& i) const
    {
        if (gSelected.isSelected(i.getType(), i.getGlID())) {
            glColor3f(0, .4f, .8f);
        } else {
            glColor3f(0.7f, 0.7f, 0.7f);
        }
    }

    virtual ColorSetType getSetType() const
    {
        return CST_SET;
    }

};


#endif

/****************************************************************************/

