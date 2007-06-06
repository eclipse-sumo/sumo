/****************************************************************************/
/// @file    OutputDevice_File.cpp
/// @author  Daniel Krajzewicz
/// @date    2004
/// @version $Id$
///
// missing_desc
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include "OutputDevice_File.h"

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
OutputDevice_File::OutputDevice_File(std::ofstream *strm)
        : myFileStream(strm)
{}


OutputDevice_File::~OutputDevice_File()
{
    delete myFileStream;
}


bool
OutputDevice_File::ok()
{
    return myFileStream->good();
}


void
OutputDevice_File::close()
{
    myFileStream->close();
}


bool
OutputDevice_File::supportsStreams() const
{
    return true;
}


std::ostream &
OutputDevice_File::getOStream()
{
    return *myFileStream;
}


OutputDevice &
OutputDevice_File::writeString(const std::string &str)
{
    (*myFileStream) << str;
    return *this;
}


void
OutputDevice_File::closeInfo()
{
    (*myFileStream) << endl;
}



/****************************************************************************/

