#ifndef GUIAggregatedLaneDrawer_h
#define GUIAggregatedLaneDrawer_h
//---------------------------------------------------------------------------//
//                        GUIAggregatedLaneDrawer.h -
//  A class for lane drawing; Lanes are drawn as straight lines
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
#include <gui/GUISUMOAbstractView.h>


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
class GUIAggregatedLaneDrawer : public GUISUMOAbstractView::GUILaneDrawer {
public:
    /// constructor
    GUIAggregatedLaneDrawer(std::vector<GUIEdge*> &edges);

    /// destructor
    ~GUIAggregatedLaneDrawer();

    void drawGLLanes(size_t *which, size_t maxEdges,
        double width, GUISUMOAbstractView::LaneColoringScheme scheme);

private:
    /// initialises the drawing
    void initStep(/*const double &width*/);

    /// draws a single vehicle; no tool-tip informations (faster)
    void drawLaneNoTooltips(const GUILaneWrapper &lane,
        GUISUMOAbstractView::LaneColoringScheme scheme, double width);

    /// draws a single vehicle; tool-tip informations shall be generated
    void drawLaneWithTooltips(const GUILaneWrapper &lane,
        GUISUMOAbstractView::LaneColoringScheme scheme, double width);

    /// sets the colour of the vehicle to draw
    void setLaneColor(const GUILaneWrapper &lane,
        GUISUMOAbstractView::LaneColoringScheme scheme);

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "GUIAggregatedLaneDrawer.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

