//---------------------------------------------------------------------------//
//                        ROWeightsHandler.cpp -
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
// Revision 1.9  2005/05/04 08:55:13  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.8  2004/11/23 10:25:52  dkrajzew
// debugging
//
// Revision 1.7  2004/07/02 09:39:41  dkrajzew
// debugging while working on INVENT; preparation of classes to be derived
//  for an online-routing
//
// Revision 1.6  2004/01/26 08:01:21  dkrajzew
// loaders and route-def types are now renamed in an senseful way;
//  further changes in order to make both new routers work; documentation
//  added
//
// Revision 1.5  2003/06/19 11:00:26  dkrajzew
// usage of false tag-enums patched
//
// Revision 1.4  2003/06/18 11:20:54  dkrajzew
// new message and error processing: output to user may be a message, warning
//  or an error now; it is reported to a Singleton (MsgHandler);
//  this handler puts it further to output instances.
//  changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.3  2003/02/07 10:45:06  dkrajzew
// updated
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <utils/options/OptionsCont.h>
#include <utils/xml/GenericSAX2Handler.h>
#include <utils/xml/AttributesHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/convert/ToString.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include "ROEdge.h"
#include "RONet.h"
#include "ROWeightsHandler.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
ROWeightsHandler::ROWeightsHandler(OptionsCont &oc, RONet &net,
                                   const std::string &file)
    : SUMOSAXHandler("sumo-netweights", file), _options(oc), _net(net),
    _currentTimeBeg(-1), _currentTimeEnd(-1), _currentEdge(0)
{
    _scheme = _options.getString("scheme");
}


ROWeightsHandler::~ROWeightsHandler()
{
}


void ROWeightsHandler::myStartElement(int element, const std::string &name,
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
ROWeightsHandler::parseTimeStep(const Attributes &attrs)
{
    try {
        _currentTimeBeg = getLong(attrs, SUMO_ATTR_BEGIN);
        _currentTimeEnd = getLong(attrs, SUMO_ATTR_END);
    } catch (...) {
        MsgHandler::getErrorInstance()->inform("Problems with timestep value.");
    }
}


void
ROWeightsHandler::parseEdge(const Attributes &attrs)
{
    _currentEdge = 0;
    try {
        string id = getString(attrs, SUMO_ATTR_ID);
        _currentEdge = _net.getEdge(id);
        myAggValue = 0;
        myNoLanes = 0;
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("An edge without an id occured.");
        MsgHandler::getErrorInstance()->inform(" Contact your weight data supplier.");
    }
}


void
ROWeightsHandler::parseLane(const Attributes &attrs)
{
    string id;
    float value = -1;
    // try to get the lane id
    try {
        id = getString(attrs, SUMO_ATTR_ID);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("A lane without an id occured.");
        MsgHandler::getErrorInstance()->inform(" Contact your weight data supplier.");
    }
    // try to get the lane value - depending on the used scheme
    try {
        value = getFloat(attrs, SUMO_ATTR_VALUE);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(string("Missing value '")
            + _scheme + string("' in lane '") + id + string("'."));
        MsgHandler::getErrorInstance()->inform("Contact your weight data supplier.");
    } catch (NumberFormatException) {
        MsgHandler::getErrorInstance()->inform(string("The value should be numeric, but is not ('") +
            getString(attrs, SUMO_ATTR_VALUE) +
            string("'"));
        if(id.length()!=0)
            MsgHandler::getErrorInstance()->inform(string(" In lane '") + id
                + string("' at time step ") + toString<long>(_currentTimeBeg)
                + string("."));
    }
    // set the values when retrieved (no errors)
    if(id.length()!=0&&value>=0&&_currentEdge!=0) {
        myAggValue += value;
        myNoLanes++;
    }
}


void ROWeightsHandler::myCharacters(int element, const std::string &name,
                                    const std::string &chars)
{
}


void ROWeightsHandler::myEndElement(int element, const std::string &name)
{
    if(element==SUMO_TAG_EDGE) {
        _currentEdge->addWeight(myAggValue/(float)myNoLanes,
            _currentTimeBeg, _currentTimeEnd);
        _currentEdge = 0;
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


