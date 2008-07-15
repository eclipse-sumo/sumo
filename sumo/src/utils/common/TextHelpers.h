/****************************************************************************/
/// @file    TextHelpers.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id:TextHelpers.h 4690 2007-11-09 13:19:45Z dkrajzew $
///
// Some text helping functions
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
#ifndef TextHelpers_h
#define TextHelpers_h


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
/**
 * @class TextHelpers
 * Some string-handling helpers.
 */
class TextHelpers
{
public:
    /// Returns the next version of the given string
    static std::string version(std::string str);

};


#endif

/****************************************************************************/

