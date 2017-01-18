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
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2012-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <iostream>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/common/TplConvert.h>
#include <utils/iodevices/BinaryFormatter.h>
#include <utils/iodevices/BinaryInputDevice.h>
#include "SUMOSAXAttributesImpl_Binary.h"
#include "GenericSAXHandler.h"
#include "SUMOSAXReader.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
SUMOSAXReader::SUMOSAXReader(GenericSAXHandler& handler, const XERCES_CPP_NAMESPACE::SAX2XMLReader::ValSchemes validationScheme)
    : myHandler(&handler), myValidationScheme(validationScheme),
      myXMLReader(0), myBinaryInput(0)  {}


SUMOSAXReader::~SUMOSAXReader() {
    delete myXMLReader;
    delete myBinaryInput;
}


void
SUMOSAXReader::setHandler(GenericSAXHandler& handler) {
    myHandler = &handler;
    if (myXMLReader != 0) {
        myXMLReader->setContentHandler(&handler);
        myXMLReader->setErrorHandler(&handler);
    }
}


void
SUMOSAXReader::setValidation(const XERCES_CPP_NAMESPACE::SAX2XMLReader::ValSchemes validationScheme) {
    if (myXMLReader != 0 && validationScheme != myValidationScheme) {
        if (validationScheme == XERCES_CPP_NAMESPACE::SAX2XMLReader::Val_Never) {
            myXMLReader->setEntityResolver(0);
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
        if (myXMLReader == 0) {
            myXMLReader = getSAXReader();
        }
        myXMLReader->parse(systemID.c_str());
    }
}


void
SUMOSAXReader::parseString(std::string content) {
    if (myXMLReader == 0) {
        myXMLReader = getSAXReader();
    }
    XERCES_CPP_NAMESPACE::MemBufInputSource memBufIS((const XMLByte*)content.c_str(), content.size(), "registrySettings");
    myXMLReader->parse(memBufIS);
}


bool
SUMOSAXReader::parseFirst(std::string systemID) {
    if (systemID.substr(systemID.length() - 4) == ".sbx") {
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
        if (myXMLReader == 0) {
            myXMLReader = getSAXReader();
        }
        myToken = XERCES_CPP_NAMESPACE::XMLPScanToken();
        return myXMLReader->parseFirst(systemID.c_str(), myToken);
    }
}


bool
SUMOSAXReader::parseNext() {
    if (myBinaryInput != 0) {
        int next = myBinaryInput->peek();
        switch (next) {
            case EOF:
                delete myBinaryInput;
                myBinaryInput = 0;
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
        if (myXMLReader == 0) {
            throw ProcessError("The XML-parser was not initialized.");
        }
        return myXMLReader->parseNext(myToken);
    }
}


XERCES_CPP_NAMESPACE::SAX2XMLReader*
SUMOSAXReader::getSAXReader() {
    XERCES_CPP_NAMESPACE::SAX2XMLReader* reader = XERCES_CPP_NAMESPACE::XMLReaderFactory::createXMLReader();
    if (reader == 0) {
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
    const std::string url = TplConvert::_2str(systemId);
    const std::string::size_type pos = url.rfind("/");
    if (pos != std::string::npos) {
        const std::string dir = url.substr(0, pos);
        if (dir == "http://sumo.sf.net/xsd" || dir == "http://sumo-sim.org/xsd" || dir == "http://sumo-sim.org/xsd/amitran" ||
                dir == "http://sumo.dlr.de/xsd" || dir == "http://sumo.dlr.de/xsd/amitran") {
            const char* sumoPath = std::getenv("SUMO_HOME");
            if (sumoPath == 0) {
                WRITE_WARNING("Environment variable SUMO_HOME is not set, schema resolution will use slow website lookups.");
                return 0;
            }
            const std::string file = sumoPath + std::string("/data/xsd") + url.substr(url.find("/xsd/") + 4);
            if (FileHelpers::isReadable(file)) {
                XMLCh* t = XERCES_CPP_NAMESPACE::XMLString::transcode(file.c_str());
                XERCES_CPP_NAMESPACE::InputSource* const result = new XERCES_CPP_NAMESPACE::LocalFileInputSource(t);
                XERCES_CPP_NAMESPACE::XMLString::release(&t);
                return result;
            } else {
                WRITE_WARNING("Cannot find local schema '" + file + "', will try website lookup.");
            }
        }
    }
    return 0;
}


/****************************************************************************/
