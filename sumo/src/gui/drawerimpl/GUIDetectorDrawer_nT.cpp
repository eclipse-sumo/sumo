#include <gui/GUISUMOAbstractView.h>
#include "GUIDetectorDrawer_nT.h"
#include <guisim/GUIDetectorWrapper.h>

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
// Revision 1.1  2003/09/05 14:50:39  dkrajzew
// implementations of artefact drawers moved to folder "drawerimpl"
//
//
//


void 
GUIDetectorDrawer_nT::drawGLDetectors(size_t *which, size_t maxDetectors,
                                      double scale)
{
    for(size_t i=0; i<maxDetectors; i++ ) {
        if(which[i]==0) {
            continue;
        }
        size_t pos = 1;
        for(size_t j=0; j<32; j++, pos<<=1) {
            if((which[i]&pos)!=0) {
                myDetectors[j+(i<<5)]->drawGL(scale);
            }
        }
    }
}
