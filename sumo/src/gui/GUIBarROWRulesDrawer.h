#ifndef GUIBarROWRulesDrawer_h
#define GUIBarROWRulesDrawer_h
//---------------------------------------------------------------------------//
//                        GUIBarROWRulesDrawer.h -
//  A class for
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sun, 10 Aug 2003
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
// Revision 1.2  2003/08/21 12:49:02  dkrajzew
// lane2lane connection display added
//
// Revision 1.1  2003/08/14 13:44:14  dkrajzew
// tls/row - drawer added
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
#include "GUISUMOAbstractView.h"
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
class GUIBarROWRulesDrawer :
        public GUISUMOAbstractView::GUIROWRulesDrawer {
public:
    /// constructor
    GUIBarROWRulesDrawer(std::vector<GUIEdge*> &edges);

    /// destructor
    ~GUIBarROWRulesDrawer();

    void drawGLROWs(size_t *which, size_t maxEdges,
        bool showToolTips, double width);

private:
    /// initialises the drawing
    void initStep(/*const double &width*/);

    void drawLinkRules(const GUILaneWrapper &lane);

    void initTexture(size_t no);

    void drawArrows(const GUILaneWrapper &lane);


private:
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
//#include "GUIBarROWRulesDrawer.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

