/****************************************************************************/
/// @file    ROFrame.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Class encapsulating options common to routers
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
#ifndef ROFrame_h
#define ROFrame_h
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


// ===========================================================================
// class declarations
// ===========================================================================
class OptionsCont;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROFrame
 * @brief Class encapsulating options common to routers
 */
class ROFrame
{
public:
    /// Builds the simulation options
    static void fillOptions(OptionsCont &oc);

    /// Checks whether options are valid
    static bool checkOptions(OptionsCont &oc);

};


#endif

/****************************************************************************/

