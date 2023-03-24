/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2002-2023 German Aerospace Center (DLR) and others.
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
/// @file    XMLSubSys.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Mon, 1 Jul 2002
///
// Utility methods for initialising, closing and using the XML-subsystem
/****************************************************************************/
#include <config.h>

#include <cstdint>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/framework/XMLGrammarPoolImpl.hpp>
#include <utils/common/FileHelpers.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringUtils.h>
#include "SUMOSAXHandler.h"
#include "SUMOSAXReader.h"
#include "XMLSubSys.h"

using XERCES_CPP_NAMESPACE::SAX2XMLReader;
using XERCES_CPP_NAMESPACE::XMLPlatformUtils;
using XERCES_CPP_NAMESPACE::XMLReaderFactory;


// ===========================================================================
// static member variables
// ===========================================================================
std::vector<SUMOSAXReader*> XMLSubSys::myReaders;
int XMLSubSys::myNextFreeReader;
std::string XMLSubSys::myValidationScheme = "local";
std::string XMLSubSys::myNetValidationScheme = "local";
std::string XMLSubSys::myRouteValidationScheme = "local";
XERCES_CPP_NAMESPACE::XMLGrammarPool* XMLSubSys::myGrammarPool = nullptr;


// ===========================================================================
// method definitions
// ===========================================================================
void
XMLSubSys::init() {
    try {
        XMLPlatformUtils::Initialize();
        myNextFreeReader = 0;
    } catch (const XERCES_CPP_NAMESPACE::XMLException& e) {
        throw ProcessError("Error during XML-initialization:\n " + StringUtils::transcode(e.getMessage()));
    }
}


void
XMLSubSys::setValidation(const std::string& validationScheme, const std::string& netValidationScheme, const std::string& routeValidationScheme) {
    if (validationScheme != "never" && validationScheme != "auto" && validationScheme != "always" && validationScheme != "local") {
        throw ProcessError("Unknown xml validation scheme + '" + validationScheme + "'.");
    }
    myValidationScheme = validationScheme;
    if (netValidationScheme != "never" && netValidationScheme != "auto" && netValidationScheme != "always" && netValidationScheme != "local") {
        throw ProcessError("Unknown network validation scheme + '" + netValidationScheme + "'.");
    }
    myNetValidationScheme = netValidationScheme;
    if (routeValidationScheme != "never" && routeValidationScheme != "auto" && routeValidationScheme != "always" && routeValidationScheme != "local") {
        throw ProcessError("Unknown route validation scheme + '" + routeValidationScheme + "'.");
    }
    myRouteValidationScheme = routeValidationScheme;
    if (myGrammarPool == nullptr &&
            (myValidationScheme != "never" ||
             myNetValidationScheme != "never" ||
             myRouteValidationScheme != "never")) {
        myGrammarPool = new XERCES_CPP_NAMESPACE::XMLGrammarPoolImpl(XMLPlatformUtils::fgMemoryManager);
        SAX2XMLReader* parser(XMLReaderFactory::createXMLReader(XMLPlatformUtils::fgMemoryManager, myGrammarPool));
#if _XERCES_VERSION >= 30100
        parser->setFeature(XERCES_CPP_NAMESPACE::XMLUni::fgXercesHandleMultipleImports, true);
#endif
        const char* sumoPath = std::getenv("SUMO_HOME");
        if (sumoPath == nullptr || !FileHelpers::isReadable(sumoPath + std::string("/data/xsd/net_file.xsd"))) {
            bool needWarning = true;
            if (validationScheme == "local") {
                WRITE_WARNING(TL("Environment variable SUMO_HOME is not set properly, disabling XML validation. Set 'auto' or 'always' for web lookups."));
                needWarning = false;
                myValidationScheme = "never";
            }
            if (netValidationScheme == "local") {
                if (needWarning) {
                    WRITE_WARNING(TL("Environment variable SUMO_HOME is not set properly, disabling XML validation. Set 'auto' or 'always' for web lookups."));
                    needWarning = false;
                }
                myNetValidationScheme = "never";
            }
            if (routeValidationScheme == "local") {
                if (needWarning) {
                    WRITE_WARNING(TL("Environment variable SUMO_HOME is not set properly, disabling XML validation. Set 'auto' or 'always' for web lookups."));
                    needWarning = false;
                }
                myRouteValidationScheme = "never";
            }
            if (needWarning) {
                WRITE_WARNING(TL("Environment variable SUMO_HOME is not set properly, XML validation will fail or use slow website lookups."));
            }
            return;
        }
        for (const char* const& filetype : {
                    "additional", "routes", "net"
                }) {
            const std::string file = sumoPath + std::string("/data/xsd/") + filetype + "_file.xsd";
            if (!parser->loadGrammar(file.c_str(), XERCES_CPP_NAMESPACE::Grammar::SchemaGrammarType, true)) {
                WRITE_WARNINGF(TL("Cannot read local schema '%'."), file);
            }
        }
    }
}


void
XMLSubSys::close() {
    for (std::vector<SUMOSAXReader*>::iterator i = myReaders.begin(); i != myReaders.end(); ++i) {
        delete *i;
    }
    myReaders.clear();
    delete myGrammarPool;
    myGrammarPool = nullptr;
    XMLPlatformUtils::Terminate();
    StringUtils::resetTranscoder();
}


SUMOSAXReader*
XMLSubSys::getSAXReader(SUMOSAXHandler& handler, const bool isNet, const bool isRoute) {
    std::string validationScheme = isNet ? myNetValidationScheme : myValidationScheme;
    if (isRoute) {
        validationScheme = myRouteValidationScheme;
    }
    return new SUMOSAXReader(handler, validationScheme, myGrammarPool);
}


void
XMLSubSys::setHandler(GenericSAXHandler& handler) {
    myReaders[myNextFreeReader - 1]->setHandler(handler);
}


bool
XMLSubSys::runParser(GenericSAXHandler& handler, const std::string& file,
                     const bool isNet, const bool isRoute, const bool isExternal, const bool catchExceptions) {
    MsgHandler::getErrorInstance()->clear();
    std::string errorMsg = "";
    try {
        std::string validationScheme = isNet ? myNetValidationScheme : myValidationScheme;
        if (isRoute) {
            validationScheme = myRouteValidationScheme;
        }
        if (isExternal && validationScheme == "local") {
            WRITE_MESSAGEF(TL("Disabling XML validation for external file '%'. Use 'auto' or 'always' to enable."), file);
            validationScheme = "never";
        }
        if (myNextFreeReader == (int)myReaders.size()) {
            myReaders.push_back(new SUMOSAXReader(handler, validationScheme, myGrammarPool));
        } else {
            myReaders[myNextFreeReader]->setValidation(validationScheme);
            myReaders[myNextFreeReader]->setHandler(handler);
        }
        myNextFreeReader++;
        std::string prevFile = handler.getFileName();
        handler.setFileName(file);
        myReaders[myNextFreeReader - 1]->parse(file);
        handler.setFileName(prevFile);
        myNextFreeReader--;
    } catch (const ProcessError& e) {
        if (catchExceptions) {
            errorMsg = std::string(e.what()) != std::string("") ? e.what() : TL("Process Error");
        } else {
            throw;
        }
    } catch (const std::runtime_error& re) {
        errorMsg = TLF("Runtime error: % while parsing '%'", re.what(), file);
    } catch (const std::exception& ex) {
        errorMsg = TLF("Error occurred: % while parsing '%'", ex.what(), file);
    } catch (const XERCES_CPP_NAMESPACE::SAXException& e) {
        errorMsg = TLF("SAX error occured while parsing '%':\n %", file, StringUtils::transcode(e.getMessage()));
    } catch (...) {
        errorMsg = TLF("Unspecified error occurred wile parsing '%'", file);
    }
    if (errorMsg != "") {
        if (catchExceptions) {
            WRITE_ERROR(errorMsg);
        } else {
            throw ProcessError(errorMsg);
        }
    }
    return !MsgHandler::getErrorInstance()->wasInformed();
}


/****************************************************************************/
