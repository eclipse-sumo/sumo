/****************************************************************************/
/// @file    MFXUtils.h
/// @author  Daniel Krajzewicz
/// @date    2006-01-09
/// @version $Id$
///
// Some helper functions for FOX
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
#ifndef MFXUtils_h
#define MFXUtils_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MFXUtils
 * @brief Some helper functions for FOX
 */
class MFXUtils
{
public:
    /// Deletes all children of the given window
    static void deleteChildren(FXWindow *w);

    /// Returns true if either the file given by its name does not exist or the user allows overwriting it
    static FXbool userPermitsOverwritingWhenFileExists(
        FXWindow *parent, FXString &file);

};


#endif

/****************************************************************************/

