#ifndef MSLinkCont_h
#define MSLinkCont_h
//---------------------------------------------------------------------------//
//                        MSLinkCont.h -
//  A vector of links
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
// Revision 1.1  2003/02/07 10:41:51  dkrajzew
// updated
//
//

/* =========================================================================
 * included modules
 * ======================================================================= */
#include <vector>
#include "MSLink.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
typedef std::vector<MSLink*> MSLinkCont;


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "MSLinkCont.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
