//---------------------------------------------------------------------------//
//                        GUIDetectorDrawer_nT.cpp -
//  Class for drawing detectors with no tooltip information
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
// Revision 1.4  2005/04/27 09:44:26  dkrajzew
// level3 warnings removed
//
// Revision 1.3  2004/03/19 12:34:30  dkrajzew
// porting to FOX
//
// Revision 1.2  2003/09/17 06:45:11  dkrajzew
// some documentation added/patched
//
// Revision 1.1  2003/09/05 14:50:39  dkrajzew
// implementations of artefact drawers moved to folder "drawerimpl"
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#include <vector>
#include <guisim/GUIJunctionWrapper.h>
#include <utils/glutils/GLHelper.h>
#include "GUIJunctionDrawer_nT.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUIJunctionDrawer_nT::GUIJunctionDrawer_nT(
        std::vector<GUIJunctionWrapper*> &junctions)
    : GUIBaseJunctionDrawer(junctions)
{
}


GUIJunctionDrawer_nT::~GUIJunctionDrawer_nT()
{
}


void
GUIJunctionDrawer_nT::drawGLJunctions(size_t *which,
        size_t maxJunctions,
        GUISUMOAbstractView::JunctionColoringScheme )
{
    glLineWidth(1);
    glColor3f(0, 0, 0);
    // go through edges
    for(size_t i=0; i<maxJunctions; i++ ) {
        if(which[i]==0) {
            continue;
        }
        size_t pos = 1;
        for(size_t j=0; j<32; j++, pos<<=1) {
            if((which[i]&pos)!=0) {
                GLHelper::drawFilledPoly(
                    myJunctions[j+(i<<5)]->getShape(), true);
            }
        }
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
