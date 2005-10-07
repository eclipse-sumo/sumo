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
// Revision 1.10  2005/10/07 11:42:15  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.9  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.8  2005/05/04 08:55:13  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.7  2004/07/02 09:39:41  dkrajzew
// debugging while working on INVENT; preparation of classes to be derived for an online-routing
//
// Revision 1.6  2004/01/26 08:01:21  dkrajzew
// loaders and route-def types are now renamed in an senseful way; further changes in order to make both new routers work; documentation added
//
// Revision 1.5  2003/06/18 11:20:54  dkrajzew
// new message and error processing: output to user may be a message, warning or
//  an error now; it is reported to a Singleton (MsgHandler);
//  this handler puts it further to output instances.
//  changes: no verbose-parameter needed; messages are exported to singleton
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

#include <string>
#include <parsers/SAXParser.hpp>
#include <util/PlatformUtils.hpp>
#include <util/TransService.hpp>
#include <sax2/SAX2XMLReader.hpp>
#include <utils/xml/GenericSAX2Handler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/XMLHelpers.h>
#include "ROAbstractRouteDefLoader.h"
#include "ROTypedXMLRoutesLoader.h"
#include "RONet.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
ROTypedXMLRoutesLoader::ROTypedXMLRoutesLoader(ROVehicleBuilder &vb,
                                               RONet &net,
                                               SUMOTime begin,
                                               SUMOTime end,
                                               const std::string &file)
    : ROAbstractRouteDefLoader(vb, net, begin, end),
    SUMOSAXHandler("xml-route definitions", file),
    _parser(XMLHelpers::getSAXReader(*this)), _token(), _ended(false)
{
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
ROTypedXMLRoutesLoader::myReadRoutesAtLeastUntil(SUMOTime time)
{
    while(getCurrentTimeStep()<time&&!ended()) {
        beginNextRoute();
        while(!nextRouteRead()&&!ended()) {
            try {
                _parser->parseNext(_token);
            } catch (std::string) {
                return false;
            }
        }
    }
    return true;
}


bool
ROTypedXMLRoutesLoader::init(OptionsCont &options)
{
    return _parser->parseFirst(_file.c_str(), _token);
}


void
ROTypedXMLRoutesLoader::endDocument()
{
    _ended = true;
}


bool
ROTypedXMLRoutesLoader::ended() const
{
    return _ended;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


