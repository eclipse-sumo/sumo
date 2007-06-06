/****************************************************************************/
/// @file    GUISelectionLoader.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
//
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
#ifndef GUISelectionLoader_h
#define GUISelectionLoader_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>

// ===========================================================================
// class definitions
// ===========================================================================
class GUISelectionLoader
{
public:
    static void loadSelection(const std::string &file);

protected:
    /// Constructor
    GUISelectionLoader();
    /// Destructor
    ~GUISelectionLoader();
};


#endif

/****************************************************************************/

