#ifndef HaveBoundary_h
#define HaveBoundary_h
//---------------------------------------------------------------------------//
//                        HaveBoundary.h -
//  The base class for objects that do have a boundary
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
// Revision 1.2  2005/09/15 12:18:19  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.1  2004/11/23 10:34:46  dkrajzew
// debugging
//
// Revision 1.1  2004/10/29 06:25:23  dksumo
// boundery renamed to boundary
//
// Revision 1.1  2004/10/22 12:50:45  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.5  2004/07/02 09:44:40  dkrajzew
// changes for 0.8.0.2
//
// Revision 1.4  2004/03/19 13:01:11  dkrajzew
// methods needed for the new selection within the gui added; some style adaptions
//
// Revision 1.3  2003/11/12 13:48:49  dkrajzew
// visualisation of tl-logics added
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <utils/geom/Boundary.h>


/* =========================================================================
 * included modules
 * ======================================================================= */
/**
 * @class HaveBoundary
 * Class which allows the retrieval of a boundary
 */
class HaveBoundary {
public:
    /// Constructor
    HaveBoundary() { }

    /// Destructor
    virtual ~HaveBoundary() { }

    /// Returns the object's boundary
    virtual Boundary getBoundary() const = 0;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
