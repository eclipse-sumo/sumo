//---------------------------------------------------------------------------//
//                        GUIDetectorDrawer_SGwT.cpp -
//  Class for drawing detectors with tooltips
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
// Revision 1.2  2003/09/25 08:59:28  dkrajzew
// documentation patched
//
// Revision 1.1  2003/09/23 14:28:16  dkrajzew
// possibility to visualise detectors using different geometry complexities added
//
#include <gui/GUISUMOAbstractView.h>
#include "GUIDetectorDrawer_SGwT.h"
#include <guisim/GUIDetectorWrapper.h>


void
GUIDetectorDrawer_SGwT::drawGLDetectors(size_t *which, size_t maxDetectors,
                                      double scale)
{
    for(size_t i=0; i<maxDetectors; i++ ) {
        if(which[i]==0) {
            continue;
        }
        size_t pos = 1;
        for(size_t j=0; j<32; j++, pos<<=1) {
            if((which[i]&pos)!=0) {
                glPushName(myDetectors[j+(i<<5)]->getGlID());
                myDetectors[j+(i<<5)]->drawGL_SG(scale);
                glPopName();
            }
        }
    }
}
