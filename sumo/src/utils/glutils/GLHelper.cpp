//---------------------------------------------------------------------------//
//                        GLHelper.cpp -
//  Some methods which help to draw certain geometrical objects in openGL
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.5  2004/11/23 10:35:13  dkrajzew
// debugging
//
// Revision 1.4  2004/07/02 09:46:28  dkrajzew
// some helper procedures for vss visualisation
//
// Revision 1.3  2004/03/19 13:01:29  dkrajzew
// porting to FOX
//
// Revision 1.2  2003/10/02 14:55:57  dkrajzew
// visualisation of E2-detectors implemented
//
// Revision 1.1  2003/06/05 14:27:45  dkrajzew
// some helping functions added; Makefile added
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include "GLHelper.h"
#include <utils/geom/GeomHelper.h>


/* =========================================================================
 * static member definitions
 * ======================================================================= */
std::vector<std::pair<float, float> > GLHelper::myCircleCoords;


/* =========================================================================
 * method definitions
 * ======================================================================= */
void
GLHelper::drawFilledPoly(const Position2DVector &v, bool close)
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_POLYGON);
    const Position2DVector::ContType &l = v.getCont();
    for(Position2DVector::ContType ::const_iterator i=l.begin(); i!=l.end(); i++) {
        const Position2D &p = *i;
        glVertex2d(p.x(), p.y());
    }
    if(close) {
        const Position2D &p = *(l.begin());
        glVertex2d(p.x(), p.y());
    }
    glEnd();
}


void
GLHelper::drawBoxLine(const Position2D &beg, double rot, double visLength,
                      double width)
{
    glPushMatrix();
    glTranslated(beg.x(), beg.y(), 0);
    glRotated( rot, 0, 0, 1 );
    glBegin(GL_QUADS);
    glVertex2d(-width, 0);
    glVertex2d(-width, -visLength);
    glVertex2d(width, -visLength);
    glVertex2d(width, 0);
    glEnd();
    glBegin(GL_LINES);
    glVertex2d(0, 0);
    glVertex2d(0, -visLength);
    glEnd();
    glPopMatrix();
}


void
GLHelper::drawLine(const Position2D &beg, double rot, double visLength)
{
    glPushMatrix();
    glTranslated(beg.x(), beg.y(), 0);
    glRotated( rot, 0, 0, 1 );
    glBegin(GL_LINES);
    glVertex2d(0, 0);
    glVertex2d(0, -visLength);
    glEnd();
    glPopMatrix();
}



void
GLHelper::drawFilledCircle(double width, int steps)
{
    if(myCircleCoords.size()==0) {
        for(int i=0; i<360; i+=10) {
            float x = (float) sin((float) i / 180.0 * PI);
            float y = (float) cos((float) i / 180.0 * PI);
            myCircleCoords.push_back(std::pair<float, float>(x, y));
        }
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    std::pair<float, float> p1 = myCircleCoords[0];
    for(int i=0; i<steps; i++) {
        const std::pair<float, float> &p2 =
            myCircleCoords[(size_t) (36.0/(float) steps * (float) i)];
        glBegin(GL_TRIANGLES);
        glVertex2d(p1.first * width, p1.second * width);
        glVertex2d(p2.first * width, p2.second * width);
        glVertex2d(0, 0);
        glEnd();
        p1 = p2;
    }
    const std::pair<float, float> &p2 = myCircleCoords[0];
    glBegin(GL_TRIANGLES);
    glVertex2d(p1.first * width, p1.second * width);
    glVertex2d(p2.first * width, p2.second * width);
    glVertex2d(0, 0);
    glEnd();
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


