/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2006-2023 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    OutputDevice_Network.cpp
/// @author  Michael Behrisch
/// @author  Daniel Krajzewicz
/// @author  Felix Brack
/// @date    2006
///
// An output device for TCP/IP Network connections
/****************************************************************************/


// ==========================================================================
// included modules
// ==========================================================================
#include <config.h>

#include <thread>
#include <chrono>
#include <vector>
#include "OutputDevice_Network.h"
#include "foreign/tcpip/socket.h"
#include "utils/common/ToString.h"


// ==========================================================================
// method definitions
// ==========================================================================
OutputDevice_Network::OutputDevice_Network(const std::string& host,
        const int port) : OutputDevice(0, host + ":" + toString(port)) {
    mySocket = new tcpip::Socket(host, port);
    for (int wait = 1; wait < 10; wait += 1) {
        try {
            mySocket->connect();
            break;
        } catch (tcpip::SocketException& e) {
            if (wait == 9) {
                throw IOError(toString(e.what()) + " (host: " + host + ", port: " + toString(port) + ")");
            }
            std::this_thread::sleep_for(std::chrono::seconds(wait));
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
    const std::string toSend = myMessage.str();
    myMessage.str("");
    if (toSend.empty() || !mySocket->has_client_connection()) {
        return;
    }
    std::vector<unsigned char> msg;
    msg.insert(msg.end(), toSend.begin(), toSend.end());
    try {
        mySocket->send(msg);
    } catch (const tcpip::SocketException& e) {
        mySocket->close();
        throw IOError(e.what());
    }
}


/****************************************************************************/
