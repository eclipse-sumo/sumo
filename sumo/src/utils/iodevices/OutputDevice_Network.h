/****************************************************************************/
/// @file    OutputDevice_Network.h
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
#ifndef OutputDevice_Network_h
#define OutputDevice_Network_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif // #ifdef WIN32

#include "foreign/tcpip/socket.h"
#include "foreign/tcpip/storage.h"
#include "OutputDevice.h"
#include <string>
#include <iostream>
#include <sstream>

// ==========================================================================
// class definitions
// ==========================================================================
//
// @class OutputDevice_Network
// An output device that encapsulates a network host
//
class OutputDevice_Network : public OutputDevice
{
public:
    // constructor
    OutputDevice_Network(const std::string &host, const int port, const std::string &protocol);

    // destructor
    ~OutputDevice_Network();

    // returns the information whether one can write into the device
    bool ok();

    // closes the device
    void close();

    // returns the associated ostream
    std::ostream &getOStream();

private:
    // packet buffer
    std::ostringstream myMessage;
    // the  socket to transfer the data
    tcpip::Socket* mySocket;

public:
    template <class T>
    OutputDevice &operator<<(const T &t)
    {
        getOStream() << t;
        mySocket->sendExact(tcpip::Storage(myMessage.str().c_str()));
        myMessage.rdbuf()->pubseekpos(0);
        return *this;
    }

};


#endif

/****************************************************************************/

