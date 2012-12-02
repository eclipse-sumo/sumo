/****************************************************************************/
/// @file    ROTypedXMLRoutesLoader.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// Base class for loading routes from XML-files
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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

#include <string>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/common/StdDefs.h>
#include "ROTypedXMLRoutesLoader.h"
#include "RONet.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
ROTypedXMLRoutesLoader::ROTypedXMLRoutesLoader(RONet& net,
        SUMOTime begin, SUMOTime end,
        const std::string& file)
    : SUMOSAXHandler(file), myNet(net), myBegin(begin), myEnd(end),
      myParser(0), myCurrentDepart(-1), myNextRouteRead(false),
      myEnded(false) {
    try {
        myParser = XMLSubSys::getSAXReader(*this);
        myParser->parseFirst(getFileName());
    } catch (...) {
        throw ProcessError();
    }
}


ROTypedXMLRoutesLoader::~ROTypedXMLRoutesLoader() {
    delete myParser;
}


bool
ROTypedXMLRoutesLoader::readRoutesAtLeastUntil(SUMOTime time) {
    while (getLastReadTimeStep() < time && !ended()) {
        myNextRouteRead = false;
        while (!myNextRouteRead && !ended()) {
            myEnded = !myParser->parseNext();
        }
    }
    return true;
}


void
ROTypedXMLRoutesLoader::endDocument() {
    myEnded = true;
}


/****************************************************************************/
