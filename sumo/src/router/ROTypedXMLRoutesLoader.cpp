//---------------------------------------------------------------------------//
//                        ROTypedXMLRoutesLoader.cpp -
//  The basic class for loading routes from XML-files
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.5  2003/06/18 11:20:54  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.4  2003/03/20 16:39:17  dkrajzew
// periodical car emission implemented; windows eol removed
//
// Revision 1.3  2003/03/03 15:22:37  dkrajzew
// debugging
//
// Revision 1.2  2003/02/07 10:45:06  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H
#include <string>
#include <parsers/SAXParser.hpp>
#include <util/PlatformUtils.hpp>
#include <util/TransService.hpp>
#include <sax2/SAX2XMLReader.hpp>
#include <sax2/XMLReaderFactory.hpp>
#include <sax2/DefaultHandler.hpp>
#include <utils/xml/GenericSAX2Handler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include "ROTypedRoutesLoader.h"
#include "ROTypedXMLRoutesLoader.h"
#include "RONet.h"

using namespace std;

ROTypedXMLRoutesLoader::ROTypedXMLRoutesLoader(RONet &net,
                                               const std::string &file)
    : ROTypedRoutesLoader(net),
    SUMOSAXHandler("xml-route definitions", file),
    _parser(XMLReaderFactory::createXMLReader()), _token()
{
    _parser->setFeature(
        XMLString::transcode("http://xml.org/sax/features/validation"),
        false);
    _parser->setContentHandler(this);
    _parser->setErrorHandler(this);
}

ROTypedXMLRoutesLoader::~ROTypedXMLRoutesLoader()
{
    delete _parser;
}

void
ROTypedXMLRoutesLoader::closeReading()
{
    _parser->parseReset(_token);
}

bool
ROTypedXMLRoutesLoader::readNextRoute(long start)
{
    _nextRouteRead = false;
    while(!_nextRouteRead&&!_ended) {
        try {
            _parser->parseNext(_token);
        } catch (...) {
            return false;
        }
    }
    return true;
}

bool
ROTypedXMLRoutesLoader::addAllRoutes()
{
    _parser->parse(_file.c_str());
    return !MsgHandler::getErrorInstance()->wasInformed();
}

bool
ROTypedXMLRoutesLoader::startReadingSteps()
{
/*    _parser->setContentHandler(this);
    _parser->setErrorHandler(this);*/
    return true;
}

bool
ROTypedXMLRoutesLoader::myInit(OptionsCont &options)
{
/*    _parser->setContentHandler(this);
    _parser->setErrorHandler(this);*/
    return _parser->parseFirst(_file.c_str(), _token);
}

void
ROTypedXMLRoutesLoader::endDocument()
{
    _ended = true;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "ROTypedXMLRoutesLoader.icc"
//#endif

// Local Variables:
// mode:C++
// End:


