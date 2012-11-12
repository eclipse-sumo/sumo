/****************************************************************************/
/// @file    SUMOSAXReader.cpp
/// @author  Michael Behrisch
/// @date    Nov 2012
/// @version $Id$
///
// SAX-reader encapsulation containing binary reader
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
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
#include <xercesc/framework/MemBufInputSource.hpp>
#include <utils/common/ToString.h>
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
SUMOSAXReader::SUMOSAXReader(GenericSAXHandler& handler, const bool enableValidation)
    : myHandler(&handler), myEnableValidation(enableValidation),
      myToken(0), myXMLReader(0), myBinaryInput(0)  {}


SUMOSAXReader::~SUMOSAXReader() {
    delete myToken;
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
SUMOSAXReader::parse(std::string systemID) {
    if (systemID.substr(systemID.length()-4) == ".sbx") {
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
    MemBufInputSource memBufIS((const XMLByte*)content.c_str(), content.size(), "registrySettings");
    myXMLReader->parse(memBufIS);
}


bool 
SUMOSAXReader::parseFirst(std::string systemID) {
    if (systemID.substr(systemID.length()-4) == ".sbx") {
        myBinaryInput = new BinaryInputDevice(systemID, true, myEnableValidation);
        char sbxVer;
        *myBinaryInput >> sbxVer;
        if (sbxVer != 1) {
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
        std::vector< std::vector<unsigned int> > followers;
        *myBinaryInput >> followers;
        // !!! check followers here
        return parseNext();
    } else {
        if (myXMLReader == 0) {
            myXMLReader = getSAXReader();
        }
        delete myToken;
        myToken = new XERCES_CPP_NAMESPACE_QUALIFIER XMLPScanToken();
        return myXMLReader->parseFirst(systemID.c_str(), *myToken);
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
            int t;
            *myBinaryInput >> t;
            SUMOSAXAttributesImpl_Binary attrs(myHandler->myPredefinedTagsMML, toString((SumoXMLTag)t), myBinaryInput);
            myHandler->myStartElement(t, attrs);
            break;
                                                }
        case BinaryFormatter::BF_XML_TAG_END: {
            int t;
            *myBinaryInput >> t;
            myHandler->myEndElement(t);
            break;
                                              }
        default:
            throw ProcessError("Invalid binary file");
        }
        return true;
    } else {
        if (myXMLReader == 0) {
            throw ProcessError("The XML-parser was not initialized.");
        }
        return myXMLReader->parseNext(*myToken);
    }
}


XERCES_CPP_NAMESPACE_QUALIFIER SAX2XMLReader*
SUMOSAXReader::getSAXReader() {
    SAX2XMLReader* reader = XMLReaderFactory::createXMLReader();
    if (reader == 0) {
        throw ProcessError("The XML-parser could not be build.");
    }
    if (!myEnableValidation) {
        reader->setProperty(XMLUni::fgXercesScannerName, (void*)XMLUni::fgWFXMLScanner);
    }
    setFeature(*reader, "http://xml.org/sax/features/namespaces", false);
    setFeature(*reader, "http://apache.org/xml/features/validation/schema", myEnableValidation);
    setFeature(*reader, "http://apache.org/xml/features/validation/schema-full-checking", myEnableValidation);
    setFeature(*reader, "http://xml.org/sax/features/validation", myEnableValidation);
    setFeature(*reader, "http://apache.org/xml/features/validation/dynamic", myEnableValidation);
    reader->setContentHandler(myHandler);
    reader->setErrorHandler(myHandler);
    return reader;
}


void
SUMOSAXReader::setFeature(XERCES_CPP_NAMESPACE_QUALIFIER SAX2XMLReader& reader,
                          const std::string& feature, bool value) {
    XMLCh* xmlFeature = XMLString::transcode(feature.c_str());
    reader.setFeature(xmlFeature, value);
    XMLString::release(&xmlFeature);
}


/****************************************************************************/
