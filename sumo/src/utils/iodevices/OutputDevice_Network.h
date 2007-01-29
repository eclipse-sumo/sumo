#ifndef OutputDevice_Network_h
#define OutputDevice_Network_h
//---------------------------------------------------------------------------
//                        OutputDevice_Network,h -
//  An output device that encapsulates a network host
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : 2006
//  copyright            : (C) 2006 LTEC AG, Felix Brack
//  organisation         : LTEC AG, http://www.ltec.ch
//  email                : fb@ltec.ch
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------

// Only build this module if socket support is enabled
#ifdef USE_SOCKETS

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


// ==========================================================================
// included modules
// ==========================================================================
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif // #ifdef WIN32
#endif // #ifdef HAVE_CONFIG_H

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
class OutputDevice_Network : public OutputDevice {
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
    XMLDevice &writeString(const std::string &str);

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

#endif // #ifdef OutputDevice_Network_h

#endif // #ifdef USE_SOCKETS
