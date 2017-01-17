/****************************************************************************/
/// @file    OutputDevice_Network.cpp
/// @author  Michael Behrisch
/// @author  Daniel Krajzewicz
/// @author  Felix Brack
/// @date    2006
/// @version $Id$
///
// An output device for TCP/IP Network connections
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2006-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
#endif // #ifdef _MSC_VER

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <vector>
#include "OutputDevice_Network.h"
#include "foreign/tcpip/socket.h"
#include "utils/common/ToString.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // #ifdef CHECK_MEMORY_LEAKS


// ==========================================================================
// method definitions
// ==========================================================================
OutputDevice_Network::OutputDevice_Network(const std::string& host,
        const int port) {
    mySocket = new tcpip::Socket(host, port);
    for (int wait = 1000; true; wait += 1000) {
        try {
            mySocket->connect();
            break;
        } catch (tcpip::SocketException& e) {
            if (wait == 9000) {
                throw IOError(toString(e.what()) + " (host: " + host + ", port: " + toString(port) + ")");
            }
#ifdef WIN32
            Sleep(wait);
#else
            usleep(wait * 1000);
#endif
        }
    }
}


OutputDevice_Network::~OutputDevice_Network() {
    mySocket->close();
    delete mySocket;
}


std::ostream&
OutputDevice_Network::getOStream() {
    return myMessage;
}


void
OutputDevice_Network::postWriteHook() {
    std::string toSend = myMessage.str();
    std::vector<unsigned char> msg;
    msg.insert(msg.end(), toSend.begin(), toSend.end());
    try {
        mySocket->send(msg);
    } catch (tcpip::SocketException& e) {
        throw IOError(toString(e.what()));
    }
    myMessage.str("");
}


/****************************************************************************/
