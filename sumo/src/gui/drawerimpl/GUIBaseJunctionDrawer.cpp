
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
#include <vector>
#include <guisim/GUIJunctionWrapper.h>
#include <utils/glutils/GLHelper.h>
#include "GUIBaseJunctionDrawer.h"

GUIBaseJunctionDrawer::GUIBaseJunctionDrawer(
        std::vector<GUIJunctionWrapper*> &junctions)
    : GUIJunctionDrawer(junctions)
{
}


GUIBaseJunctionDrawer::~GUIBaseJunctionDrawer()
{
}
