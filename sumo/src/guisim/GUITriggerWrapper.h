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
// Revision 1.4  2005/09/15 11:06:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2005/07/12 12:19:31  dkrajzew
// code style adapted
//
// Revision 1.2  2004/11/24 08:46:43  dkrajzew
// recent changes applied
//
// Revision 1.1  2004/07/02 08:59:58  dkrajzew
// detector handling (handling of additional elements) revisited
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <utils/geom/HaveBoundary.h>
#include <utils/geom/Position2D.h>
/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class GUITriggerWrapper
    : public HaveBoundary {
public:
    GUITriggerWrapper();
    ~GUITriggerWrapper();
    Boundary getBoundary() const;
    Position2D getPosition() const;
};

#endif
// Local Variables:
// mode:C++
// End:
