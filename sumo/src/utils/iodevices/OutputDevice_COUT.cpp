/****************************************************************************/
/// @file    OutputDevice_COUT.cpp
/// @author  Daniel Krajzewicz
/// @date    2004
/// @version $Id$
///
// An output device that encapsulates cout
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

#include <iostream>
#include "OutputDevice_COUT.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
OutputDevice_COUT::OutputDevice_COUT()
{}


OutputDevice_COUT::~OutputDevice_COUT()
{}


bool
OutputDevice_COUT::ok()
{
    return true;
}


void
OutputDevice_COUT::close()
{
    throw 1;
}

std::ostream &
OutputDevice_COUT::getOStream()
{
    return cout;
}


bool
OutputDevice_COUT::supportsStreams() const
{
    return true;
}


XMLDevice &
OutputDevice_COUT::writeString(const std::string &str)
{
    cout << str;
    return *this;
}


void
OutputDevice_COUT::closeInfo()
{
    cout << endl;
}



/****************************************************************************/

