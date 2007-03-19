/****************************************************************************/
/// @file    socket.h
/// @author  Dennis Pfisterer <pfisterer@itm.uni-luebeck.de>
/// @date    <FILE CREATION DATE>
/// @version $Id: $
///
/// socket access (client and server)
/****************************************************************************/
// This file is part of the network simulator Shawn.
// Copyright (C) 2004, 2005 by     SwarmsNet (www.swarmnet.de)
//                          and by SWARMS    (www.swarms.de)
// This part of Shawn is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License, version 2
/****************************************************************************/
#ifndef SOCKET_H
#define SOCKET_H



// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef MSVC_VER // disable certain MSVC-compiler warnings
#pragma warning(disable: 4786)
#endif



// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <map>
#include <vector>
#include <list>
#include <deque>
#include <iostream>




// ===========================================================================
// class declarations
// ===========================================================================



// ===========================================================================
// class definitions
// ===========================================================================
struct in_addr;
namespace itm {
//struct in_addr;

class SocketException {
private:
    std::string what_;
public:
    SocketException( std::string what )
            : what_(what) {
        //std::cerr << "SocketException: " << what << std::endl << std::flush;
    }
    std::string what() const {
        return what_;
    }
};

class Socket {
public:
    Socket(std::string host, int port);
    Socket(int port);
    ~Socket();

    void connect() throw( SocketException );
    void accept() throw( SocketException );
    void send( std::vector<unsigned char> ) throw( SocketException );
    bool sendExact( std::list<unsigned char> & ) throw( SocketException );
    std::vector<unsigned char> receive( int bufSize = 2048 ) throw( SocketException );
    bool receiveExact( std::list<unsigned char> &) throw( SocketException );
    void close();
    int port();
    void set_blocking(bool) throw(SocketException );
    bool is_blocking() throw();
    bool has_client_connection() const throw();

private:
    void init();
    void BailOnSocketError( std::string ) const throw( SocketException );
#ifdef WIN32
    std::string GetWinsockErrorString(int err) const;
#endif
    bool atoaddr(std::string, struct ::in_addr& addr);
    bool datawaiting(int sock) const throw();

    std::string host_;
    int port_;
    int socket_;
    int server_socket_;
    bool blocking_;
#ifdef WIN32
    static bool init_windows_sockets_;
    static bool windows_sockets_initialized_;
    static int instance_count_;
#endif
};
}

#endif

