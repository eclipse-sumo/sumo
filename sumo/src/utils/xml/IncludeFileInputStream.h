/****************************************************************************/
/// @file    IncludeFileInputStream.h
/// @author  Michael Behrisch
/// @date    Nov 2007
/// @version $Id$
///
// A handler which converts occuring elements and attributes into enums
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef IncludeFileInputStream_h
#define IncludeFileInputStream_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <xercesc/util/BinInputStream.hpp>
#include <xercesc/util/PlatformUtils.hpp>


// ===========================================================================
// class definitions
// ===========================================================================
class IncludeFileInputStream : public BinInputStream
{
public :
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    IncludeFileInputStream(const XMLCh* const fileName);

    virtual ~IncludeFileInputStream();


    // -----------------------------------------------------------------------
    //  Getter methods
    // -----------------------------------------------------------------------
    bool getIsOpen() const;


    // -----------------------------------------------------------------------
    //  Implementation of the input stream interface
    // -----------------------------------------------------------------------
    virtual unsigned int curPos() const;

    virtual unsigned int readBytes
    (
        XMLByte* const      toFill
        , const unsigned int        maxToRead
    );


private :
    // -----------------------------------------------------------------------
    //  Unimplemented constructors and operators
    // -----------------------------------------------------------------------
    IncludeFileInputStream(const IncludeFileInputStream&);
    IncludeFileInputStream& operator=(const IncludeFileInputStream&);

    // -----------------------------------------------------------------------
    //  Private data members
    //
    //  mySource
    //      The source file that we represent. The FileHandle type is defined
    //      per platform.
    // -----------------------------------------------------------------------
    FileHandle              mySource;
    const unsigned int      myBufSize = 1024;
    unsigned int            myBufIndex;
    XMLByte                 myByteBuf[myBufSize];
    unsigned int            myBytesAvail;
};


// ---------------------------------------------------------------------------
//  IncludeFileInputStream: Getter methods
// ---------------------------------------------------------------------------
inline bool IncludeFileInputStream::getIsOpen() const
{
    return (fSource != (FileHandle) XERCES_Invalid_File_Handle);
}

#endif
