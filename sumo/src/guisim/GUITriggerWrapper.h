#ifndef GUITriggerWrapper_h
#define GUITriggerWrapper_h
//---------------------------------------------------------------------------//
//                        GUITriggerWrapper.h -
//  A MSNet extended by some values for usage within the gui
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
// $Log$
// Revision 1.1  2004/07/02 08:59:58  dkrajzew
// detector handling (handling of additional elements) revisited
//

#include <utils/geom/HaveBoundery.h>
#include <utils/geom/Position2D.h>

class GUITriggerWrapper
    : public HaveBoundery {
public:
    GUITriggerWrapper();
    ~GUITriggerWrapper();
    Boundery getBoundery() const;
    Position2D getPosition() const;
};

#endif
