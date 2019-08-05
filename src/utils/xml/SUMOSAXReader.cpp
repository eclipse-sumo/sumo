/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    SUMOSAXReader.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Nov 2012
/// @version $Id$
///
// SAX-reader encapsulation containing binary reader
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <string>
#include <memory>
#include <iostream>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/common/StringUtils.h>
#include <utils/iodevices/BinaryFormatter.h>
#include <utils/iodevices/BinaryInputDevice.h>
#include "SUMOSAXAttributesImpl_Binary.h"
#include "GenericSAXHandler.h"
#ifdef HAVE_ZLIB
#include <foreign/zstr/zstr.hpp>
#endif
#include "IStreamInputSource.h"
#include "SUMOSAXReader.h"


// ===========================================================================
// method definitions
// ===========================================================================
SUMOSAXReader::SUMOSAXReader(GenericSAXHandler& handler, const XERCES_CPP_NAMESPACE::SAX2XMLReader::ValSchemes validationScheme)
    : myHandler(nullptr), myValidationScheme(validationScheme), myXMLReader(nullptr), myBinaryInput(nullptr), myIStream(nullptr), myInputStream(nullptr) {
    setHandler(handler);
}


SUMOSAXReader::~SUMOSAXReader() {
    delete myXMLReader;
    delete myBinaryInput;
}


void
SUMOSAXReader::setHandler(GenericSAXHandler& handler) {
    myHandler = &handler;
    mySchemaResolver.setHandler(handler);
    if (myXMLReader != nullptr) {
        myXMLReader->setContentHandler(&handler);
        myXMLReader->setErrorHandler(&handler);
    }
}


void
SUMOSAXReader::setValidation(const XERCES_CPP_NAMESPACE::SAX2XMLReader::ValSchemes validationScheme) {
    if (myXMLReader != nullptr && validationScheme != myValidationScheme) {
        if (validationScheme == XERCES_CPP_NAMESPACE::SAX2XMLReader::Val_Never) {
            myXMLReader->setEntityResolver(nullptr);
            myXMLReader->setProperty(XERCES_CPP_NAMESPACE::XMLUni::fgXercesScannerName, (void*)XERCES_CPP_NAMESPACE::XMLUni::fgWFXMLScanner);
        } else {
            myXMLReader->setEntityResolver(&mySchemaResolver);
            myXMLReader->setProperty(XERCES_CPP_NAMESPACE::XMLUni::fgXercesScannerName, (void*)XERCES_CPP_NAMESPACE::XMLUni::fgIGXMLScanner);
            myXMLReader->setFeature(XERCES_CPP_NAMESPACE::XMLUni::fgXercesSchema, true);
            myXMLReader->setFeature(XERCES_CPP_NAMESPACE::XMLUni::fgSAX2CoreValidation, true);
            myXMLReader->setFeature(XERCES_CPP_NAMESPACE::XMLUni::fgXercesDynamic, validationScheme == XERCES_CPP_NAMESPACE::SAX2XMLReader::Val_Auto);
        }
    }
    myValidationScheme = validationScheme;
}


void
SUMOSAXReader::parse(std::string systemID) {
    if (systemID.length() >= 4 && systemID.substr(systemID.length() - 4) == ".sbx") {
        if (parseFirst(systemID)) {
            while (parseNext());
        }
    } else {
        if (myXMLReader == nullptr) {
            myXMLReader = getSAXReader();
        }
        if (!FileHelpers::isReadable(systemID)) {
            throw ProcessError("Cannot read file '" + systemID + "'!");
        }
#ifdef HAVE_ZLIB
        zstr::ifstream istream(systemID.c_str(), std::fstream::in | std::fstream::binary);
        myXMLReader->parse(IStreamInputSource(istream));
#else
        myXMLReader->parse(systemID.c_str());
#endif
    }
}


void
SUMOSAXReader::parseString(std::string content) {
    if (myXMLReader == nullptr) {
        myXMLReader = getSAXReader();
    }
    XERCES_CPP_NAMESPACE::MemBufInputSource memBufIS((const XMLByte*)content.c_str(), content.size(), "registrySettings");
    myXMLReader->parse(memBufIS);
}


bool
SUMOSAXReader::parseFirst(std::string systemID) {
    if (systemID.length() >= 4 && systemID.substr(systemID.length() - 4) == ".sbx") {
        myBinaryInput = new BinaryInputDevice(systemID, true, myValidationScheme == XERCES_CPP_NAMESPACE::SAX2XMLReader::Val_Always);
        *myBinaryInput >> mySbxVersion;
        if (mySbxVersion < 1 || mySbxVersion > 2) {
            throw ProcessError("Unknown sbx version");
        }
        std::string sumoVer;
        *myBinaryInput >> sumoVer;
        std::vector<std::string> elems;
        *myBinaryInput >> elems;
        // !!! check elems here
        elems.clear();
        *myBinaryInput >> elems;
        // !!! check attrs here
        elems.clear();
        *myBinaryInput >> elems;
        // !!! check node types here
        elems.clear();
        *myBinaryInput >> elems;
        // !!! check edge types here
        elems.clear();
        *myBinaryInput >> elems;
        // !!! check edges here
        std::vector< std::vector<int> > followers;
        *myBinaryInput >> followers;
        // !!! check followers here
        return parseNext();
    } else {
        if (!FileHelpers::isReadable(systemID)) {
            throw ProcessError("Cannot read file '" + systemID + "'!");
        }
        if (myXMLReader == nullptr) {
            myXMLReader = getSAXReader();
        }
        myToken = XERCES_CPP_NAMESPACE::XMLPScanToken();
#ifdef HAVE_ZLIB
        myIStream = std::unique_ptr<zstr::ifstream>(new zstr::ifstream(systemID.c_str(), std::fstream::in | std::fstream::binary));
        myInputStream = std::unique_ptr<IStreamInputSource>(new IStreamInputSource(*myIStream));
        return myXMLReader->parseFirst(*myInputStream, myToken);
#else
        return myXMLReader->parseFirst(systemID.c_str(), myToken);
#endif
    }
}


bool
SUMOSAXReader::parseNext() {
    if (myBinaryInput != nullptr) {
        int next = myBinaryInput->peek();
        switch (next) {
            case EOF:
                delete myBinaryInput;
                myBinaryInput = nullptr;
                return false;
            case BinaryFormatter::BF_XML_TAG_START: {
                int tag;
                unsigned char tagByte;
                *myBinaryInput >> tagByte;
                tag = tagByte;
                if (mySbxVersion > 1) {
                    myBinaryInput->putback(BinaryFormatter::BF_BYTE);
                    *myBinaryInput >> tagByte;
                    tag += 256 * tagByte;
                }
                myXMLStack.push_back((SumoXMLTag)tag);
                SUMOSAXAttributesImpl_Binary attrs(myHandler->myPredefinedTagsMML, toString((SumoXMLTag)tag), myBinaryInput, mySbxVersion);
                myHandler->myStartElement(tag, attrs);
                break;
            }
            case BinaryFormatter::BF_XML_TAG_END: {
                if (myXMLStack.empty()) {
                    throw ProcessError("Binary file is invalid, unexpected tag end.");
                }
                myHandler->myEndElement(myXMLStack.back());
                myXMLStack.pop_back();
                myBinaryInput->read(mySbxVersion > 1 ? 1 : 2);
                break;
            }
            default:
                throw ProcessError("Binary file is invalid, expected tag start or tag end.");
        }
        return true;
    } else {
        if (myXMLReader == nullptr) {
            throw ProcessError("The XML-parser was not initialized.");
        }
        return myXMLReader->parseNext(myToken);
    }
}


XERCES_CPP_NAMESPACE::SAX2XMLReader*
SUMOSAXReader::getSAXReader() {
    XERCES_CPP_NAMESPACE::SAX2XMLReader* reader = XERCES_CPP_NAMESPACE::XMLReaderFactory::createXMLReader();
    if (reader == nullptr) {
        throw ProcessError("The XML-parser could not be build.");
    }
    // see here https://svn.apache.org/repos/asf/xerces/c/trunk/samples/src/SAX2Count/SAX2Count.cpp for the way to set features
    if (myValidationScheme == XERCES_CPP_NAMESPACE::SAX2XMLReader::Val_Never) {
        reader->setProperty(XERCES_CPP_NAMESPACE::XMLUni::fgXercesScannerName, (void*)XERCES_CPP_NAMESPACE::XMLUni::fgWFXMLScanner);
    } else {
        reader->setEntityResolver(&mySchemaResolver);
        reader->setFeature(XERCES_CPP_NAMESPACE::XMLUni::fgXercesSchema, true);
        reader->setFeature(XERCES_CPP_NAMESPACE::XMLUni::fgSAX2CoreValidation, true);
        reader->setFeature(XERCES_CPP_NAMESPACE::XMLUni::fgXercesDynamic, myValidationScheme == XERCES_CPP_NAMESPACE::SAX2XMLReader::Val_Auto);
    }
    reader->setContentHandler(myHandler);
    reader->setErrorHandler(myHandler);
    return reader;
}


XERCES_CPP_NAMESPACE::InputSource*
SUMOSAXReader::LocalSchemaResolver::resolveEntity(const XMLCh* const /* publicId */, const XMLCh* const systemId) {
    const std::string url = StringUtils::transcode(systemId);
    const std::string::size_type pos = url.find("/xsd/");
    if (pos != std::string::npos) {
        myHandler->setSchemaSeen();
        const char* sumoPath = std::getenv("SUMO_HOME");
        if (sumoPath == nullptr) {
            WRITE_WARNING("Environment variable SUMO_HOME is not set, schema resolution will use slow website lookups.");
            return nullptr;
        }
        const std::string file = sumoPath + std::string("/data") + url.substr(pos);
        if (FileHelpers::isReadable(file)) {
            XMLCh* t = XERCES_CPP_NAMESPACE::XMLString::transcode(file.c_str());
            XERCES_CPP_NAMESPACE::InputSource* const result = new XERCES_CPP_NAMESPACE::LocalFileInputSource(t);
            XERCES_CPP_NAMESPACE::XMLString::release(&t);
            return result;
        } else {
            WRITE_WARNING("Cannot read local schema '" + file + "', will try website lookup.");
        }
    }
    return nullptr;
}


void
SUMOSAXReader::LocalSchemaResolver::setHandler(GenericSAXHandler& handler) {
    myHandler = &handler;
}


/****************************************************************************/
