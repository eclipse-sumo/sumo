#ifndef GUIROWDrawer_FGwT_h
#define GUIROWDrawer_FGwT_h
//---------------------------------------------------------------------------//
//                        GUIROWDrawer_FGwT.h -
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
// Revision 1.3  2004/03/19 12:34:30  dkrajzew
// porting to FOX
//
// Revision 1.2  2003/11/12 13:45:25  dkrajzew
// visualisation of tl-logics added
//
// Revision 1.1  2003/10/15 11:35:06  dkrajzew
// old row-drawer replaced by new ones; setting of name information seems tobe necessary
//
// Revision 1.2  2003/09/17 06:45:11  dkrajzew
// some documentation added/patched
//
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
#include <microsim/MSLink.h>
#include "GUIBaseROWDrawer.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUILaneWrapper;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * Draws lanes as simple, one-colored straights
 */
class GUIROWDrawer_FGwT :
        public GUIBaseROWDrawer {
public:
    /// constructor
    GUIROWDrawer_FGwT(std::vector<GUIEdge*> &edges);

    /// destructor
    ~GUIROWDrawer_FGwT();

private:
    /// Function top draw the lnk rules
    void drawLinkRules(const GUINet &net,
        const GUILaneWrapper &lane);

    /// Function to draw the direction arrows
    void drawArrows(const GUILaneWrapper &lane);

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "GUIROWDrawer_FGwT.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

