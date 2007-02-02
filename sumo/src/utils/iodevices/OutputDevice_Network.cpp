/****************************************************************************/
/// @file    OutputDevice_Network.cpp
/// @author  Felix Brack
/// @date    2006
/// @version $Id: $
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
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ==========================================================================
// included modules
// ==========================================================================
// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif // #ifdef WIN32

// Only build this module if socket support is enabled
#ifdef USE_SOCKETS

#include "OutputDevice_Network.h"
#include <utils/common/UtilExceptions.h>

#include <iostream>
#include <sstream>

// includes for DataReel library
#include <Gxip32.h>
#include <Gxsocket.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // #ifdef _DEBUG


// ==========================================================================
// used namespaces
// ===========================================================================
// used namespaces
// ===========================================================================
// ==========================================================================
using namespace std;


// ==========================================================================
// method definitions
// ==========================================================================
OutputDevice_Network::OutputDevice_Network(const std::string &host, const int port, const std::string &protocol)
{
    ostringstream portString;

    // initialize
    m_useUDP= true;
    portString << port;

    // initialize socket
    if (protocol=="udp") {
        m_sockUDP= new gxSocket(SOCK_DGRAM, port, (char*)(host.c_str()));
        if (m_sockUDP->GetSocketError()!=gxSOCKET_NO_ERROR) {
            throw NetworkError("Unable to create UDP socket for host '" + host + ":" + portString.str() +"'.");
        }
    } else if (protocol=="tcp-client") {
        throw NetworkError("Protocol '" + protocol + "' is not yet supported");
    } else if (protocol=="tcp-server") {
        throw NetworkError("Protocol '" + protocol + "' is not yet supported");
    } else {
        throw NetworkError("Protocol '" + protocol + "' is unknown (this parameter is case sensitive!)");
    }
}

OutputDevice_Network::~OutputDevice_Network()
{
    delete m_sockUDP;
}

bool
OutputDevice_Network::ok()
{
    return true;
}

void
OutputDevice_Network::close()
{}

bool
OutputDevice_Network::supportsStreams() const
{
    return false;
}

std::ostream &
OutputDevice_Network::getOStream()
{
    // we do not support an output stream, redirect to standard error
    return std::cerr;
}

XMLDevice &
OutputDevice_Network::writeString(const std::string &str)
{
    m_Message+= str;
    if (str.find("/>")!=string::npos) {
        Send(m_Message);
        m_Message.clear();
    }
    return *this;
}

void
OutputDevice_Network::closeInfo()
{}


bool
OutputDevice_Network::Send(std::string st)
{
    if (m_useUDP) {
        m_sockUDP->SendTo(st.c_str(), st.length());
    }

    return false;
}

#endif USE_SOCKETS // #ifdef USE_SOCKETS

/****************************************************************************/

