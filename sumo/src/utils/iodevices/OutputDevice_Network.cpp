/****************************************************************************/
/// @file    OutputDevice_Network.cpp
/// @author  Felix Brack
/// @date    2006
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


// ==========================================================================
// included modules
// ==========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif // #ifdef WIN32

#include "OutputDevice_Network.h"
#include <utils/common/UtilExceptions.h>

#include <iostream>
#include <sstream>

#include "foreign/tcpip/socket.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // #ifdef CHECK_MEMORY_LEAKS


// ==========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ==========================================================================
// method definitions
// ==========================================================================
OutputDevice_Network::OutputDevice_Network(const std::string &host, const int port, const std::string &protocol)
{
    mySocket = new tcpip::Socket(host, port);
}

OutputDevice_Network::~OutputDevice_Network()
{
    delete mySocket;
}

std::ostream &
OutputDevice_Network::getOStream()
{
    return myMessage;
}

/****************************************************************************/

