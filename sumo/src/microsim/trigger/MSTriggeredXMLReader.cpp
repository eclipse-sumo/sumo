/****************************************************************************/
/// @file    MSTriggeredXMLReader.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The basic class for classes that read XML-triggers
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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

#include <string>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <utils/common/TplConvert.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/XMLSubSys.h>
#include <microsim/MSEventControl.h>
#include "MSTriggeredReader.h"
#include "MSTriggeredXMLReader.h"
#include <utils/common/WrappingCommand.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
MSTriggeredXMLReader::MSTriggeredXMLReader(MSNet &net,
        const std::string &filename)
        : MSTriggeredReader(net),
        SUMOSAXHandler(filename),
        myParser(0), myHaveMore(true) {
    Command* c = new WrappingCommand< MSTriggeredReader >(this, &MSTriggeredReader::wrappedExecute);
    MSNet::getInstance()->getEmissionEvents().addEvent(
        c, net.getCurrentTimeStep(), MSEventControl::NO_CHANGE);
}


MSTriggeredXMLReader::~MSTriggeredXMLReader() throw() {}


bool
MSTriggeredXMLReader::readNextTriggered() {
    while (myHaveMore&&myParser->parseNext(myToken)) {
        if (nextRead()) {
            return true;
        }
    }
    myHaveMore = false;
    return false;
}


void
MSTriggeredXMLReader::myInit() {
    try {
        myParser = XMLSubSys::getSAXReader(*this);
        if (!myParser->parseFirst(getFileName().c_str(), myToken)) {
            throw ProcessError("Can not read XML-file '" + getFileName() + "'.");

        }
    } catch (SAXException &e) {
        throw ProcessError(TplConvert<XMLCh>::_2str(e.getMessage()));

    } catch (XMLException &e) {
        throw ProcessError(TplConvert<XMLCh>::_2str(e.getMessage()));

    }

    if (readNextTriggered()) {
        if (myOffset<MSNet::getInstance()->getCurrentTimeStep()) {
            myOffset = MSNet::getInstance()->getCurrentTimeStep() + 1;
            // !!! Warning?
        }
    } else {
        myHaveMore = false;
    }
}



/****************************************************************************/

