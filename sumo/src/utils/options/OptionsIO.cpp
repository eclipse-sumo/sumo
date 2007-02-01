/****************************************************************************/
/// @file    OptionsIO.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 17 Dec 2001
/// @version $Id: $
///
// Loads the configuration file using "OptionsLoader"
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <iostream>
#include <parsers/SAXParser.hpp>
#include <sax/HandlerBase.hpp>
#include <sax/AttributeList.hpp>
#include <util/PlatformUtils.hpp>
#include <sax/SAXParseException.hpp>
#include <sax/SAXException.hpp>
#include <cstdlib>
#include "OptionsIO.h"
#include "OptionsCont.h"
#include "OptionsLoader.h"
#include "OptionsParser.h"
#include <utils/common/FileHelpers.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/TplConvert.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
bool
OptionsIO::getOptions(bool loadConfig, OptionsCont *oc, int argc, char **argv)
{
    bool ret = true;
    // preparse the options
    //  (maybe another configuration file was chosen)
    ret = OptionsParser::parse(oc, argc, argv);
    // return when the help shall be printed
    if (oc->exists("help")&&oc->getBool("help")) {
        return ret;
    }
    // check whether to use the command line parameetr only
    if (!loadConfig) {
        return true;
    }
    // read the configuration when everything's ok
    if (ret) {
        oc->resetWritable();
        ret = loadConfiguration(oc);
    }
    // reparse the options
    //  (overwrite the settings from the configuration file)
    if (ret) {
        oc->resetWritable();
        ret = OptionsParser::parse(oc, argc, argv);
    }
    return ret;
}


bool
OptionsIO::loadConfiguration(OptionsCont *oc)
{
    if (!oc->exists("configuration-file") || !oc->isSet("configuration-file")) {
        return true;
    }
    bool ok = true;
    string path = oc->getString("configuration-file");
    if (!FileHelpers::exists(path)) {
        MsgHandler::getErrorInstance()->inform("Could not find configuration '" + oc->getString("configuration-file") + "'.");
        return false;
    }
    MsgHandler::getMessageInstance()->beginProcessMsg("Loading configuration...");
    // build parser
    SAXParser parser;
    parser.setValidationScheme(SAXParser::Val_Auto);
    parser.setDoNamespaces(false);
    parser.setDoSchema(false);
    // start the parsing
    OptionsLoader handler(oc, path.c_str(), oc->getBool("verbose"));
    try {
        parser.setDocumentHandler(&handler);
        parser.setErrorHandler(&handler);
        parser.parse(path.c_str());
        if (handler.errorOccured()) {
            ok = false;
        }
    } catch (const XMLException&) {
        ok = false;
    }
    if (oc->getBool("verbose")) {
        if (ok) {
            MsgHandler::getMessageInstance()->endProcessMsg("done.");
        } else {
            MsgHandler::getMessageInstance()->endProcessMsg("failed.");
        }
    }
    if (!ok) {
        MsgHandler::getErrorInstance()->inform("Could not load configuration '" + path + "'.");
    }
    return ok;
}



/****************************************************************************/

