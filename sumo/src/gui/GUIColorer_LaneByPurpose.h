#ifndef GUIColorer_LaneByPurpose_h
#define GUIColorer_LaneByPurpose_h
//---------------------------------------------------------------------------//
//                        GUIColorer_LaneByPurpose.h -
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
// Revision 1.4  2005/09/15 11:05:28  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2005/07/12 11:57:55  dkrajzew
// level 3 warnings removed; code style adapted
//
// Revision 1.3  2005/06/14 11:14:23  dksumo
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

#include <utils/gui/drawer/GUIBaseColorer.h>
#include <utils/gfx/RGBColor.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <microsim/MSEdge.h>
#include <guisim/GUILaneWrapper.h>
#include <GL/gl.h>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
template<class _T>
class GUIColorer_LaneByPurpose : public GUIBaseColorer<_T> {
public:
	GUIColorer_LaneByPurpose() { }

	virtual ~GUIColorer_LaneByPurpose() { }

	void setGlColor(const _T& i) const {
        switch(i.getPurpose()) {
        case MSEdge::EDGEFUNCTION_NORMAL:
            glColor3f(0, 0, 0);
            return;
        case MSEdge::EDGEFUNCTION_SOURCE:
            glColor3f(0, 1, 0);
            return;
        case MSEdge::EDGEFUNCTION_SINK:
            glColor3f(1, 0, 0);
            return;
        case MSEdge::EDGEFUNCTION_INTERNAL:
            glColor3f(0, 0, 1);
            return;
        default:
            throw 1;
        }
	}

	void setGlColor(double val) const {
        glColor3d(val, val, val);
    }

};
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
