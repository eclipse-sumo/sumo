//---------------------------------------------------------------------------//
//                        GUIBaseLaneDrawer.h -
//  Base class for drawing lanes
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
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
#include "config.h"
#endif // HAVE_CONFIG_H

#include <iostream>
#include <string>
#include <microsim/MSEdge.h>
#include <guisim/GUIVehicle.h>
#include <guisim/GUIEdge.h>
#include <guisim/GUILaneWrapper.h>
#include <gui/GUIGlobals.h>
#include "GUIBaseLaneDrawer.h"
#include <utils/geom/GeomHelper.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUIBaseLaneDrawer::GUIBaseLaneDrawer(std::vector<GUIEdge*> &edges)
    : myEdges(edges), myUseExponential(true)
{
}


GUIBaseLaneDrawer::~GUIBaseLaneDrawer()
{
}


void
GUIBaseLaneDrawer::drawGLLanes(size_t *which, size_t maxEdges,
                               double width,
                               GUISUMOAbstractView::LaneColoringScheme scheme)
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
                GUIEdge *edge = static_cast<GUIEdge*>(myEdges[j+(i<<5)]);
                size_t noLanes = edge->nLanes();
                // go through the current edge's lanes
                for(size_t k=0; k<noLanes; k++) {
                    const GUILaneWrapper &lane = edge->getLaneGeometry(k);
                    if(lane.getPurpose()!=MSEdge::EDGEFUNCTION_INTERNAL) {
                        drawLane(lane, scheme, width);
                    } else {
                        drawLane(lane, scheme, 0.1);
                    }
                }
            }
        }
    }
}


void
GUIBaseLaneDrawer::initStep()
{
    glLineWidth(1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColor3f(0, 0, 0);
}


void
GUIBaseLaneDrawer::setUseExponential(bool val)
{
    myUseExponential = val;
}


void
GUIBaseLaneDrawer::setLaneColor(const GUILaneWrapper &lane,
                                GUISUMOAbstractView::LaneColoringScheme scheme)
{
    switch(scheme) {
    case GUISUMOAbstractView::LCS_BLACK:
        glColor3f(0, 0, 0);
        break;
    case GUISUMOAbstractView::LCS_BY_PURPOSE:
        switch(lane.getPurpose()) {
        case MSEdge::EDGEFUNCTION_NORMAL:
            glColor3f(0, 0, 0);
            break;
        case MSEdge::EDGEFUNCTION_SOURCE:
            glColor3f(0, 1, 0);
            break;
        case MSEdge::EDGEFUNCTION_SINK:
            glColor3f(1, 0, 0);
            break;
        case MSEdge::EDGEFUNCTION_INTERNAL:
            glColor3f(0, 0, 1);
            break;
        case MSEdge::EDGEFUNCTION_UNKNOWN:
        default:
            throw 1;
        }
        break;
    case GUISUMOAbstractView::LCS_BY_SPEED:
        {
            double speed = lane.maxSpeed();
            double maxSpeed = GUILaneWrapper::getOverallMaxSpeed();
            double fact = speed / maxSpeed / 2.0;
            glColor3f(1.0-fact, 0.5, 0.5+fact);
        }
        break;
    case GUISUMOAbstractView::LCS_BY_SELECTION:
        {
            if(gfIsSelected(GLO_LANE, lane.getGlID())) {
                glColor3f(0, 0.8, 0.8);
            } else {
                glColor3f(0, 0, 0);
            }
        }
        break;
    case GUISUMOAbstractView::LCS_BY_DENSITY:
        {
            double density = myUseExponential
                ? lane.getAggregatedFloat(E2::DENSITY)
                : lane.getAggregatedNormed(E2::DENSITY, 0);
            if(density==-1) {
                glColor3f(0.5, 0.5, 0.5);
            } else {
                density /= 2.0;
                glColor3f(0.5, 1.0-density, 0.5+density);
            }
        }
        break;
    case GUISUMOAbstractView::LCS_BY_MEAN_SPEED:
        {
            double speed = myUseExponential
                ? lane.getAggregatedFloat(E2::SPACE_MEAN_SPEED)
                : lane.getAggregatedNormed(E2::SPACE_MEAN_SPEED, 0);
            if(speed==-1) {
                glColor3f(0.5, 0.5, 0.5);
            } else {
                speed /= 2.0;
                glColor3f(1.0-speed, 0.5, 0.5+speed);
            }
        }
        break;
    case GUISUMOAbstractView::LCS_BY_MEAN_HALTS:
        {
            double halts = myUseExponential
                ? lane.getAggregatedFloat(E2::HALTING_DURATION_MEAN)
                : lane.getAggregatedNormed(E2::HALTING_DURATION_MEAN, 0);
            if(halts==-1) {
                glColor3f(0.5, 0.5, 0.5);
            } else {
                halts /= 2.0;
                glColor3f(0.5+halts, 1.0-halts, 0);
            }
        }
        break;
    default:
        throw 1;
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


