#ifndef GUIColorer_LaneBySelection_h
#define GUIColorer_LaneBySelection_h
//---------------------------------------------------------------------------//
//                        GUIColorer_LaneBySelection.h -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Fri, 29.04.2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
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
// Revision 1.3  2005/09/15 12:19:10  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.2  2005/07/12 12:49:08  dkrajzew
// code style adapted
//
// Revision 1.2  2005/06/14 11:29:50  dksumo
// documentation added
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

#include "GUIBaseColorer.h"
#include <utils/gfx/RGBColor.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <utils/gui/div/GUIGlobalSelection.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
template<class _T>
class GUIColorer_LaneBySelection : public GUIBaseColorer<_T> {
public:
	GUIColorer_LaneBySelection() { }

	virtual ~GUIColorer_LaneBySelection() { }

	void setGlColor(const _T& i) const {
        if(gSelected.isSelected(i.getType(), i.getGlID())) {
            glColor3f(0, .4f, .8f);
        } else {
            glColor3f(0, 0, 0);
        }
	}

};
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
