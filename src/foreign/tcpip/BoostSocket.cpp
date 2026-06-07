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
/// @file    BoostSocket.cpp
/// @author  Michael Behrisch
/// @date    2025-10-19
///
// Implementation of BoostSocket.
/****************************************************************************/
#include <config.h>

#ifdef HAVE_BOOST_ASIO

#include "BoostSocket.h"

#include <string>

#include <boost/system/error_code.hpp>

namespace asio = boost::asio;
using asio::ip::tcp;


// Length in bytes of the TraCI message length prefix (4-byte big-endian int).
static const int LENGTH_PREFIX_BYTES = 4;


// A single io_context shared by all BoostSocket instances. We only use
// synchronous operations (no run() loop), so the io_context is used purely as
// a service registry. Sharing it means accepted client sockets can outlive the
// listening BoostSocket without dangling references (the listener creates
// child BoostSocket instances that bind to this same context).
static asio::io_context& sharedContext() {
    static asio::io_context ctx;
    return ctx;
}


// ---------------------------------------------------------------------------
// construction
// ---------------------------------------------------------------------------
BoostSocket::BoostSocket(std::string host, int port)
    : myHost(std::move(host)), myPort(port), mySocket(sharedContext()) {
}


BoostSocket::BoostSocket(int port)
    : myHost(), myPort(port), mySocket(sharedContext()) {
}


// ---------------------------------------------------------------------------
// static helpers
// ---------------------------------------------------------------------------
int
BoostSocket::getFreeSocketPort() {
    // Match the server bind strategy: prefer IPv6 dual-stack, fall back to IPv4.
    boost::system::error_code ec;
    tcp::acceptor acceptor(sharedContext());
    acceptor.open(tcp::v6(), ec);
    if (!ec) {
        // dual-stack so the returned port is usable for both v4 and v6
        acceptor.set_option(asio::ip::v6_only(false), ec);  // ignore failure
        acceptor.bind(tcp::endpoint(tcp::v6(), 0), ec);
    }
    if (ec || !acceptor.is_open()) {
        ec.clear();
        acceptor.close(ec);
        acceptor.open(tcp::v4(), ec);
        if (ec) {
            throw tcpip::SocketException(std::string("BoostSocket::getFreeSocketPort open: ") + ec.message());
        }
        acceptor.bind(tcp::endpoint(tcp::v4(), 0), ec);
        if (ec) {
            throw tcpip::SocketException(std::string("BoostSocket::getFreeSocketPort bind: ") + ec.message());
        }
    }
    const int p = acceptor.local_endpoint(ec).port();
    if (ec) {
        throw tcpip::SocketException(std::string("BoostSocket::getFreeSocketPort local_endpoint: ") + ec.message());
    }
    acceptor.close(ec);
    return p;
}


bool
BoostSocket::has_client_connection() const {
    return mySocket.is_open();
}


// ---------------------------------------------------------------------------
// connect / accept / close
// ---------------------------------------------------------------------------
void
BoostSocket::connect() {
    tcp::resolver resolver(sharedContext());
    boost::system::error_code ec;
    // resolve allows both IPv4 and IPv6 endpoints
    auto endpoints = resolver.resolve(myHost, std::to_string(myPort), ec);
    if (ec) {
        throw tcpip::SocketException(std::string("BoostSocket::connect resolve: ") + ec.message());
    }
    asio::connect(mySocket, endpoints, ec);
    if (ec) {
        throw tcpip::SocketException(std::string("BoostSocket::connect: ") + ec.message());
    }
    // Match tcpip::Socket: disable Nagle for low-latency TraCI traffic.
    mySocket.set_option(tcp::no_delay(true), ec);
}


BoostSocket*
BoostSocket::accept(const bool /* create, always true in SUMO */) {
    if (mySocket.is_open()) {
        return nullptr;
    }
    boost::system::error_code ec;
    if (!myAcceptor) {
        // Create the listening acceptor; try IPv6 dual-stack first,
        // fall back to IPv4 if IPv6 is unavailable on this host.
        myAcceptor.reset(new tcp::acceptor(sharedContext()));
        myAcceptor->open(tcp::v6(), ec);
        if (!ec) {
            myAcceptor->set_option(asio::socket_base::reuse_address(true), ec);
            ec.clear();
            myAcceptor->set_option(asio::ip::v6_only(false), ec); // ignore
            ec.clear();
            tcp::endpoint ep(tcp::v6(), static_cast<unsigned short>(myPort));
            myAcceptor->bind(ep, ec);
        }
        if (ec || !myAcceptor->is_open()) {
            // fall back to IPv4
            ec.clear();
            myAcceptor->close(ec);
            myAcceptor->open(tcp::v4(), ec);
            if (ec) {
                throw tcpip::SocketException(std::string("BoostSocket::accept open: ") + ec.message());
            }
            myAcceptor->set_option(asio::socket_base::reuse_address(true), ec);
            ec.clear();
            tcp::endpoint ep(tcp::v4(), static_cast<unsigned short>(myPort));
            myAcceptor->bind(ep, ec);
            if (ec) {
                throw tcpip::SocketException(std::string("BoostSocket::accept bind: ") + ec.message());
            }
        }
        myAcceptor->listen(asio::socket_base::max_listen_connections, ec);
        if (ec) {
            throw tcpip::SocketException(std::string("BoostSocket::accept listen: ") + ec.message());
        }
    }

    // Accept directly into a new child BoostSocket so we can keep
    // listening on this instance without handle transfer.
    std::unique_ptr<BoostSocket> result(new BoostSocket(0));
    myAcceptor->accept(result->mySocket, ec);
    if (ec) {
        throw tcpip::SocketException(std::string("BoostSocket::accept accept: ") + ec.message());
    }
    result->mySocket.set_option(tcp::no_delay(true), ec);
    return result.release();
}


void
BoostSocket::close() {
    if (mySocket.is_open()) {
        boost::system::error_code ec;
        mySocket.shutdown(tcp::socket::shutdown_both, ec);  // ignore errors
        mySocket.close(ec);
    }
}


// ---------------------------------------------------------------------------
// I/O
// ---------------------------------------------------------------------------
void
BoostSocket::send(const std::vector<unsigned char>& buffer) {
    if (!mySocket.is_open()) {
        return;
    }
    boost::system::error_code ec;
    asio::write(mySocket, asio::buffer(buffer), ec);
    if (ec) {
        throw tcpip::SocketException(std::string("BoostSocket::send: ") + ec.message());
    }
}


void
BoostSocket::sendExact(const tcpip::Storage& b) {
    const int length = static_cast<int>(b.size());
    // length prefix is encoded via Storage to share the wire-format definition
    // (4 bytes, big-endian, includes the prefix itself).
    tcpip::Storage lengthStorage;
    lengthStorage.writeInt(LENGTH_PREFIX_BYTES + length);

    std::vector<unsigned char> msg;
    msg.reserve(LENGTH_PREFIX_BYTES + length);
    msg.insert(msg.end(), lengthStorage.begin(), lengthStorage.end());
    msg.insert(msg.end(), b.begin(), b.end());
    send(msg);
}


bool
BoostSocket::receiveExact(tcpip::Storage& msg) {
    if (!mySocket.is_open()) {
        throw tcpip::SocketException("BoostSocket::receiveExact: not connected");
    }
    std::vector<unsigned char> buffer(LENGTH_PREFIX_BYTES);
    boost::system::error_code ec;
    // Read the length prefix.
    asio::read(mySocket, asio::buffer(&buffer[0], LENGTH_PREFIX_BYTES), ec);
    if (ec) {
        if (ec == asio::error::eof) {
            throw tcpip::SocketException("BoostSocket::receiveExact: peer shutdown");
        }
        throw tcpip::SocketException(std::string("BoostSocket::receiveExact length: ") + ec.message());
    }
    tcpip::Storage lengthStorage(&buffer[0], LENGTH_PREFIX_BYTES);
    const int totalLen = lengthStorage.readInt();
    if (totalLen <= LENGTH_PREFIX_BYTES) {
        throw tcpip::SocketException("BoostSocket::receiveExact: invalid message length "
                                     + std::to_string(totalLen));
    }

    buffer.resize(totalLen);
    asio::read(mySocket, asio::buffer(&buffer[LENGTH_PREFIX_BYTES],
                                      totalLen - LENGTH_PREFIX_BYTES), ec);
    if (ec) {
        if (ec == asio::error::eof) {
            throw tcpip::SocketException("BoostSocket::receiveExact: peer shutdown");
        }
        throw tcpip::SocketException(std::string("BoostSocket::receiveExact body: ") + ec.message());
    }

    msg.reset();
    msg.writePacket(&buffer[LENGTH_PREFIX_BYTES], totalLen - LENGTH_PREFIX_BYTES);
    return true;
}


#endif // HAVE_BOOST_ASIO
