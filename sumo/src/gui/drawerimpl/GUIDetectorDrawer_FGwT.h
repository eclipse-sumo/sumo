#ifndef GUIDetectorDrawer_FGwT_h
#define GUIDetectorDrawer_FGwT_h
//---------------------------------------------------------------------------//
//                        GUIDetectorDrawer_FGwT.h -
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
// $Log$
// Revision 1.2  2003/09/25 08:59:28  dkrajzew
// documentation patched
//
// Revision 1.1  2003/09/23 14:28:16  dkrajzew
// possibility to visualise detectors using different geometry complexities added
//
//

#include <gui/GUISUMOAbstractView.h>

class GUIDetectorDrawer_FGwT
        : public GUISUMOAbstractView::GUIDetectorDrawer
{
public:
    GUIDetectorDrawer_FGwT(std::vector<GUIDetectorWrapper*> &detectors)
        : GUISUMOAbstractView::GUIDetectorDrawer(detectors) { }
    ~GUIDetectorDrawer_FGwT() { }
    void drawGLDetectors(size_t *which, size_t maxDetectors,
        double scale);

};

#endif
