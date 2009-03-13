/****************************************************************************/
/// @file    XMLSubSys.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 1 Jul 2002
/// @version $Id$
///
// Utility methods for initialising, closing and using the XML-subsystem
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
#include <utils/common/TplConvert.h>
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
XMLSubSys::init(bool enableValidation) throw(ProcessError) {
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
XMLSubSys::close() throw() {
    for (std::vector<SAX2XMLReader*>::iterator i=myReaders.begin(); i!=myReaders.end(); ++i) {
        delete *i;
    }
    myReaders.clear();
    XMLPlatformUtils::Terminate();
}


SAX2XMLReader *
XMLSubSys::getSAXReader(SUMOSAXHandler &handler) throw() {
    SAX2XMLReader *reader = getSAXReader();
    if (reader==0) {
        return 0;
    }
    reader->setContentHandler(&handler);
    reader->setErrorHandler(&handler);
    return reader;
}


void
XMLSubSys::setHandler(GenericSAXHandler &handler) {
    myReaders[myNextFreeReader-1]->setContentHandler(&handler);
    myReaders[myNextFreeReader-1]->setErrorHandler(&handler);
}


bool
XMLSubSys::runParser(SUMOSAXHandler &handler,
                     const std::string &file) throw() {
    try {
        if (myNextFreeReader == myReaders.size()) {
            myReaders.push_back(getSAXReader());
        }
        myNextFreeReader++;
        setHandler(handler);
        myReaders[myNextFreeReader-1]->parse(file.c_str());
        myNextFreeReader--;
    } catch (ProcessError &e) {
        if (std::string(e.what())!=std::string("Process Error") && std::string(e.what())!=std::string("")) {
            MsgHandler::getErrorInstance()->inform(e.what());
        }
        return false;
    } catch (...) {
        MsgHandler::getErrorInstance()->inform("An error occured.");
        return false;
    }
    return !MsgHandler::getErrorInstance()->wasInformed();
}


SAX2XMLReader *
XMLSubSys::getSAXReader() throw() {
    SAX2XMLReader *reader = XMLReaderFactory::createXMLReader();
    if (reader==0) {
        MsgHandler::getErrorInstance()->inform("The XML-parser could not be build");
        return 0;
    }
    if (!myEnableValidation) {
        reader->setProperty(XMLUni::fgXercesScannerName, (void *)XMLUni::fgWFXMLScanner);
    }
    setFeature(*reader, "http://xml.org/sax/features/namespaces", false);
    setFeature(*reader, "http://apache.org/xml/features/validation/schema", myEnableValidation);
    setFeature(*reader, "http://apache.org/xml/features/validation/schema-full-checking", myEnableValidation);
    setFeature(*reader, "http://xml.org/sax/features/validation", myEnableValidation);
    setFeature(*reader, "http://apache.org/xml/features/validation/dynamic", myEnableValidation);
    return reader;
}


void
XMLSubSys::setFeature(XERCES_CPP_NAMESPACE_QUALIFIER SAX2XMLReader &reader,
                      const std::string &feature, bool value) throw() {
    XMLCh *xmlFeature = XMLString::transcode(feature.c_str());
    reader.setFeature(xmlFeature, value);
    XMLString::release(&xmlFeature);
}




/****************************************************************************/

