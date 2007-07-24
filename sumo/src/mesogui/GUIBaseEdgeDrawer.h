#ifndef GUIBaseEdgeDrawer_h
#define GUIBaseEdgeDrawer_h
//---------------------------------------------------------------------------//
//                        GUIBaseEdgeDrawer.h -
//  Base class for edge drawing;
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 10.05.2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
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
// $Log: GUIBaseEdgeDrawer.h,v $
// Revision 1.8  2006/04/05 05:36:20  dksumo
// debugging
//
// Revision 1.7  2006/03/09 10:58:56  dksumo
// reworking the drawers
//
// Revision 1.6  2006/03/08 13:16:13  dksumo
// some work on lane visualization
//
// Revision 1.5  2006/01/03 11:04:30  dksumo
// new visualization settings implemented
//
// Revision 1.4  2005/10/06 13:39:47  dksumo
// using of a configuration file rechecked
//
// Revision 1.3  2005/09/20 06:13:02  dksumo
// floats and doubles replaced by SUMOReal; warnings removed
//
// Revision 1.2  2005/09/09 12:55:00  dksumo
// complete code rework: debug_new and config added
//
// Revision 1.1  2005/05/30 08:21:35  dksumo
// work on mesosim
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <fx.h>
#include <fx3d.h>
#include <map>
#include <utils/gfx/RGBColor.h>
#include <utils/glutils/polyfonts.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/drawer/GUIBaseColorer.h>

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
class GUIBaseEdgeDrawer {
public:
    /// constructor
    GUIBaseEdgeDrawer(const std::vector<_E1*> &edges)
		: myEdges(edges) { }


    /// destructor
	virtual ~GUIBaseEdgeDrawer() { }

    /// Sets the information whether the gl-id shall be set
    void setGLID(bool val)
    {
        myShowToolTips = val;
    }


    virtual void drawGLLanes(size_t *which, size_t maxEdges,
        SUMOReal width, GUIBaseColorer<_E1> &colorer,
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
                    colorer.setGlColor(*edge);
                    if(width*SUMO_const_halfLaneWidth*2.>1.) {
                        drawEdge(*edge, 1.);
                    } else {
                        drawLine(*edge);
                    }
	            }
		    }
	    }
	}


    /// Draws all lanes' names
    virtual void drawGLLaneNames(size_t *which, size_t maxEdges,
                                 SUMOReal width, GUISUMOAbstractView::VisualizationSettings &settings)
    {
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
                    _E2 *edge = static_cast<_E2*>(myEdges[j+(i<<5)]);
                    const _L1 &lane1 = edge->getLaneGeometry((size_t) 0);
                    const _L1 &lane2 = edge->getLaneGeometry(edge->nLanes()-1);
                    glPushMatrix();
                    Position2D p = lane1.getShape().positionAtLengthPosition(lane1.getShape().length()/(SUMOReal) 2.);
                    p.add(lane2.getShape().positionAtLengthPosition(lane2.getShape().length()/(SUMOReal) 2.));
                    p.mul(.5);
                    glTranslated(p.x(), p.y(), 0);
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                    pfSetPosition(0, 0);
                    pfSetScale(settings.edgeNameSize / width);
                    glColor3d(1, .5, 0);
                    SUMOReal w = pfdkGetStringWidth(edge->microsimID().c_str());
                    glRotated(180, 1, 0, 0);
                    SUMOReal angle = lane1.getShape().rotationDegreeAtLengthPosition(lane1.getShape().length()/(SUMOReal) 2.);
                    angle += 90;
                    if (angle>90&&angle<270) {
                        glColor3d(1, 0, .5);
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
		glColor3d(0, 0, 0);
	}

    /// draws a single vehicle;
    void drawEdge(const _E2 &edge, SUMOReal mult) const
    {
        if (myShowToolTips) {
            glPushName(edge.getGlID());
        }
        const _L1 &lane1 = edge.getLaneGeometry((size_t) 0);
        const _L1 &lane2 = edge.getLaneGeometry(edge.nLanes()-1);
        GLHelper::drawBoxLines(lane1.getShape(), lane2.getShape(), lane1.getShapeRotations(), lane1.getShapeLengths(), (SUMOReal) edge.nLanes()*SUMO_const_halfLaneAndOffset*mult);
        if (myShowToolTips) {
            glPopName();
        }
    }

    void drawLine(const _E2 &edge) const
    {
        if (myShowToolTips) {
            glPushName(edge.getGlID());
        }
        const _L1 &lane1 = edge.getLaneGeometry((size_t) 0);
        const _L1 &lane2 = edge.getLaneGeometry(edge.nLanes()-1);
        const DoubleVector &rots = lane1.getShapeRotations();
        const DoubleVector &lengths = lane1.getShapeLengths();
        const Position2DVector &geom1 = lane1.getShape();
        const Position2DVector &geom2 = lane2.getShape();
        for (size_t i=0; i<geom1.size()-1; i++) {
            GLHelper::drawLine(geom1[i], geom2[i], rots[i], lengths[i]);
        }
        if (myShowToolTips) {
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

