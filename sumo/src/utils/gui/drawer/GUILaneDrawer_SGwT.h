#ifndef GUILaneDrawer_SGwT_h
#define GUILaneDrawer_SGwT_h
//---------------------------------------------------------------------------//
//                        GUILaneDrawer_SGwT.cpp -
//  Class for drawing lanes with simple geometry and tooltips
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 02.09.2003
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
// Revision 1.2  2005/09/15 12:19:10  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.1  2004/11/23 10:38:30  dkrajzew
// debugging
//
// Revision 1.1  2004/10/22 12:50:49  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.2  2003/09/17 06:45:11  dkrajzew
// some documentation added/patched
//
// Revision 1.1  2003/09/05 14:50:39  dkrajzew
// implementations of artefact drawers moved to folder "drawerimpl"
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <map>
#include <utils/gfx/RGBColor.h>
#include "GUIBaseLaneDrawer.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUILaneWrapper;
class Position2D;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * Draws lanes as simple, one-colored straights
 */
template<class _E1, class _E2, class _L1>
class GUILaneDrawer_SGwT : public GUIBaseLaneDrawer<_E1, _E2, _L1> {
public:
    /// constructor
    GUILaneDrawer_SGwT(const std::vector<_E1*> &edges)
        : GUIBaseLaneDrawer<_E1, _E2, _L1>(edges) { }

    /// destructor
        ~GUILaneDrawer_SGwT() { }

private:
    /// draws a single vehicle;
    void drawLane(const _L1 &lane, double width) const
    {
        glPushName(lane.getGlID());
        if(width>1.0) {
            glPushMatrix();
            const Position2D &beg = lane.getBegin();
            glTranslated(beg.x(), beg.y(), 0);
            glRotated( lane.getRotation(), 0, 0, 1 );
            double visLength = -lane.visLength();
            glBegin( GL_QUADS );
            glVertex2d(0-1.5, 0);
            glVertex2d(-1.5, visLength);
            glVertex2d(1.5, visLength);
            glVertex2d(1.5, 0);
            glEnd();
            glBegin( GL_LINES);
            // without the substracted offsets, lines are partially longer
            //  than the boxes
            glVertex2d(0, 0-.1);
            glVertex2d(0, visLength-.1);
            glEnd();
            glPopMatrix();
        } else {
            const Position2D &begin = lane.getBegin();
            const Position2D &end = lane.getEnd();
            glBegin( GL_LINES);
            glVertex2d(begin.x(), begin.y());
            glVertex2d(end.x(), end.y());
            glEnd();
        }
        glPopName();
    }


};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

