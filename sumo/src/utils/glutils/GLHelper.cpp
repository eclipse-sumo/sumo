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
		glVertex2f(p.x(), p.y());
	}
    if(close) {
        const Position2D &p = *(l.begin());
        glVertex2f(p.x(), p.y());
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
    glVertex2f(-width, 0);
    glVertex2f(-width, -visLength);
    glVertex2f(width, -visLength);
    glVertex2f(width, 0);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(0, 0);
    glVertex2f(0, -visLength);
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
    glVertex2f(0, 0);
    glVertex2f(0, -visLength);
    glEnd();
    glPopMatrix();
}





/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GLHelper.icc"
//#endif

// Local Variables:
// mode:C++
// End:


