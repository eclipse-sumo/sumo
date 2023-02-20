/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2023 German Aerospace Center (DLR) and others.
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
/// @file    SUMOSAXReader.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Nov 2012
///
// SAX-reader encapsulation
/****************************************************************************/
#include <config.h>

#include <string>
#include <memory>
#include <iostream>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

#include <utils/common/FileHelpers.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/common/StringUtils.h>
#include "GenericSAXHandler.h"
#ifdef HAVE_ZLIB
#include <foreign/zstr/zstr.hpp>
#endif
#include "IStreamInputSource.h"
#include "SUMOSAXReader.h"

using XERCES_CPP_NAMESPACE::SAX2XMLReader;
using XERCES_CPP_NAMESPACE::XMLUni;


// ===========================================================================
// method definitions
// ===========================================================================

SUMOSAXReader::SUMOSAXReader(GenericSAXHandler& handler, const std::string& validationScheme, XERCES_CPP_NAMESPACE::XMLGrammarPool* grammarPool) :
    myHandler(nullptr),
    myValidationScheme(validationScheme),
    myGrammarPool(grammarPool),
    myXMLReader(nullptr),
    myIStream(nullptr),
    myInputStream(nullptr),
    mySchemaResolver(true, false),
    myLocalResolver(false, false),
    myNoOpResolver(false, true),
    myNextSection(-1, nullptr) {
    setHandler(handler);
}


SUMOSAXReader::~SUMOSAXReader() {
    delete myXMLReader;
    delete myNextSection.second;
}


void
SUMOSAXReader::setHandler(GenericSAXHandler& handler) {
    myHandler = &handler;
    if (myXMLReader != nullptr) {
        myXMLReader->setContentHandler(&handler);
        myXMLReader->setErrorHandler(&handler);
    }
}


void
SUMOSAXReader::setValidation(std::string validationScheme) {
    // The settings ensure that by default (validationScheme "local" or "never") no network access occurs
    // this is achieved by either resolving no entities at all (myNoOpResolver) or resolving only
    // to local files (myLocalResolver). Thus we can safely disable the Sonar warnings in the parse methods below.
    if (myXMLReader != nullptr && validationScheme != myValidationScheme) {
        if (validationScheme == "") {
            validationScheme = myValidationScheme;
        }
        // see here https://svn.apache.org/repos/asf/xerces/c/trunk/samples/src/SAX2Count/SAX2Count.cpp for the way to set features
        if (validationScheme == "never") {
            myXMLReader->setEntityResolver(&myNoOpResolver);
            myXMLReader->setProperty(XMLUni::fgXercesScannerName, (void*)XMLUni::fgWFXMLScanner);
        } else {
            myXMLReader->setEntityResolver(validationScheme == "local" ? &myLocalResolver : &mySchemaResolver);
            myXMLReader->setProperty(XMLUni::fgXercesScannerName, (void*)XMLUni::fgIGXMLScanner);
            myXMLReader->setFeature(XMLUni::fgXercesSchema, true);
            myXMLReader->setFeature(XMLUni::fgSAX2CoreValidation, true);
            myXMLReader->setFeature(XMLUni::fgXercesDynamic, validationScheme == "local" || validationScheme == "auto");
            myXMLReader->setFeature(XMLUni::fgXercesUseCachedGrammarInParse, myValidationScheme == "always");
        }
    }
    myValidationScheme = validationScheme;
}


void
SUMOSAXReader::parse(std::string systemID) {
    if (!FileHelpers::isReadable(systemID)) {
        throw IOError(TLF("Cannot read file '%'!", systemID));
    }
    if (FileHelpers::isDirectory(systemID)) {
        throw IOError(TLF("File '%' is a directory!", systemID));
    }
    ensureSAXReader();
#ifdef HAVE_ZLIB
    zstr::ifstream istream(StringUtils::transcodeToLocal(systemID).c_str(), std::fstream::in | std::fstream::binary);
    myXMLReader->parse(IStreamInputSource(istream));  // NOSONAR
#else
    myXMLReader->parse(StringUtils::transcodeToLocal(systemID).c_str());  // NOSONAR
#endif
}


void
SUMOSAXReader::parseString(std::string content) {
    ensureSAXReader();
    XERCES_CPP_NAMESPACE::MemBufInputSource memBufIS((const XMLByte*)content.c_str(), content.size(), "registrySettings");
    myXMLReader->parse(memBufIS);  // NOSONAR
}


bool
SUMOSAXReader::parseFirst(std::string systemID) {
    if (!FileHelpers::isReadable(systemID)) {
        throw IOError(TLF("Cannot read file '%'!", systemID));
    }
    if (FileHelpers::isDirectory(systemID)) {
        throw IOError(TLF("File '%' is a directory!", systemID));
    }
    ensureSAXReader();
    myToken = XERCES_CPP_NAMESPACE::XMLPScanToken();
#ifdef HAVE_ZLIB
    myIStream = std::unique_ptr<zstr::ifstream>(new zstr::ifstream(StringUtils::transcodeToLocal(systemID).c_str(), std::fstream::in | std::fstream::binary));
    myInputStream = std::unique_ptr<IStreamInputSource>(new IStreamInputSource(*myIStream));
    return myXMLReader->parseFirst(*myInputStream, myToken);  // NOSONAR
#else
    return myXMLReader->parseFirst(StringUtils::transcodeToLocal(systemID).c_str(), myToken);  // NOSONAR
#endif
}


bool
SUMOSAXReader::parseNext() {
    if (myXMLReader == nullptr) {
        throw ProcessError(TL("The XML-parser was not initialized."));
    }
    return myXMLReader->parseNext(myToken);
}


bool
SUMOSAXReader::parseSection(int element) {
    if (myXMLReader == nullptr) {
        throw ProcessError(TL("The XML-parser was not initialized."));
    }
    bool started = false;
    if (myNextSection.first != -1) {
        started = myNextSection.first == element;
        myHandler->myStartElement(myNextSection.first, *myNextSection.second);
        delete myNextSection.second;
        myNextSection.first = -1;
        myNextSection.second = nullptr;
    }
    myHandler->setSection(element, started);
    while (!myHandler->sectionFinished()) {
        if (!myXMLReader->parseNext(myToken)) {
            return false;
        }
    }
    myNextSection = myHandler->retrieveNextSectionStart();
    return true;
}


void
SUMOSAXReader::ensureSAXReader() {
    if (myXMLReader == nullptr) {
        myXMLReader = XERCES_CPP_NAMESPACE::XMLReaderFactory::createXMLReader(XERCES_CPP_NAMESPACE::XMLPlatformUtils::fgMemoryManager, myGrammarPool);
        if (myXMLReader == nullptr) {
            throw ProcessError(TL("The XML-parser could not be build."));
        }
        setValidation();
        myXMLReader->setContentHandler(myHandler);
        myXMLReader->setErrorHandler(myHandler);
    }
}


SUMOSAXReader::LocalSchemaResolver::LocalSchemaResolver(const bool haveFallback, const bool noOp) :
    myHaveFallback(haveFallback),
    myNoOp(noOp) {
}


XERCES_CPP_NAMESPACE::InputSource*
SUMOSAXReader::LocalSchemaResolver::resolveEntity(const XMLCh* const /* publicId */, const XMLCh* const systemId) {
    if (myNoOp) {
        return new XERCES_CPP_NAMESPACE::MemBufInputSource((const XMLByte*)"", 0, "");
    }
    const std::string url = StringUtils::transcode(systemId);
    const std::string::size_type pos = url.find("/xsd/");
    if (pos != std::string::npos) {
        const char* sumoPath = std::getenv("SUMO_HOME");
        // no need for a warning if SUMO_HOME is not set, global preparsing should have done it.
        if (sumoPath != nullptr) {
            const std::string file = sumoPath + std::string("/data") + url.substr(pos);
            if (FileHelpers::isReadable(file)) {
                XMLCh* t = XERCES_CPP_NAMESPACE::XMLString::transcode(file.c_str());
                XERCES_CPP_NAMESPACE::InputSource* const result = new XERCES_CPP_NAMESPACE::LocalFileInputSource(t);
                XERCES_CPP_NAMESPACE::XMLString::release(&t);
                return result;
            } else {
                WRITE_WARNING("Cannot read local schema '" + file + (myHaveFallback ? "', will try website lookup." : "', XML validation will fail."));
            }
        }
    }
    if (myHaveFallback || (!StringUtils::startsWith(url, "http:") && !StringUtils::startsWith(url, "https:") && !StringUtils::startsWith(url, "ftp:"))) {
        return nullptr;
    }
    return new XERCES_CPP_NAMESPACE::MemBufInputSource((const XMLByte*)"", 0, "");
}

/****************************************************************************/
