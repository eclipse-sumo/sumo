/***************************************************************************
                          OptionsIO.cpp
			  Loads the configuration file using "OptionsLoader"
			  and parses the given command line arguments using
			  "OptionsParser"
                             -------------------
    project              : SUMO
    begin                : Mon, 17 Dec 2001
    copyright            : (C) 2001 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.4  2002/04/29 05:38:54  dkrajzew
// Better error handling on missing configuration implemented
//
// Revision 1.3  2002/04/17 11:19:57  dkrajzew
// windows-carriage returns removed
//
// Revision 1.2  2002/04/16 12:22:59  dkrajzew
// Usage of SUMO_DATA removed
//
// Revision 1.1.1.1  2002/04/08 07:21:25  traffic
// new project name
//
// Revision 2.1  2002/03/20 08:20:57  dkrajzew
// New configuration-default handling
//
// Revision 2.0  2002/02/14 14:43:27  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.1  2002/02/13 15:48:19  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
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
#include "FileHelpers.h"
#include "XMLConvert.h"

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * method definitions
 * ======================================================================= */
bool OptionsIO::getOptions(OptionsCont *oc, int argc, char **argv) {
    bool ret = true;
    ret = OptionsParser::parse(oc, argc, argv);
    if(ret) oc->resetDefaults();
    if(ret)
        ret = loadConfiguration(oc);
    if(ret) oc->resetDefaults();
    if(ret) ret = OptionsParser::parse(oc, argc, argv);
    return ret;
}

string OptionsIO::getConfigurationPath(OptionsCont *oc, bool &ok) {
    string path = oc->getString("c");
    // check the user supplied path
    if(FileHelpers::exists(path))
        return path;
    cout << "The configurations file '" << path << "' could not be found." << endl;
    ok = false;
    return "";
}

bool OptionsIO::loadConfiguration(OptionsCont *oc) {
    if(oc->exists("no-config") && oc->isSet("no-config") && oc->getBool("no-config"))
        return true;
    if(!oc->exists("c") || !oc->isSet("c"))
        return true;
    bool ok = true;
    string path = getConfigurationPath(oc, ok);
    if(path.length()==0||!ok)
        return false;
    try {
      XMLPlatformUtils::Initialize();
    } catch (const XMLException& toCatch) {
      cerr << "Error during XML-initialization: " << XMLConvert::_2str(toCatch.getMessage()) << endl;
      return false;
    }
    // build parser
    SAXParser parser;
    if(oc->getBool("v"))
      cout << "Loading configuration..." << endl;
    parser.setValidationScheme(SAXParser::Val_Auto);
    parser.setDoNamespaces(false);
    parser.setDoSchema(false);
    // start the parsing
    OptionsLoader *handler = new OptionsLoader(oc, path.c_str(), oc->getBool("w"), oc->getBool("v"));
    try {
      parser.setDocumentHandler(handler);
      parser.setErrorHandler(handler);
      parser.parse(path.c_str());
      if(handler->errorOccured())
        ok = false;
    } catch (const XMLException& toCatch) {
      cerr << "Error: " << XMLConvert::_2str(toCatch.getMessage()) << endl;
      ok = false;
    }
    //XMLPlatformUtils::Terminate();
    delete handler;
    return ok;
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "OptionsIO.icc"
//#endif

// Local Variables:
// mode:C++
// End:



