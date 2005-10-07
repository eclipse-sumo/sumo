#ifndef BoolVector_h
#define BoolVector_h
//---------------------------------------------------------------------------//
//                        BoolVector.h -
//  A simple vector of bools
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
// Revision 1.5  2005/10/07 11:43:30  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.4  2005/09/15 12:13:08  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2005/04/28 09:02:46  dkrajzew
// level3 warnings removed
//
// Revision 1.2  2003/06/05 11:54:48  dkrajzew
// class templates applied; documentation added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <vector>


/* =========================================================================
 * definitions
 * ======================================================================= */
typedef std::vector<bool> BoolVector;


#endif

// Local Variables:
// mode:C++
// End:

