#ifndef GUISimpleLaneDrawer_h
#define GUISimpleLaneDrawer_h
//---------------------------------------------------------------------------//
//                        GUISimpleLaneDrawer.h -
//  A class for lane drawing; Lanes are drawn as straight lines
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
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
// Revision 1.1  2003/02/07 10:34:14  dkrajzew
// files updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "GUIViewTraffic.h"


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
class GUISimpleLaneDrawer : public GUIViewTraffic::GUILaneDrawer {
public:
    /// constructor
    GUISimpleLaneDrawer();

    /// destructor
    ~GUISimpleLaneDrawer();

    /// initialises the drawing
    void initStep(const double &width);

    /// draws a single vehicle; no tool-tip informations (faster)
    void drawLaneNoTooltips(const GUILaneWrapper &lane,
        GUIViewTraffic::LaneColoringScheme scheme);

    /// draws a single vehicle; tool-tip informations shall be generated
    void drawLaneWithTooltips(const GUILaneWrapper &lane,
        GUIViewTraffic::LaneColoringScheme scheme);

    /// ends the drawing
    void closeStep();

private:
    /// sets the colour of the vehicle to draw
    void setLaneColor(const GUILaneWrapper &lane,
        GUIViewTraffic::LaneColoringScheme scheme);

private:
    /// a pointer set in initStep whether lines or quads shal be drawn
    bool _drawLines;
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "GUISimpleLaneDrawer.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

