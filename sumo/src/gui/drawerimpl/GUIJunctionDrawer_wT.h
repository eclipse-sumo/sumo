#ifndef GUIJunctionDrawer_wT_h
#define GUIJunctionDrawer_wT_h

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
#include "GUIBaseJunctionDrawer.h"

class GUIJunctionWrapper;

class GUIJunctionDrawer_wT :
    public GUIBaseJunctionDrawer
{
public:
    GUIJunctionDrawer_wT(std::vector<GUIJunctionWrapper*> &junctions);
    ~GUIJunctionDrawer_wT();
    void drawGLJunctions(size_t *which, size_t maxJunctions,
        GUISUMOAbstractView::JunctionColoringScheme scheme);
};

#endif
