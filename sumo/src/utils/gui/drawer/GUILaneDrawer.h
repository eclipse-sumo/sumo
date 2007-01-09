#ifndef GUILaneDrawer_h
#define GUILaneDrawer_h
//---------------------------------------------------------------------------//
//                        GUILaneDrawer.h -
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
// Revision 1.2  2007/01/09 11:12:01  dkrajzew
// the names of nodes, additional structures, vehicles, edges, pois may now be shown
//
// Revision 1.1  2006/12/12 12:19:26  dkrajzew
// removed simple/full geometry options; everything is now drawn using full geometry
//
// Revision 1.11  2006/10/26 10:27:30  dkrajzew
// debugging
//
// Revision 1.10  2006/03/27 07:33:38  dkrajzew
// extracted drawing of lane geometries
//
// Revision 1.9  2006/03/17 11:03:07  dkrajzew
// made access to positions in Position2DVector c++ compliant
//
// Revision 1.8  2006/03/08 13:16:23  dkrajzew
// some work on lane visualization
//
// Revision 1.7  2006/01/19 08:49:46  dkrajzew
// debugging for the next release
//
// Revision 1.6  2006/01/16 13:38:33  dkrajzew
// debugging
//
// Revision 1.5  2006/01/09 11:50:21  dkrajzew
// new visualization settings implemented
//
// Revision 1.4  2005/10/07 11:45:09  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.3  2005/09/23 06:07:53  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
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
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <fx.h>
#include <fx3d.h>
#include <map>
#include <utils/gfx/RGBColor.h>
#include <utils/glutils/GLHelper.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/common/StdDefs.h>
#include "GUIGradients.h"
#include "GUIBaseColorer.h"
#include <utils/glutils/polyfonts.h>

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
class GUILaneDrawer {
public:
    /// constructor
    GUILaneDrawer(const std::vector<_E1*> &edges)
		: myEdges(edges) { }


    /// destructor
	virtual ~GUILaneDrawer() { }

    void setGLID(bool val) {
        myShowToolTips = val;
    }

    virtual void drawGLLanes(size_t *which, size_t maxEdges,
        SUMOReal width, GUIBaseColorer<_L1> &colorer,
        GUISUMOAbstractView::VisualizationSettings &settings)
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

                    // check whether lane boundaries shall be drawn
                    if(settings.laneShowBorders&&width>1.) {
                        if(myShowToolTips) {
                            glPushName(edge->getGlID());
                        }
                        glColor3d(1,1,1);
                        // draw white boundings
                        size_t k;
                        for(k=0; k<noLanes; k++) {
                            const _L1 &lane = edge->getLaneGeometry(k);
                            GLHelper::drawBoxLines(lane.getShape(), lane.getShapeRotations(), lane.getShapeLengths(), SUMO_const_halfLaneAndOffset);
                        }
                        // draw black boxes
                        for(k=1; k<noLanes; k++) {
                            const _L1 &lane = edge->getLaneGeometry(k);
                            colorer.setGlColor(lane);
		                    const DoubleVector &rots = lane.getShapeRotations();
	                        const DoubleVector &lengths = lane.getShapeLengths();
		                    const Position2DVector &geom = lane.getShape();
                            for(size_t i=0; i<geom.size()-1; i++) {
                                glPushMatrix();
                                glTranslated(geom[i].x(), geom[i].y(), 0);
                                glRotated( rots[i], 0, 0, 1 );
                                for(SUMOReal t=0; t<lengths[i]; t+=6) {
                                    glBegin(GL_QUADS);
                                    glVertex2d(-1.8, -t);
                                    glVertex2d(-1.8, -t-3.);
                                    glVertex2d(1.8, -t-3.);
                                    glVertex2d(1.8, -t);
                                    glEnd();
                                }
                                glPopMatrix();
                            }
                        }
                        if(myShowToolTips) {
                            glPopName();
                        }
                    }

		            // go through the current edge's lanes
                    if(true) {
        			        for(size_t k=0; k<noLanes; k++) {
	        			        const _L1 &lane = edge->getLaneGeometry(k);
		        				colorer.setGlColor(lane);
//			        		      if(lane.getPurpose()!=MSEdge::EDGEFUNCTION_INTERNAL) {
                                if(width>1.) {//&&lane.getPurpose()!=MSEdge::EDGEFUNCTION_INTERNAL) {
//                                if(width>1.&&lane.getPurpose()!=MSEdge::EDGEFUNCTION_INTERNAL) {
				        		    drawLane(lane, 1.);
                                } else {
                                    drawLine(lane);
                                }
    	  /*                          } else {
	    	                        drawLane(lane, scheme, 0.1);
    		    	            }*/
                            }
                    } else {
                        if(width>1.) {
                            drawEdge(*edge, 1.);
                        } else {
                            drawLine(*edge);
                        }
                    }
                    // check whether the name shall be drawn
                    if(settings.drawEdgeName) {
                        const _L1 &lane = edge->getLaneGeometry((int) 0);
                        glPushMatrix();
                        Position2D p = lane.getShape().positionAtLengthPosition(lane.getShape().length()/2.);
                        glTranslated(p.x(), p.y(), 0);
                        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                        pfSetPosition(0, 0);
                        pfSetScale(settings.junctionNameSize / width);
                        glColor3d(1, .5, 0);
                        SUMOReal w = pfdkGetStringWidth(edge->microsimID().c_str());
                        glRotated(180, 1, 0, 0);
                        SUMOReal angle = lane.getShape().rotationDegreeAtLengthPosition(lane.getShape().length()/2.);
                        angle += 90;
                        if(angle>90&&angle<270) {
                            glColor3d(1, 0, .5);
                            angle -= 180;
                        }
                        glRotated(angle, 0, 0, 1);
                        glTranslated(-w/2., 0.4, 0);
                        pfDrawString(edge->microsimID().c_str());
                        glPopMatrix();
                    }
	            }
		    }
	    }
	}

protected:
    /// initialises the drawing
    virtual void initStep() {
		glLineWidth(1);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glColor3d(0, 0, 0);
	}

    /// draws a single lane as a box list
    void drawLane(const _L1 &lane, SUMOReal mult) const
	{
        if(myShowToolTips) {
            glPushName(lane.getGlID());
        }
        GLHelper::drawBoxLines(lane.getShape(), lane.getShapeRotations(), lane.getShapeLengths(), SUMO_const_halfLaneWidth*mult);
        if(myShowToolTips) {
            glPopName();
        }
	}

    /// draws a single edge as a box list
    void drawEdge(const _E2 &edge, SUMOReal mult) const
	{
        if(myShowToolTips) {
            glPushName(edge.getGlID());
        }
        const _L1 &lane1 = edge.getLaneGeometry((size_t) 0);
        const _L1 &lane2 = edge.getLaneGeometry(edge.nLanes()-1);
        GLHelper::drawBoxLines(lane1.getShape(), lane2.getShape(), lane1.getShapeRotations(), lane1.getShapeLengths(), (SUMOReal) edge.nLanes()*SUMO_const_halfLaneAndOffset*mult);
        if(myShowToolTips) {
            glPopName();
        }
	}

    /// draws a lane as a line
    void drawLine(const _L1 &lane) const
	{
        if(myShowToolTips) {
            glPushName(lane.getGlID());
        }
		const DoubleVector &rots = lane.getShapeRotations();
		const DoubleVector &lengths = lane.getShapeLengths();
		const Position2DVector &geom = lane.getShape();
        for(size_t i=0; i<geom.size()-1; i++) {
		    GLHelper::drawLine(geom[i], rots[i], lengths[i]);
		}
        if(myShowToolTips) {
            glPopName();
        }
	}

    /// draws an edge as a line
    void drawLine(const _E2 &edge) const
	{
        if(myShowToolTips) {
            glPushName(edge.getGlID());
        }
        const _L1 &lane1 = edge.getLaneGeometry((size_t) 0);
        const _L1 &lane2 = edge.getLaneGeometry(edge.nLanes()-1);
		const DoubleVector &rots = lane1.getShapeRotations();
	    const DoubleVector &lengths = lane1.getShapeLengths();
		const Position2DVector &geom1 = lane1.getShape();
        const Position2DVector &geom2 = lane2.getShape();
        for(size_t i=0; i<geom1.size()-1; i++) {
		    GLHelper::drawLine(geom1[i], geom2[i], rots[i], lengths[i]);
		}
        if(myShowToolTips) {
            glPopName();
        }
	}


protected:
    /// The list of edges to consider at drawing
    const std::vector<_E1*> &myEdges;

    /// Information whether the gl-id shall be set
    bool myShowToolTips;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

