//---------------------------------------------------------------------------//
//                        GUIJunctionDrawer_wT.cpp -
//  Class for drawing junctions with tooltips
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
// Revision 1.2  2003/09/17 06:45:11  dkrajzew
// some documentation added/patched
//
// Revision 1.1  2003/09/05 14:50:39  dkrajzew
// implementations of artefact drawers moved to folder "drawerimpl"
//
//
//
#include <vector>
#include <guisim/GUIJunctionWrapper.h>
#include <utils/glutils/GLHelper.h>
#include "GUIJunctionDrawer_wT.h"

GUIJunctionDrawer_wT::GUIJunctionDrawer_wT(
        std::vector<GUIJunctionWrapper*> &junctions)
    : GUIBaseJunctionDrawer(junctions)
{
}


GUIJunctionDrawer_wT::~GUIJunctionDrawer_wT()
{
}


void
GUIJunctionDrawer_wT::drawGLJunctions(size_t *which,
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
                glPushName(myJunctions[j+(i<<5)]->getGlID());
                GLHelper::drawFilledPoly(
                    myJunctions[j+(i<<5)]->getShape(), true);
                glPopName();
            }
        }
    }
}

