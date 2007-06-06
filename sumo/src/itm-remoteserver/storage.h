/****************************************************************************/
/// @file    storage.h
/// @author  Axel Wegener <wegener@itm.uni-luebeck.de>
/// @date    2006/09/07
/// @version $Id$
///
/// methods for sending and receiving data over socket connections
/****************************************************************************/
// This file is part of the network simulator ns-2.
// Copyright (C) 2006 by AutoNomos (www.auto-nomos.de)
// This part of ns-2 is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License, version 2
//
// Derived from Shawn (see www.swarmnet.de)
//
// **************************************************************************/
#ifndef storage_h
#define storage_h

// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef MSVC_VER // disable certain MSVC-compiler warnings
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <list>
#include <string>



// ===========================================================================
// class declarations
// ===========================================================================



// ===========================================================================
// class definitions
// ===========================================================================
namespace itm {
class Storage: public std::list<unsigned char> {
private:
    unsigned int pos_;
    std::list<unsigned char>::iterator iter_;
    std::list<unsigned char>::iterator iterEnd_;


public:
    Storage();
    Storage(unsigned char[], int length=-1);
    virtual ~Storage();

    void advance(int);
    bool valid_pos();

    void reset();

    virtual unsigned char readChar() throw();
    virtual void writeChar(unsigned char) throw();

    int readByte() throw();
    virtual void writeByte(int) throw();
    virtual void writeByte(unsigned char) throw();

    virtual std::string readString() throw();
    virtual void writeString(std::string s) throw();

    virtual int readShort() throw();
    virtual void writeShort(int) throw();

    virtual int readInt() throw();
    virtual void writeInt(int) throw();

    virtual float readFloat() throw();
    virtual void writeFloat( float ) throw();

    virtual double readDouble() throw();
    virtual void writeDouble( double ) throw();
};
}
#endif
