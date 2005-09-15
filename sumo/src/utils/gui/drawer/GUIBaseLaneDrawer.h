#ifndef GUIBaseLaneDrawer_h
#define GUIBaseLaneDrawer_h
//---------------------------------------------------------------------------//
//                        GUIBaseLaneDrawer.h -
//  Base class for lane drawing;
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
// Revision 1.2  2004/10/29 06:20:47  dksumo
// patched some false dependencies
//
// Revision 1.1  2004/10/22 12:50:48  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.5  2004/08/02 11:29:37  dkrajzew
// first steps towards user-defined color gradients usage
//
// Revision 1.4  2004/03/19 12:34:30  dkrajzew
// porting to FOX
//
// Revision 1.3  2003/10/02 14:55:56  dkrajzew
// visualisation of E2-detectors implemented
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

#include <fx.h>
#include <fx3d.h>
#include <map>
#include <utils/gfx/RGBColor.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include "GUIGradients.h"
#include "GUIBaseColorer.h"

#include <GL/gl.h>

/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUILaneWrapper;
class Position2D;
class GUILaneRepresentation;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * Draws lanes as simple, one-colored straights
 */
template<class _E1, class _E2, class _L1>
class GUIBaseLaneDrawer {
public:
    /// constructor
    GUIBaseLaneDrawer(const std::vector<_E1*> &edges)
        : myEdges(edges), myUseExponential(true) { }


    /// destructor
    virtual ~GUIBaseLaneDrawer() { }


    virtual void drawGLLanes(size_t *which, size_t maxEdges,
        double width, GUIBaseColorer<_L1> &colorer)
    {
        // initialise drawing
        initStep();
        // go through edges
        for(size_t i=0; i<maxEdges; i++ ) {
            if(which[i]==0) {
                continue;
            }
            size_t pos = 1;
            for(size_t j=0; j<32; j++, pos<<=1) {
                if((which[i]&pos)!=0) {
                    _E2 *edge = static_cast<_E2*>(myEdges[j+(i<<5)]);
                    size_t noLanes = edge->nLanes();
                    // go through the current edge's lanes
                    for(size_t k=0; k<noLanes; k++) {
                        const _L1 &lane = edge->getLaneGeometry(k);
                        colorer.setGlColor(lane);
//                        if(lane.getPurpose()!=MSEdge::EDGEFUNCTION_INTERNAL) {
                        drawLane(lane, width);
      /*                  } else {
                            drawLane(lane, scheme, 0.1);
                        }*/
                    }
                }
            }
        }
    }


    void setUseExponential(bool val) {
        myUseExponential = val;
    }

protected:
    /// initialises the drawing
    virtual void initStep() {
        glLineWidth(1);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glColor3d(0, 0, 0);
    }

    /// draws a single vehicle
    virtual void drawLane(const _L1 &lane, double width) const = 0;

protected:
    /// The list of edges to consider at drawing
    const std::vector<_E1*> &myEdges;

private:
    bool myUseExponential;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

