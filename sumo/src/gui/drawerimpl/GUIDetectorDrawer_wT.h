#ifndef GUIDetectorDrawer_wT_h
#define GUIDetectorDrawer_wT_h
//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.1  2003/09/05 14:50:39  dkrajzew
// implementations of artefact drawers moved to folder "drawerimpl"
//
//
//

#include <gui/GUISUMOAbstractView.h>

class GUIDetectorDrawer_wT 
        : public GUISUMOAbstractView::GUIDetectorDrawer
{
public:
    GUIDetectorDrawer_wT(std::vector<GUIDetectorWrapper*> &detectors) 
        : GUISUMOAbstractView::GUIDetectorDrawer(detectors) { }
    ~GUIDetectorDrawer_wT() { }
    void drawGLDetectors(size_t *which, size_t maxDetectors,
        double scale);

};

#endif
