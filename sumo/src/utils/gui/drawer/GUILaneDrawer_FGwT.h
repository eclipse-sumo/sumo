#ifndef GUILaneDrawer_FGwT_h
#define GUILaneDrawer_FGwT_h
//---------------------------------------------------------------------------//
//                        GUILaneDrawer_FGwT.cpp -
//  Class for drawing lanes with full geom and tooltip information
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
// Revision 1.1  2004/11/23 10:38:30  dkrajzew
// debugging
//
// Revision 1.1  2004/10/22 12:50:48  dksumo
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
#include "config.h"
#endif // HAVE_CONFIG_H

#include <map>
#include <utils/gfx/RGBColor.h>
#include "GUIBaseLaneDrawer.h"
#include <utils/glutils/GLHelper.h>


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
class GUILaneDrawer_FGwT : public GUIBaseLaneDrawer<_E1, _E2, _L1> {
public:
    /// constructor
    GUILaneDrawer_FGwT(const std::vector<_E1*> &edges)
        : GUIBaseLaneDrawer<_E1, _E2, _L1>(edges) { }

    /// destructor
    ~GUILaneDrawer_FGwT() { }

private:
    /// draws a single vehicle;
    void drawLane(const _L1 &lane, double width) const
    {
        glPushName(lane.getGlID());
        const DoubleVector &rots = lane.getShapeRotations();
        const DoubleVector &lengths = lane.getShapeLengths();
        const Position2DVector &geom = lane.getShape();
        if(width>1.0) {
            for(size_t i=0; i<geom.size()-1; i++) {
                GLHelper::drawBoxLine(geom.at(i), rots[i], lengths[i], 1.5);
            }
        } else {
            for(size_t i=0; i<geom.size()-1; i++) {
                GLHelper::drawLine(geom.at(i), rots[i], lengths[i]);
            }
        }
        glPopName();
    }

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

