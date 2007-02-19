/****************************************************************************/
/// @file    MSTriggeredXMLReader.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The basic class for classes that read XML-triggers
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
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <utils/common/TplConvert.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include <utils/common/XMLHelpers.h>
#include <microsim/MSEventControl.h>
#include "MSTriggeredReader.h"
#include "MSTriggeredXMLReader.h"

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
        SUMOSAXHandler("sumo-trigger values", filename),
        myParser(0), myHaveMore(true)
{
    MSNet::getInstance()->getBeginOfTimestepEvents().addEvent(
        new MSTriggerCommand(*this), net.getCurrentTimeStep(),
        MSEventControl::NO_CHANGE);
}


MSTriggeredXMLReader::~MSTriggeredXMLReader()
{}


bool
MSTriggeredXMLReader::readNextTriggered()
{
    try {
        while (myHaveMore&&myParser->parseNext(myToken)) {
            if (nextRead()) {
                return true;
            }
        }
    } catch (ProcessError &) {}
    myHaveMore = false;
    return false;
}


void
MSTriggeredXMLReader::myInit()
{
    try {
        myParser = XMLHelpers::getSAXReader(*this);
        if (!myParser->parseFirst(_file.c_str(), myToken)) {
            MsgHandler::getErrorInstance()->inform("Can not read XML-file '" + _file + "'.");
            throw ProcessError();
        }
    } catch (SAXException &e) {
        MsgHandler::getErrorInstance()->inform(TplConvert<XMLCh>::_2str(e.getMessage()));
        throw ProcessError();
    } catch (XMLException &e) {
        MsgHandler::getErrorInstance()->inform(TplConvert<XMLCh>::_2str(e.getMessage()));
        throw ProcessError();
    }

    if (readNextTriggered()) {
        if (_offset<MSNet::getInstance()->getCurrentTimeStep()) {
            _offset = MSNet::getInstance()->getCurrentTimeStep() + 1;
            // !!! Warning?
        }
        /*
        MSEventControl::getBeginOfTimestepEvents()->addEvent(
            new MSTriggerCommand(*this), _offset, MSEventControl::ADAPT_AFTER_EXECUTION);
            */
    } else {
        myHaveMore = false;
    }
}



/****************************************************************************/

