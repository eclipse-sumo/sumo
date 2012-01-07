/****************************************************************************/
/// @file    XMLSubSys.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Mon, 1 Jul 2002
/// @version $Id$
///
// Utility methods for initialising, closing and using the XML-subsystem
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

#include <iostream>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <utils/common/TplConvert.h>
#include <utils/common/MsgHandler.h>
#include "SUMOSAXHandler.h"
#include "XMLSubSys.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static member variables
// ===========================================================================
std::vector<SAX2XMLReader*> XMLSubSys::myReaders;
unsigned int XMLSubSys::myNextFreeReader;
bool XMLSubSys::myEnableValidation;


// ===========================================================================
// method definitions
// ===========================================================================
void
XMLSubSys::init(bool enableValidation) {
    myEnableValidation = enableValidation;
    try {
        XMLPlatformUtils::Initialize();
        myReaders.push_back(getSAXReader());
        myNextFreeReader = 0;
    } catch (const XMLException& e) {
        throw ProcessError("Error during XML-initialization:\n " + TplConvert<XMLCh>::_2str(e.getMessage()));
    }
}


void
XMLSubSys::close() {
    for (std::vector<SAX2XMLReader*>::iterator i = myReaders.begin(); i != myReaders.end(); ++i) {
        delete *i;
    }
    myReaders.clear();
    XMLPlatformUtils::Terminate();
}


SAX2XMLReader*
XMLSubSys::getSAXReader(SUMOSAXHandler& handler) {
    SAX2XMLReader* reader = getSAXReader();
    if (reader == 0) {
        return 0;
    }
    reader->setContentHandler(&handler);
    reader->setErrorHandler(&handler);
    return reader;
}


void
XMLSubSys::setHandler(GenericSAXHandler& handler) {
    myReaders[myNextFreeReader - 1]->setContentHandler(&handler);
    myReaders[myNextFreeReader - 1]->setErrorHandler(&handler);
}


bool
XMLSubSys::runParser(GenericSAXHandler& handler,
                     const std::string& file) {
    try {
        if (myNextFreeReader == myReaders.size()) {
            myReaders.push_back(getSAXReader());
        }
        myNextFreeReader++;
        setHandler(handler);
        std::string prevFile = handler.getFileName();
        handler.setFileName(file);
        myReaders[myNextFreeReader - 1]->parse(file.c_str());
        handler.setFileName(prevFile);
        myNextFreeReader--;
    } catch (ProcessError& e) {
        if (std::string(e.what()) != std::string("Process Error") && std::string(e.what()) != std::string("")) {
            WRITE_ERROR(e.what());
        }
        return false;
    } catch (...) {
        WRITE_ERROR("An error occured.");
        return false;
    }
    return !MsgHandler::getErrorInstance()->wasInformed();
}


SAX2XMLReader*
XMLSubSys::getSAXReader() {
    SAX2XMLReader* reader = XMLReaderFactory::createXMLReader();
    if (reader == 0) {
        WRITE_ERROR("The XML-parser could not be build");
        return 0;
    }
    if (!myEnableValidation) {
        reader->setProperty(XMLUni::fgXercesScannerName, (void*)XMLUni::fgWFXMLScanner);
    }
    setFeature(*reader, "http://xml.org/sax/features/namespaces", false);
    setFeature(*reader, "http://apache.org/xml/features/validation/schema", myEnableValidation);
    setFeature(*reader, "http://apache.org/xml/features/validation/schema-full-checking", myEnableValidation);
    setFeature(*reader, "http://xml.org/sax/features/validation", myEnableValidation);
    setFeature(*reader, "http://apache.org/xml/features/validation/dynamic", myEnableValidation);
    return reader;
}


void
XMLSubSys::setFeature(XERCES_CPP_NAMESPACE_QUALIFIER SAX2XMLReader& reader,
                      const std::string& feature, bool value) {
    XMLCh* xmlFeature = XMLString::transcode(feature.c_str());
    reader.setFeature(xmlFeature, value);
    XMLString::release(&xmlFeature);
}


/****************************************************************************/

