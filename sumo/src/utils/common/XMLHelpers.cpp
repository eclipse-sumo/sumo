/***************************************************************************
                          XMLHelpers.cpp
			  Some utility functions for xml usage
                             -------------------
    project              : SUMO
    begin                : Wed, 21 Jan 2004
    copyright            : (C) 2004 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.7  2005/10/07 11:43:30  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.6  2005/09/23 06:05:45  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.5  2005/09/15 12:13:08  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/04/28 09:02:47  dkrajzew
// level3 warnings removed
//
// Revision 1.3  2004/03/19 13:00:20  dkrajzew
// some style adaptions only
//
// Revision 1.2  2004/02/06 08:48:11  dkrajzew
// memory leaks removed
//
// Revision 1.1  2004/01/26 07:27:04  dkrajzew
// added some xml-helping functions
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <iostream>
#include <sax2/XMLReaderFactory.hpp>
#include <util/PlatformUtils.hpp>
#include <utils/common/TplConvert.h>
#include <utils/common/MsgHandler.h>
#include <utils/xml/GenericSAX2Handler.h>
#include "XMLHelpers.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * xerces 2.2 compatibility
 * ======================================================================= */
#if defined(XERCES_HAS_CPP_NAMESPACE)
using namespace XERCES_CPP_NAMESPACE;
#endif


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
SAX2XMLReader *
XMLHelpers::getSAXReader(DefaultHandler &handler)
{
    SAX2XMLReader *reader = XMLReaderFactory::createXMLReader();
    if(reader==0) {
        MsgHandler::getErrorInstance()->inform("The XML-parser could not be build");
        return 0;
    }
    setFeature(*reader,
        "http://xml.org/sax/features/namespaces", false);
    setFeature(*reader,
        "http://apache.org/xml/features/validation/schema", false );
    setFeature(*reader,
        "http://apache.org/xml/features/validation/schema-full-checking", false );
    setFeature(*reader,
        "http://xml.org/sax/features/validation", false );
    setFeature(*reader,
        "http://apache.org/xml/features/validation/dynamic" , false );
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
                       const std::string &feature, bool value)
{
    XMLCh *xmlFeature = XMLString::transcode(feature.c_str());
    reader.setFeature(xmlFeature, false );
    XMLString::release(&xmlFeature);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

