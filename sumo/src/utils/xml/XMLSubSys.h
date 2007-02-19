/****************************************************************************/
/// @file    XMLSubSys.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 1 Jul 2002
/// @version $Id$
///
// Utility for initialisation and closing of the XML-subsystem
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
#ifndef XMLSubSys_h
#define XMLSubSys_h
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
// class definitions
// ===========================================================================
/**
 * @class XMLSubSys
 * some static methods for initialisation and closing of the xml-subsystem.
 * The Xerces-Parses needs such an initialisation and should also be closed.
 * As we use this system for both the input files and the configuration we
 * would have to check whether the system was initialised before. Instead,
 * we call XMLSubSys::init() once at the beginning of our application and
 * XMLSubSys::close() at the end.
 */
class XMLSubSys
{
public:
    /// initialises the xml-subsystem
    static bool init();

    /// closes the xml-subsystem
    static void close();

};


#endif

/****************************************************************************/

