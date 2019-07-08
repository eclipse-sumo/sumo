/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2002-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    IStreamInputSource.h
/// @author  Michael Behrisch
/// @author  Gilles Filippini
/// @date    Sept 2002
/// @version $Id$
///
// Xerces InputSource reading from arbitrary std::istream
// reimplementation inspired by https://marc.info/?l=xerces-dev&m=86952133511623
/****************************************************************************/
#ifndef IStreamInputSource_h
#define IStreamInputSource_h


// ===========================================================================
// included modules
// ===========================================================================
#include <iostream>
#include <xercesc/util/BinInputStream.hpp>
#include <xercesc/sax/InputSource.hpp>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class IStreamBinInputStream
 * @brief Xerces BinInputStream reading from arbitrary std::istream
 */
class IStreamBinInputStream : public XERCES_CPP_NAMESPACE::BinInputStream {
public:
    IStreamBinInputStream(std::istream& in) : myIn(in) { }
    virtual ~IStreamBinInputStream(void) { }
    virtual XMLFilePos curPos(void) const {
        return myIn.tellg();
    }
    virtual XMLSize_t readBytes(XMLByte* const buf, const XMLSize_t max) {
        myIn.read((char*)buf, max);
        return (XMLSize_t)myIn.gcount();
    }
    virtual const XMLCh* getContentType() const {
        return nullptr;
    }
private:
    std::istream& myIn;
};


/**
 * @class IStreamInputSource
 * @brief Xerces InputSource reading from arbitrary std::istream
 */
class IStreamInputSource : public XERCES_CPP_NAMESPACE::InputSource {
public:
    IStreamInputSource(std::istream& in) :
        XERCES_CPP_NAMESPACE::InputSource("istream"), myIn(in) { }
    virtual ~IStreamInputSource(void) { }
    virtual XERCES_CPP_NAMESPACE::BinInputStream* makeStream(void) const {
        return new IStreamBinInputStream(myIn);
    }
private:
    std::istream& myIn;
};


#endif


/****************************************************************************/
