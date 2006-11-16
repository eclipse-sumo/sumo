//---------------------------------------------------------------------------//
//                        MSWeightsHandler.cpp -
//  A SAX-handler for loading SUMO-weights (aggregated dumps)
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
// Revision 1.2  2006/11/16 10:50:44  dkrajzew
// warnings removed
//
// Revision 1.1  2006/11/14 06:44:51  dkrajzew
// first steps towards car2car-based rerouting
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

#include <string>
#include <utils/options/OptionsCont.h>
#include <utils/xml/GenericSAX2Handler.h>
#include <utils/xml/AttributesHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include "MSEdge.h"
#include "MSNet.h"
#include "MSWeightsHandler.h"

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
MSWeightsHandler::MSWeightsHandler(const OptionsCont &oc, MSNet &net,
                                   const std::string &file,
                                   bool useLanes)
    : SUMOSAXHandler("sumo-netweights", file), _options(oc), _net(net),
    _currentTimeBeg(-1), _currentTimeEnd(-1), _currentEdge(0),
    myUseLanes(useLanes)
{
    _scheme = _options.getString("scheme");
}


MSWeightsHandler::~MSWeightsHandler()
{
}


void MSWeightsHandler::myStartElement(int element, const std::string &/*name*/,
                                      const Attributes &attrs)
{
    switch(element) {
    case SUMO_TAG_INTERVAL:
        parseTimeStep(attrs);
        break;
    case SUMO_TAG_EDGE:
        parseEdge(attrs);
        break;
    case SUMO_TAG_LANE:
        parseLane(attrs);
        break;
    default:
        break;
    }
}


void
MSWeightsHandler::parseTimeStep(const Attributes &attrs)
{
    try {
        _currentTimeBeg = getLong(attrs, SUMO_ATTR_BEGIN);
        _currentTimeEnd = getLong(attrs, SUMO_ATTR_END);
    } catch (...) {
        MsgHandler::getErrorInstance()->inform("Problems with timestep value.");
    }
}


void
MSWeightsHandler::parseEdge(const Attributes &attrs)
{
    _currentEdge = 0;
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
        _currentEdge = MSEdge::dictionary(id);
        myAggValue = 0;
        myNoLanes = 0;
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("An edge without an id occured.");
        return;
    }
    // return if the lanes shall be used
    if(myUseLanes) {
        return;
    }
    // parse the edge information if wished
    try {
        myAggValue = getFloat(attrs, SUMO_ATTR_VALUE);
        myNoLanes = 1;
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("Missing value '" + _scheme + "' in edge '" + id + "'.");
    } catch (NumberFormatException) {
        MsgHandler::getErrorInstance()->inform("The value should be numeric, but is not ('" + getString(attrs, SUMO_ATTR_VALUE) + "'");
        if(id.length()!=0)
            MsgHandler::getErrorInstance()->inform(" In edge '" + id + "' at time step " + toString<long>(_currentTimeBeg) + ".");
    }
}


void
MSWeightsHandler::parseLane(const Attributes &attrs)
{
    if(!myUseLanes) {
        return;
    }
    string id;
    SUMOReal value = -1;
    // try to get the lane id
    try {
        id = getString(attrs, SUMO_ATTR_ID);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("A lane without an id occured.");
    }
    // try to get the lane value - depending on the used scheme
    try {
        value = getFloat(attrs, SUMO_ATTR_VALUE);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("Missing value '" + _scheme + "' in lane '" + id + "'.");
    } catch (NumberFormatException) {
        MsgHandler::getErrorInstance()->inform("The value should be numeric, but is not ('" + getString(attrs, SUMO_ATTR_VALUE) + "'");
        if(id.length()!=0)
            MsgHandler::getErrorInstance()->inform(" In lane '" + id + "' at time step " + toString<long>(_currentTimeBeg) + ".");
    }
    // set the values when retrieved (no errors)
    if(id.length()!=0&&value>=0&&_currentEdge!=0) {
        myAggValue += value;
        myNoLanes++;
    }
}


void MSWeightsHandler::myCharacters(int /*element*/, const std::string &/*name*/,
                                    const std::string &/*chars*/)
{
}


void MSWeightsHandler::myEndElement(int element, const std::string &/*name*/)
{
    if(element==SUMO_TAG_EDGE) {
        _currentEdge->addWeight(myAggValue / (SUMOReal) myNoLanes,
            _currentTimeBeg, _currentTimeEnd);
        _currentEdge = 0;
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


