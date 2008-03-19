/****************************************************************************/
/// @file    GUILaneDrawer.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 02.09.2003
/// @version $Id$
///
// Base class for lane drawing;
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUILaneDrawer_h
#define GUILaneDrawer_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>
#include <fx3d.h>
#include <map>
#include <utils/common/RGBColor.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/common/StdDefs.h>
#include "GUIGradients.h"
#include "GUIBaseColorer.h"
#include <foreign/polyfonts/polyfonts.h>

#include <GL/gl.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUILaneWrapper;
class Position2D;
class GUILaneRepresentation;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * Draws lanes as simple, one-colored straights
 */
template<class E1, class E2, class L1>
class GUILaneDrawer
{
public:
    /// constructor
    GUILaneDrawer(const std::vector<E1*> &edges)
            : myEdges(edges) { }


    /// destructor
    virtual ~GUILaneDrawer() { }

    /// Sets the information whether the gl-id shall be set
    void setGLID(bool val) {
        myShowToolTips = val;
    }

    /// Draws the lanes
    virtual void drawGLLanes(size_t *which, size_t maxEdges,
                             SUMOReal width, GUIBaseColorer<L1> &colorer,
                             GUISUMOAbstractView::VisualizationSettings &settings) {
        // initialise drawing
        initStep();
        // go through edges
        for (size_t i=0; i<maxEdges; i++) {
            if (which[i]==0) {
                continue;
            }
            size_t pos = 1;
            for (size_t j=0; j<32; j++, pos<<=1) {
                if ((which[i]&pos)!=0) {
                    E2 *edge = static_cast<E2*>(myEdges[j+(i<<5)]);
                    size_t noLanes = edge->nLanes();

                    // check whether lane boundaries shall be drawn
                    if (settings.laneShowBorders&&width>1.&& edge->getPurpose()!=MSEdge::EDGEFUNCTION_INTERNAL) {
                        if (myShowToolTips) {
                            glPushName(edge->getGlID());
                        }
                        glColor3d(1,1,1);
                        // draw white boundings
                        size_t k;
                        for (k=0; k<noLanes; k++) {
                            const L1 &lane = edge->getLaneGeometry(k);
                            GLHelper::drawBoxLines(lane.getShape(), lane.getShapeRotations(), lane.getShapeLengths(), SUMO_const_halfLaneAndOffset);
                        }
                        // draw black boxes
                        for (k=1; k<noLanes; k++) {
                            const L1 &lane = edge->getLaneGeometry(k);
                            colorer.setGlColor(lane);
                            const DoubleVector &rots = lane.getShapeRotations();
                            const DoubleVector &lengths = lane.getShapeLengths();
                            const Position2DVector &geom = lane.getShape();
                            int e = (int) geom.size() - 1;
                            for (int i=0; i<e; i++) {
                                glPushMatrix();
                                glTranslated(geom[i].x(), geom[i].y(), 0);
                                glRotated(rots[i], 0, 0, 1);
                                for (SUMOReal t=0; t<lengths[i]; t+=6) {
                                    glBegin(GL_QUADS);
                                    glVertex2d(-1.8, -t);
                                    glVertex2d(-1.8, -t-3.);
                                    glVertex2d(1.0, -t-3.);
                                    glVertex2d(1.0, -t);
                                    glEnd();
                                }
                                glPopMatrix();
                            }
                        }
                        if (myShowToolTips) {
                            glPopName();
                        }
                    }

                    // go through the current edge's lanes
                    if (true) {
                        for (size_t k=0; k<noLanes; k++) {
                            const L1 &lane = edge->getLaneGeometry(k);
                            colorer.setGlColor(lane);
                            if (width>1.) {
                                drawLane(lane, 1.);
                            } else {
                                drawLine(lane);
                            }
                        }
                    } else {
                        if (width>1.) {
                            drawEdge(*edge, 1.);
                        } else {
                            drawLine(*edge);
                        }
                    }
                }
            }
        }
    }

    /// Draws all lanes' names
    virtual void drawGLLaneNames(size_t *which, size_t maxEdges,
                                 SUMOReal width, GUISUMOAbstractView::VisualizationSettings &settings) {
        // set name colors
        glColor3f(settings.edgeNameColor.red(), settings.edgeNameColor.green(), settings.edgeNameColor.blue());
        // initialise drawing
        initStep();
        // go through edges
        for (size_t i=0; i<maxEdges; i++) {
            if (which[i]==0) {
                continue;
            }
            size_t pos = 1;
            for (size_t j=0; j<32; j++, pos<<=1) {
                if ((which[i]&pos)!=0) {
                    E2 *edge = static_cast<E2*>(myEdges[j+(i<<5)]);
                    const L1 &lane1 = edge->getLaneGeometry((size_t) 0);
                    const L1 &lane2 = edge->getLaneGeometry(edge->nLanes()-1);
                    glPushMatrix();
                    Position2D p = lane1.getShape().positionAtLengthPosition(lane1.getShape().length()/(SUMOReal) 2.);
                    p.add(lane2.getShape().positionAtLengthPosition(lane2.getShape().length()/(SUMOReal) 2.));
                    p.mul(.5);
                    glTranslated(p.x(), p.y(), 0);
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                    pfSetPosition(0, 0);
                    pfSetScale(settings.edgeNameSize / width);
                    SUMOReal w = pfdkGetStringWidth(edge->microsimID().c_str());
                    glRotated(180, 1, 0, 0);
                    SUMOReal angle = lane1.getShape().rotationDegreeAtLengthPosition(lane1.getShape().length()/(SUMOReal) 2.);
                    angle += 90;
                    if (angle>90&&angle<270) {
                        angle -= 180;
                    }
                    glRotated(angle, 0, 0, 1);
                    glTranslated(-w/2., .2*settings.edgeNameSize / width, 0);
                    pfDrawString(edge->microsimID().c_str());
                    glPopMatrix();
                }
            }
        }
    }

protected:
    /// initialises the drawing
    virtual void initStep() {
        glLineWidth(1);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    /// draws a single lane as a box list
    void drawLane(const L1 &lane, SUMOReal mult) const {
        if (myShowToolTips) {
            glPushName(lane.getGlID());
        }
        if(lane.getPurpose()!=MSEdge::EDGEFUNCTION_INTERNAL) {
            GLHelper::drawBoxLines(lane.getShape(), lane.getShapeRotations(), lane.getShapeLengths(), SUMO_const_halfLaneWidth*mult);
        } else {
            GLHelper::drawBoxLines(lane.getShape(), lane.getShapeRotations(), lane.getShapeLengths(), SUMO_const_quarterLaneWidth*mult);
        }
        if (myShowToolTips) {
            glPopName();
        }
    }

    /// draws a single edge as a box list
    void drawEdge(const E2 &edge, SUMOReal mult) const {
        if (myShowToolTips) {
            glPushName(edge.getGlID());
        }
        const L1 &lane1 = edge.getLaneGeometry((size_t) 0);
        const L1 &lane2 = edge.getLaneGeometry(edge.nLanes()-1);
        GLHelper::drawBoxLines(lane1.getShape(), lane2.getShape(), lane1.getShapeRotations(), lane1.getShapeLengths(), (SUMOReal) edge.nLanes()*SUMO_const_halfLaneAndOffset*mult);
        if (myShowToolTips) {
            glPopName();
        }
    }

    /// draws a lane as a line
    void drawLine(const L1 &lane) const {
        if (myShowToolTips) {
            glPushName(lane.getGlID());
        }
        const DoubleVector &rots = lane.getShapeRotations();
        const DoubleVector &lengths = lane.getShapeLengths();
        const Position2DVector &geom = lane.getShape();
        int e = (int) geom.size() - 1;
        for (int i=0; i<e; i++) {
            GLHelper::drawLine(geom[i], rots[i], lengths[i]);
        }
        if (myShowToolTips) {
            glPopName();
        }
    }

    /// draws an edge as a line
    void drawLine(const E2 &edge) const {
        if (myShowToolTips) {
            glPushName(edge.getGlID());
        }
        const L1 &lane1 = edge.getLaneGeometry((size_t) 0);
        const L1 &lane2 = edge.getLaneGeometry(edge.nLanes()-1);
        const DoubleVector &rots = lane1.getShapeRotations();
        const DoubleVector &lengths = lane1.getShapeLengths();
        const Position2DVector &geom1 = lane1.getShape();
        const Position2DVector &geom2 = lane2.getShape();
        int e = (int) geom1.size() - 1;
        for (int i=0; i<e; i++) {
            GLHelper::drawLine(geom1[i], geom2[i], rots[i], lengths[i]);
        }
        if (myShowToolTips) {
            glPopName();
        }
    }


protected:
    /// The list of edges to consider at drawing
    const std::vector<E1*> &myEdges;

    /// Information whether the gl-id shall be set
    bool myShowToolTips;

};


#endif

/****************************************************************************/

