#ifndef TableTypes_h
#define TableTypes_h
//---------------------------------------------------------------------------//
//                        TableTypes.h -
//  A list for possible types of entries within the parameter table
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
// Revision 1.2  2003/06/05 11:37:31  dkrajzew
// class templates applied
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H


/* =========================================================================
 * definitions
 * ======================================================================= */
enum TableType {
    TT_DOUBLE,
    TT_MENU_BEGIN,
    TT_MENU_END
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "TableTypes.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

