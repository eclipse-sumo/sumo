#ifndef GUIBaseROWDrawer_h
#define GUIBaseROWDrawer_h
//---------------------------------------------------------------------------//
//                        GUIBaseROWDrawer.h -
//  Base class for drawing right of way - rules
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
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <map>
#include <utils/gfx/RGBColor.h>
#include <microsim/MSLink.h>
#include <gui/GUISUMOAbstractView.h>
#include <qimage.h>


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
class GUIBaseROWDrawer :
        public GUISUMOAbstractView::GUIROWRulesDrawer {
public:
    /// constructor
    GUIBaseROWDrawer(std::vector<GUIEdge*> &edges);

    /// destructor
    virtual ~GUIBaseROWDrawer();

    void drawGLROWs(size_t *which, size_t maxEdges, double width);

private:
    /// initialises the drawing
    virtual void initStep();

    virtual void drawLinkRules(const GUILaneWrapper &lane) = 0;

    void initTexture(size_t no);

    virtual void drawArrows(const GUILaneWrapper &lane) = 0;


protected:
    /// Definition of a storage for link colors
    typedef std::map<MSLink::LinkState, RGBColor> LinkColorMap;

    /// The colors to use for certain link types
    LinkColorMap myLinkColors;

    bool myAmInitialised;

    size_t myTextureIDs[6];

    QImage myTextures[6];

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "GUIBaseROWDrawer.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

