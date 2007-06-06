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
// Only build this module if socket support is enabled
#ifdef USE_SOCKETS



// ==========================================================================
// included modules
// ==========================================================================
// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif // #ifdef WIN32

#include "OutputDevice.h"
#include <string>

// includes from DataReel library
#include <Gxsocket.h>

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

    // @brief returns the information whether a stream is available
    // If not, one has to use the "writeString" API
    bool supportsStreams() const;

    // returns the assiciated ostream
    std::ostream &getOStream();

    // writes the given string
    OutputDevice &writeString(const std::string &str);

    void closeInfo();

private:
    // transfer a string to remote host by whatever protocol is configured
    bool Send(std::string st);

private:
    // packet buffer
    std::string m_Message;
    // if true, UDP is used to transfer data. If false, TCP is used
    bool m_useUDP;
    // the UDP socket to transfer the data
    gxSocket* m_sockUDP;
};


#endif // #ifdef USE_SOCKETS
#endif

/****************************************************************************/

