/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2006-2026 German Aerospace Center (DLR) and others.
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
/// @file    BoostSocket.h
/// @author  Michael Behrisch
/// @date    2025-10-19
///
// A TCP/IP socket implementation using boost::asio (with IPv6 support).
// API-compatible (for the subset actually used in SUMO) with tcpip::Socket.
//
// NOTE: This header pulls in <boost/asio.hpp> which is heavy (~200k lines
// preprocessed). Include it only from .cpp files (or headers that genuinely
// need the full class definition); other headers should forward-declare
// BoostSocket via the small #ifdef block they already use for TraCISocket.
/****************************************************************************/
#pragma once
#include <config.h>

#include <foreign/tcpip/socket.h>   // for tcpip::SocketException and fallback

#ifndef HAVE_BOOST_ASIO

// No Boost.Asio available: fall back to the bundled tcpip::Socket so callsites
// can use TraCISocket regardless of the build configuration.
typedef tcpip::Socket TraCISocket;

#else

#include <memory>
#include <string>
#include <vector>

#include <boost/asio.hpp>

#include <foreign/tcpip/storage.h>


/**
 * @class BoostSocket
 * @brief A TCP/IP socket using boost::asio.
 *
 * Only the surface actually used in SUMO is exposed (connect, accept,
 * sendExact, receiveExact, send, close, has_client_connection,
 * getFreeSocketPort). Errors are reported as tcpip::SocketException so
 * existing catch blocks keep working when switching backends.
 */
class BoostSocket {
public:
    /// Constructor that prepares to connect to host:port (IPv4 or IPv6).
    BoostSocket(std::string host, int port);

    /// Constructor that prepares for accepting a connection on the given port.
    BoostSocket(int port);

    /// Returns a free TCP port on the system (binds an ephemeral port and closes it).
    static int getFreeSocketPort();

    /// Connects to host:port.
    void connect();

    /// Wait for an incoming connection on the configured port and return it
    /// as a new heap-allocated BoostSocket; this instance keeps listening.
    /// The @p create parameter is kept for API parity with tcpip::Socket
    /// (the only mode SUMO uses is create=true).
    BoostSocket* accept(const bool create = true);

    /// Send raw bytes on the current client connection.
    void send(const std::vector<unsigned char>& buffer);

    /// Send a TraCI-framed message (4-byte big-endian length prefix + payload).
    void sendExact(const tcpip::Storage& msg);

    /// Receive a complete TraCI-framed message into @p msg.
    bool receiveExact(tcpip::Storage& msg);

    void close();
    bool has_client_connection() const;

private:
    std::string myHost;
    int myPort;
    boost::asio::ip::tcp::socket mySocket;
    std::unique_ptr<boost::asio::ip::tcp::acceptor> myAcceptor;

    BoostSocket(const BoostSocket&) = delete;
    BoostSocket& operator=(const BoostSocket&) = delete;
};

typedef BoostSocket TraCISocket;

#endif // HAVE_BOOST_ASIO
