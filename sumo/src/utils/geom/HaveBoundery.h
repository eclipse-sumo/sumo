#ifndef HaveBoundery_h
#define HaveBoundery_h
//---------------------------------------------------------------------------//
//                        HaveBoundery.h -
//  The base class for objects that do have a boundery
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
// Revision 1.4  2004/03/19 13:01:11  dkrajzew
// methods needed for the new selection within the gui added; some style adaptions
//
// Revision 1.3  2003/11/12 13:48:49  dkrajzew
// visualisation of tl-logics added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <utils/geom/Boundery.h>


/* =========================================================================
 * included modules
 * ======================================================================= */
/**
 * @class HaveBoundery
 * Class which allows the retrieval of a boundery
 */
class HaveBoundery {
public:
    /// Constructor
    HaveBoundery() { }

    /// Destructor
    virtual ~HaveBoundery() { }

    /// Returns the object's boundery
    virtual Boundery getBoundery() const = 0;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "HaveBoundery.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
