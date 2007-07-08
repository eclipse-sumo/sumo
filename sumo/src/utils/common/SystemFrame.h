/****************************************************************************/
/// @file    SystemFrame.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 23.06.2003
/// @version $Id$
///
// A set of actions common to all applications
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
#ifndef SystemFrame_h
#define SystemFrame_h


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
class LogFile;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class SystemFrame
 * As almost all applications within the SUMO-packgae share the same
 * initialisation procedure, it is encapsulated within this class.
 * To leave the command line options variable for each application, they are
 * inserted into an application-global OptionsCont via a method to supply
 * at the call of the "init" method.
 */
class SystemFrame
{
public:
    static void addConfigurationOptions(OptionsCont &oc);

    /// Closes all of an applications subsystems
    static void close();

};


#endif

/****************************************************************************/

