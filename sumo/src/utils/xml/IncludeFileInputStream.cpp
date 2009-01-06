/****************************************************************************/
/// @file    IncludeFileInputStream.cpp
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

/*
 * The following is an example of code which already works with Xerces
 * and should be replaced by the include element

<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE routes [
  <!ENTITY route0 SYSTEM "hello.inc.xml">
]>

<routes>
  <vtype accel="1.0" decel="5.0" id="Car" length="2.0" maxspeed="100.0"
         sigma="0" />

  &route0;

  <vehicle depart="1" id="veh0" route="route0" type="Car" />
</routes>
*/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <xercesc/util/IncludeFileInputStream.hpp>
#include <xercesc/util/PlatformUtils.hpp>


// ---------------------------------------------------------------------------
//  IncludeFileInputStream: Constructors and Destructor
// ---------------------------------------------------------------------------
IncludeFileInputStream::IncludeFileInputStream(const XMLCh* const fileName) :

        mySource(XMLPlatformUtils::openFile(fileName)),
        myBytesAvail(0),
        myBufIndex(0)
{
}


IncludeFileInputStream::~IncludeFileInputStream()
{
    if (getIsOpen())
        XMLPlatformUtils::closeFile(fSource);
}


// ---------------------------------------------------------------------------
//  IncludeFileInputStream: Implementation of the input stream interface
// ---------------------------------------------------------------------------
unsigned int
IncludeFileInputStream::curPos() const
{
    return XMLPlatformUtils::curFilePos(fSource);
}


unsigned int
IncludeFileInputStream::readBytes(XMLByte* const toFill, const unsigned int maxToRead)
{
    const unsigned int bytesLeft = myBytesAvail - myBufIndex;
    if (bytesLeft < 500) {
        for (unsigned int index = 0; index < bytesLeft; index++) {
            myByteBuf[index] = myByteBuf[myBufIndex + index];
        }
        myBufIndex = 0;
        myBytesAvail = XMLPlatformUtils::readFileBuffer(fSource, myBufSize - bytesLeft,
                       &myByteBuf[bytesLeft]) + bytesLeft;
        bytesLeft = myBytesAvail;
    }
    if (bytesLeft == 0) {
        return 0;
    }
    for (unsigned int i = 0; i < bytesLeft; i++) {
        if (&myByteBuf[myBufIndex] == "<inc") {
            std::cout << "Hello\n";
        } else {
            toFill[i] = myByteBuf[myBufIndex];
        }
        myBufIndex++;
    }
}
