/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    OptionsIO.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Mon, 17 Dec 2001
///
// Helper for parsing command line arguments and reading configuration files
/****************************************************************************/
#include <config.h>

#include <string>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <xercesc/framework/XMLPScanToken.hpp>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include "OptionsIO.h"
#include "OptionsCont.h"
#include "OptionsLoader.h"
#include "OptionsParser.h"
#include <utils/common/FileHelpers.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringUtils.h>
#ifdef HAVE_ZLIB
#include <foreign/zstr/zstr.hpp>
#endif
#include <utils/xml/IStreamInputSource.h>


// ===========================================================================
// static member definitions
// ===========================================================================
std::vector<std::string> OptionsIO::myArgs;
std::chrono::time_point<std::chrono::system_clock> OptionsIO::myLoadTime;


// ===========================================================================
// method definitions
// ===========================================================================
void
OptionsIO::setArgs(int argc, char** argv) {
    myArgs.clear();
    for (int i = 0; i < argc; i++) {
        myArgs.push_back(StringUtils::transcodeFromLocal(argv[i]));
    }
}


void
OptionsIO::setArgs(const std::vector<std::string>& args) {
    myArgs.resize(1);  // will insert an empty string if no first element is there
    myArgs.insert(myArgs.end(), args.begin(), args.end());
}


void
OptionsIO::getOptions(const bool commandLineOnly) {
    myLoadTime = std::chrono::system_clock::now();
    if (myArgs.size() == 2 && myArgs[1][0] != '-') {
        // special case only one parameter, check who can handle it
        if (OptionsCont::getOptions().setByRootElement(getRoot(myArgs[1]), myArgs[1])) {
            if (!commandLineOnly) {
                loadConfiguration();
            }
            return;
        }
    }
    // preparse the options
    //  (maybe another configuration file was chosen)
    if (!OptionsParser::parse(myArgs, true)) {
        throw ProcessError(TL("Could not parse commandline options."));
    }
    if (!commandLineOnly || OptionsCont::getOptions().isSet("save-configuration", false)) {
        // read the configuration when everything's ok
        loadConfiguration();
    }
}


void
OptionsIO::loadConfiguration() {
    OptionsCont& oc = OptionsCont::getOptions();
    if (oc.exists("configuration-file") && oc.isSet("configuration-file")) {
        const std::string path = oc.getString("configuration-file");
        if (!FileHelpers::isReadable(path)) {
            throw ProcessError(TLF("Could not access configuration '%'.", oc.getString("configuration-file")));
        }
        const bool verbose = !oc.exists("verbose") || oc.getBool("verbose");
        if (verbose) {
            PROGRESS_BEGIN_MESSAGE(TL("Loading configuration"));
        }
        oc.resetWritable();
        // build parser
        XERCES_CPP_NAMESPACE::SAXParser parser;
        parser.setValidationScheme(XERCES_CPP_NAMESPACE::SAXParser::Val_Never);
        parser.setDisableDefaultEntityResolution(true);
        // start the parsing
        OptionsLoader handler(OptionsCont::getOptions());
        try {
            parser.setDocumentHandler(&handler);
            parser.setErrorHandler(&handler);
            parser.parse(StringUtils::transcodeToLocal(path).c_str());
            if (handler.errorOccurred()) {
                throw ProcessError(TLF("Could not load configuration '%'.", path));
            }
        } catch (const XERCES_CPP_NAMESPACE::XMLException& e) {
            throw ProcessError("Could not load configuration '" + path + "':\n " + StringUtils::transcode(e.getMessage()));
        }
        oc.relocateFiles(path);
        if (verbose) {
            PROGRESS_DONE_MESSAGE();
        }
    }
    if (myArgs.size() > 2) {
        // reparse the options (overwrite the settings from the configuration file)
        oc.resetWritable();
        if (!OptionsParser::parse(myArgs)) {
            throw ProcessError(TL("Could not parse commandline options."));
        }
    }
}


std::string
OptionsIO::getRoot(const std::string& filename) {
    // build parser
    XERCES_CPP_NAMESPACE::SAXParser parser;
    parser.setValidationScheme(XERCES_CPP_NAMESPACE::SAXParser::Val_Never);
    parser.setDisableDefaultEntityResolution(true);
    // start the parsing
    OptionsLoader handler(OptionsCont::getOptions());
    try {
        parser.setDocumentHandler(&handler);
        parser.setErrorHandler(&handler);
        XERCES_CPP_NAMESPACE::XMLPScanToken token;
        if (!FileHelpers::isReadable(filename) || FileHelpers::isDirectory(filename)) {
            throw ProcessError(TLF("Could not open '%'.", filename));
        }
#ifdef HAVE_ZLIB
        zstr::ifstream istream(StringUtils::transcodeToLocal(filename).c_str(), std::fstream::in | std::fstream::binary);
        IStreamInputSource inputStream(istream);
        const bool result = parser.parseFirst(inputStream, token);
#else
        const bool result = parser.parseFirst(StringUtils::transcodeToLocal(filename).c_str(), token);
#endif
        if (!result) {
            throw ProcessError(TLF("Can not read XML-file '%'.", filename));
        }
        while (parser.parseNext(token) && handler.getItem() == "");
        if (handler.errorOccurred()) {
            throw ProcessError(TLF("Could not load '%'.", filename));
        }
    } catch (const XERCES_CPP_NAMESPACE::XMLException& e) {
        throw ProcessError("Could not load '" + filename + "':\n " + StringUtils::transcode(e.getMessage()));
    }
    return handler.getItem();
}


/****************************************************************************/
