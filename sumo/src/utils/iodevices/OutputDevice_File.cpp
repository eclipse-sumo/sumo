//---------------------------------------------------------------------------//
//                        OutputDevice_COUT.cpp -
//  An output device that encapsulates cout
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : 2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.4  2005/04/28 09:02:49  dkrajzew
// level3 warnings removed
//
// Revision 1.3  2004/11/23 10:35:47  dkrajzew
// debugging
//
// Revision 1.2  2004/11/22 12:54:56  dksumo
// tried to generelise the usage of detectors and output devices
//
// Revision 1.1  2004/10/22 12:50:58  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.2  2004/08/02 13:01:16  dkrajzew
// documentation added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#include <iostream>
#include "OutputDevice_File.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
OutputDevice_File::OutputDevice_File(std::ofstream *strm)
    : myFileStream(strm)
{
}


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


XMLDevice &
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




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
