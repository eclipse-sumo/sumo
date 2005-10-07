//---------------------------------------------------------------------------//
//                        version.cpp -
//  A string containing the current program version
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
/*
namespace
{
    const char rcsid[] =
    "$Id$";
}
*/
// $Log$
// Revision 1.7  2005/10/07 11:48:01  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.6  2005/09/15 12:27:08  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.5  2004/12/20 15:00:36  dkrajzew
// version patched
//
// Revision 1.4  2004/12/20 14:54:32  dkrajzew
// version patched
//
// Revision 1.3  2004/12/20 13:17:02  dkrajzew
// version number patched
//
// Revision 1.2  2004/07/02 09:52:38  dkrajzew
// all 0.8.0.2 changes
//
// Revision 1.1  2003/06/18 10:57:09  dkrajzew
// quite unsane specification within a cpp file only is now split on a
//  cpp and a h file
//
// Revision 1.2  2003/06/05 14:45:28  dkrajzew
// class templates applied; documentation added
//
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

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * definitions
 * ======================================================================= */
const char *version = "0.9.0";


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


