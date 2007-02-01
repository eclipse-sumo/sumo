/****************************************************************************/
/// @file    ROTypedXMLRoutesLoader.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: $
///
// The basic class for loading routes from XML-files
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


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
ROTypedXMLRoutesLoader::ROTypedXMLRoutesLoader(ROVehicleBuilder &vb,
        RONet &net,
        SUMOTime begin,
        SUMOTime end,
        const std::string &file)
        : ROAbstractRouteDefLoader(vb, net, begin, end),
        SUMOSAXHandler("xml-route definitions", file),
        _parser(XMLHelpers::getSAXReader(*this)), _token(), _ended(false)
{}


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
    while (getCurrentTimeStep()<time&&!ended()) {
        beginNextRoute();
        while (!nextRouteRead()&&!ended()) {
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
ROTypedXMLRoutesLoader::init(OptionsCont &)
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



/****************************************************************************/

