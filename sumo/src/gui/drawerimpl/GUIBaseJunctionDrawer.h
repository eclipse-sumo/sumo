#ifndef GUIBaseJunctionDrawer_h
#define GUIBaseJunctionDrawer_h
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

#include <vector>
#include <gui/GUISUMOAbstractView.h>

class GUIJunctionWrapper;

class GUIBaseJunctionDrawer :
    public GUISUMOAbstractView::GUIJunctionDrawer
{
public:
    GUIBaseJunctionDrawer(std::vector<GUIJunctionWrapper*> &junctions);
    virtual ~GUIBaseJunctionDrawer();
    virtual void drawGLJunctions(size_t *which, size_t maxJunctions,
        GUISUMOAbstractView::JunctionColoringScheme scheme) = 0;
};

#endif
