/****************************************************************************/
/// @file    OutputDevice_Network.h
/// @author  Michael Behrisch
/// @author  Daniel Krajzewicz
/// @author  Felix Brack
/// @date    2006
/// @version $Id$
///
// An output device for TCP/IP Network connections
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
#endif // #ifdef _MSC_VER

#include "foreign/tcpip/socket.h"
#include "foreign/tcpip/storage.h"
#include "OutputDevice.h"
#include <utils/common/UtilExceptions.h>
#include <string>
#include <iostream>
#include <sstream>


// ==========================================================================
// class definitions
// ==========================================================================
/**
 * @class OutputDevice_Network
 * @brief An output device for TCP/IP network connections
 *
 * The implementation uses a portable socket implementation from the Shawn
 *  project (shawn.sf.net) located in src/foreign/tcpip/socket.h. It uses
 *  an internal storage for the messages, which is sent via the socket when
 *  "postWriteHook" is called.
 * @see postWriteHook
 */
class OutputDevice_Network : public OutputDevice {
public:
    /** @brief Constructor
     *
     * @param[in] host The host to connect
     * @param[in] port The port to connect
     * @exception IOError If the connection could not be established
     */
    OutputDevice_Network(const std::string& host,
                         const int port);


    /// @brief Destructor
    ~OutputDevice_Network();


protected:
    /// @name Methods that override/implement OutputDevice-methods
    /// @{

    /** @brief Returns the associated ostream
     *
     * The stream is an ostringstream, actually, into which the message
     *  is written. It is sent when postWriteHook is called.
     *
     * @return The used stream
     * @see postWriteHook
     */
    std::ostream& getOStream();


    /** @brief Sends the data which was written to the string stream over the socket.
     *
     * Converts the stored message into a vector of chars and sends them via to
     *  the socket implementation. Resets the message, afterwards.
     */
    virtual void postWriteHook();
    /// @}

private:
    /// @brief packet buffer
    std::ostringstream myMessage;

    /// @brief the socket to transfer the data
    tcpip::Socket* mySocket;

};


#endif

/****************************************************************************/
