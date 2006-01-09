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
// Revision 1.10  2006/01/09 13:36:09  dkrajzew
// further visualization options added
//
// Revision 1.9  2005/11/09 07:29:43  dkrajzew
// debugging
//
// Revision 1.8  2005/10/07 11:44:40  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.7  2005/09/23 06:07:25  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.6  2005/09/15 12:18:45  dkrajzew
// LARGE CODE RECHECK
//
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
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include "GLHelper.h"
#include <utils/geom/GeomHelper.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * static member definitions
 * ======================================================================= */
std::vector<std::pair<SUMOReal, SUMOReal> > GLHelper::myCircleCoords;


/* =========================================================================
 * method definitions
 * ======================================================================= */
void
GLHelper::drawFilledPoly(const Position2DVector &v, bool close)
{
	if(v.size()==0) {
		return;
	}
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
GLHelper::drawBoxLine(const Position2D &beg, SUMOReal rot, SUMOReal visLength,
                      SUMOReal width)
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
GLHelper::drawLine(const Position2D &beg, SUMOReal rot, SUMOReal visLength)
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
GLHelper::drawFilledCircle(SUMOReal width, int steps)
{
    drawFilledCircle(width, steps, 0, 360);
}


void
GLHelper::drawFilledCircle(SUMOReal width, int steps, SUMOReal beg, SUMOReal end)
{
    if(myCircleCoords.size()==0) {
        for(int i=0; i<360; i+=10) {
            SUMOReal x = (SUMOReal) sin((SUMOReal) i / 180.0 * PI);
            SUMOReal y = (SUMOReal) cos((SUMOReal) i / 180.0 * PI);
            myCircleCoords.push_back(std::pair<SUMOReal, SUMOReal>(x, y));
        }
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    std::pair<SUMOReal, SUMOReal> p1 =
        beg==0 ? myCircleCoords[0] : myCircleCoords[((int) beg/10)%36];
    for(int i=(int) (beg/10); i<steps&&(36.0/(SUMOReal) steps * (SUMOReal) i)*10<end; i++) {
        const std::pair<SUMOReal, SUMOReal> &p2 =
            myCircleCoords[(size_t) (36.0/(SUMOReal) steps * (SUMOReal) i)];
        glBegin(GL_TRIANGLES);
        glVertex2d(p1.first * width, p1.second * width);
        glVertex2d(p2.first * width, p2.second * width);
        glVertex2d(0, 0);
        glEnd();
        p1 = p2;
    }
    const std::pair<SUMOReal, SUMOReal> &p2 =
        end==360 ? myCircleCoords[0] : myCircleCoords[((int) end/10)%36];
    glBegin(GL_TRIANGLES);
    glVertex2d(p1.first * width, p1.second * width);
    glVertex2d(p2.first * width, p2.second * width);
    glVertex2d(0, 0);
    glEnd();
}


void
GLHelper::drawTriangleAtEnd(const Line2D &l, float tLength,
                            float tWidth)
{
    if(l.length()<tLength) {
        tWidth = tWidth * l.length() / tLength;
        tLength = l.length();
    }
    Line2D rl(l.getPositionAtDistance(l.length()-tLength), l.p2());
    glPushMatrix();
    glTranslated(rl.p1().x(), rl.p1().y(), 0);
    glRotated(-l.atan2DegreeAngle(), 0, 0, 1);
    glBegin( GL_TRIANGLES );
    glVertex2d(0, -tLength);
    glVertex2d(-tWidth, 0);
    glVertex2d(+tWidth, 0);
    glEnd();
    glPopMatrix();
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


