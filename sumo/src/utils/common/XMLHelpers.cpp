/****************************************************************************/
/// @file    XMLHelpers.cpp
/// @author  Daniel Krajzewicz
/// @date    Wed, 21 Jan 2004
/// @version $Id$
///
// Some utility functions for xml usage
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
#include "XMLHelpers.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// xerces 2.2 compatibility
// ===========================================================================
#if defined(XERCES_HAS_CPP_NAMESPACE)
using namespace XERCES_CPP_NAMESPACE;
#endif


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
SAX2XMLReader *
XMLHelpers::getSAXReader(DefaultHandler &handler)
{
    SAX2XMLReader *reader = XMLReaderFactory::createXMLReader();
    if (reader==0) {
        MsgHandler::getErrorInstance()->inform("The XML-parser could not be build");
        return 0;
    }
    setFeature(*reader,
               "http://xml.org/sax/features/namespaces", false);
    setFeature(*reader,
               "http://apache.org/xml/features/validation/schema", false);
    setFeature(*reader,
               "http://apache.org/xml/features/validation/schema-full-checking", false);
    setFeature(*reader,
               "http://xml.org/sax/features/validation", false);
    setFeature(*reader,
               "http://apache.org/xml/features/validation/dynamic" , false);
    reader->setContentHandler(&handler);
    reader->setErrorHandler(&handler);
    return reader;
}


void
XMLHelpers::runParser(XERCES_CPP_NAMESPACE_QUALIFIER DefaultHandler &handler,
                      const std::string &file)
{
    SAX2XMLReader *reader = getSAXReader(handler);
    reader->parse(file.c_str());
    delete reader;
}


void
XMLHelpers::setFeature(XERCES_CPP_NAMESPACE_QUALIFIER SAX2XMLReader &reader,
                       const std::string &feature, bool /*value !!!*/)
{
    XMLCh *xmlFeature = XMLString::transcode(feature.c_str());
    reader.setFeature(xmlFeature, false);
    XMLString::release(&xmlFeature);
}



/****************************************************************************/

