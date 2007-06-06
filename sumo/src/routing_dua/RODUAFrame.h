/****************************************************************************/
/// @file    RODUAFrame.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Some helping methods for usage within sumo and sumo-gui
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
#ifndef RODUAFrame_h
#define RODUAFrame_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
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
 * @class RODUAFrame
 */
class RODUAFrame
{
public:
    static void fillOptions(OptionsCont &oc);

    static bool checkOptions(OptionsCont &oc);

    static void addImportOptions(OptionsCont &oc);

    static void addDUAOptions(OptionsCont &oc);


};


#endif

/****************************************************************************/

