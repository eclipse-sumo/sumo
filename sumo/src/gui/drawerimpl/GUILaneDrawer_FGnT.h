#ifndef GUILaneDrawer_FGnT_h
#define GUILaneDrawer_FGnT_h
//---------------------------------------------------------------------------//
//                        GUILaneDrawer_FGnT.h -
//  A class for lane drawing; Lanes are drawn as straight lines
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 02.09.2003
//  copyright            : (C) 2002 by Daniel Krajzewicz
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
// Revision 1.1  2003/09/05 14:50:39  dkrajzew
// implementations of artefact drawers moved to folder "drawerimpl"
//
//
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
class GUILaneDrawer_FGnT : public GUIBaseLaneDrawer {
public:
    /// constructor
    GUILaneDrawer_FGnT(std::vector<GUIEdge*> &edges);

    /// destructor
    ~GUILaneDrawer_FGnT();

private:
    /// draws a single vehicle
    void drawLane(const GUILaneWrapper &lane,
        GUISUMOAbstractView::LaneColoringScheme scheme, double width);

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "GUILaneDrawer_FGnT.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

