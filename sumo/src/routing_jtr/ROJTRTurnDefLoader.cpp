/****************************************************************************/
/// @file    ROJTRTurnDefLoader.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Jan 2004
/// @version $Id$
///
// Loader for the description of turning percentages
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

#include <set>
#include <string>
#include <utils/common/FileHelpers.h>
#include <utils/common/XMLHelpers.h>
#include <utils/importio/CSVHelpers.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/TplConvert.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <router/RONet.h>
#include "ROJTRHelpers.h"
#include "ROJTREdge.h"
#include "ROJTRTurnDefLoader.h"

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
ROJTRTurnDefLoader::ROJTRTurnDefLoader(RONet &net)
        : SUMOSAXHandler("turn-definitions"), myNet(net),
        myAmInitialised(false)
{}


ROJTRTurnDefLoader::~ROJTRTurnDefLoader()
{}


std::set<ROJTREdge*>
    ROJTRTurnDefLoader::load(const std::string &file)
{
    _file = file;
    FileHelpers::FileType type = FileHelpers::checkFileType(file);
    switch (type) {
    case FileHelpers::XML:
        XMLHelpers::runParser(*this, file);
        return mySinks;
    case FileHelpers::CSV:
        CSVHelpers::runParser(*this, file);
        return mySinks;
    default:
        throw 1;
    }
}


void
ROJTRTurnDefLoader::myStartElement(SumoXMLTag element, const std::string &/*name*/,
                                   const Attributes &attrs)
{
    switch (element) {
    case SUMO_TAG_INTERVAL:
        beginInterval(attrs);
        break;
    case SUMO_TAG_FROMEDGE:
        beginFromEdge(attrs);
        break;
    case SUMO_TAG_TOEDGE:
        addToEdge(attrs);
        break;
    }
}


void
ROJTRTurnDefLoader::myCharacters(SumoXMLTag element, const std::string &/*name*/,
                                 const std::string &chars)
{
    switch (element) {
    case SUMO_TAG_SINK:
        addSink(chars);
        break;
    }
}


void
ROJTRTurnDefLoader::myEndElement(SumoXMLTag element, const std::string &/*name*/)
{
    switch (element) {
    case SUMO_TAG_INTERVAL:
        endInterval();
        break;
    case SUMO_TAG_FROMEDGE:
        endFromEdge();
        break;
    }
}


bool
ROJTRTurnDefLoader::report(const std::string &line)
{
    if (!myAmInitialised) {
        myColumnsParser.reinit(line, ";");
        myAmInitialised = true;
    } else {
        try {
            myColumnsParser.parseLine(line);
            try {
                myIntervalBegin =
                    TplConvert<char>::_2int(getSecure("begin").c_str());
            } catch (NumberFormatException &) {
                MsgHandler::getErrorInstance()->inform("The attribute 'from' is not numeric.");
                return false;
            }
            try {
                myIntervalEnd =
                    TplConvert<char>::_2int(getSecure("end").c_str());
            } catch (NumberFormatException &) {
                MsgHandler::getErrorInstance()->inform("The attribute 'to' is not numeric.");
                return false;
            }
            string id = getSecure("from");
            myEdge = static_cast<ROJTREdge*>(myNet.getEdge(id));
            if (myEdge==0) {
                MsgHandler::getErrorInstance()->inform("The edge '" + id + "' is not known within the network (within a 'from-edge' tag).");
                return false;
            }
            id = getSecure("to");
            ROJTREdge *edge = static_cast<ROJTREdge*>(myNet.getEdge(id));
            if (edge==0) {
                MsgHandler::getErrorInstance()->inform("The edge '" + id + "' is not known within the network (within a 'to-edge' tag).");
                return false;
            }
            SUMOReal probability;
            try {
                probability =
                    TplConvert<char>::_2SUMOReal(getSecure("split").c_str());
            } catch (NumberFormatException &) {
                MsgHandler::getErrorInstance()->inform("The attribute 'perc' is not numeric.");
                return false;
            }
            myEdge->addFollowerProbability(edge, myIntervalBegin, myIntervalEnd, probability);
        } catch (InvalidArgument &) {
            return false;
        }
    }
    return true;
}


std::string
ROJTRTurnDefLoader::getSecure(const std::string &name)
{
    try {
        return myColumnsParser.get(name);
    } catch (UnknownElement &) {
        MsgHandler::getErrorInstance()->inform("The definition of '" + name + "' is missing within the file.");
        throw InvalidArgument("");
    } catch (OutOfBoundsException &) {
        MsgHandler::getErrorInstance()->inform("The definition of '" + name + "' is missing within the file.");
        throw InvalidArgument("");
    }
}


void
ROJTRTurnDefLoader::beginInterval(const Attributes &attrs)
{
    try {
        myIntervalBegin = getInt(attrs, SUMO_ATTR_BEGIN);
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("The attribute 'from' is not numeric ('" + getString(attrs, SUMO_ATTR_FROM) + "').");
        return;
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("The 'from'-attribute is not given.");
        return;
    }
    try {
        myIntervalEnd = getInt(attrs, SUMO_ATTR_END);
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("The attribute 'to' is not numeric ('" + getString(attrs, SUMO_ATTR_FROM) + "').");
        return;
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("The 'to'-attribute is not given.");
        return;
    }
}


void
ROJTRTurnDefLoader::beginFromEdge(const Attributes &attrs)
{
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("The id of an edge is missing within a 'from-edge' tag.");
        return;
    }
    //
    myEdge = static_cast<ROJTREdge*>(myNet.getEdge(id));
    if (myEdge==0) {
        MsgHandler::getErrorInstance()->inform(
            "The edge '" + id + "' is not known within the network (within a 'from-edge' tag).");
        return;
    }
}


void
ROJTRTurnDefLoader::addToEdge(const Attributes &attrs)
{
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("The id of an edge is missing within a 'to-edge' tag.");
        return;
    }
    //
    ROJTREdge *edge = static_cast<ROJTREdge*>(myNet.getEdge(id));
    if (edge==0) {
        MsgHandler::getErrorInstance()->inform("The edge '" + id + "' is not known within the network (within a 'to-edge' tag).");
        return;
    }
    try {
        SUMOReal probability = getFloat(attrs, SUMO_ATTR_PROB);
        myEdge->addFollowerProbability(edge, myIntervalBegin, myIntervalEnd, probability);
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("The attribute 'probability' is not numeric ('" + getString(attrs, SUMO_ATTR_PROB) + "').");
        return;
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("The 'probability'-attribute is not given.");
        return;
    }
}


void
ROJTRTurnDefLoader::addSink(const std::string &chars)
{
    try {
        ROJTRHelpers::parseROJTREdges(myNet, mySinks, chars);
    } catch (ProcessError &) {}
}


void
ROJTRTurnDefLoader::endInterval()
{}


void
ROJTRTurnDefLoader::endFromEdge()
{}



/****************************************************************************/

