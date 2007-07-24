/****************************************************************************/
/// @file    MEGlobals.h
/// @author  Daniel Krajzewicz
/// @date    Tue, May 2005
/// @version $Id: MEGlobals.h 96 2007-06-06 07:40:46Z behr_mi $
///
// Declarations of global mesoscopic simulation variables
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MEGlobals_h
#define MEGlobals_h
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifdef HAVE_MESOSIM

#include <utils/common/StdDefs.h>


/* =========================================================================
 * global variable declarations
 * ======================================================================= */
extern SUMOReal segment_speed;
extern SUMOReal segment_tauff;
extern SUMOReal segment_taufj;
extern SUMOReal segment_taujf;
extern SUMOReal segment_taujj;

#endif // HAVE_MESOSIM

#endif

/****************************************************************************/

