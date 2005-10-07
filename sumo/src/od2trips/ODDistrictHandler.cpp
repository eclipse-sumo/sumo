//---------------------------------------------------------------------------//
//                        ODDistrictHandler.cpp -
//  The XML-Handler for district loading
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
// Revision 1.7  2005/10/07 11:42:00  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.6  2005/09/23 06:04:23  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.5  2005/09/15 12:04:48  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/05/04 08:44:57  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.3  2003/06/18 11:20:24  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.2  2003/02/07 10:44:19  dkrajzew
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
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include <utility>
#include <iostream>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include "ODDistrict.h"
#include "ODDistrictCont.h"
#include "ODDistrictHandler.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG

using namespace std;

ODDistrictHandler::ODDistrictHandler(ODDistrictCont &cont)
    : SUMOSAXHandler("sumo-districts"),
    _cont(cont), _current(0)
{
}


ODDistrictHandler::~ODDistrictHandler()
{
}


void
ODDistrictHandler::myStartElement(int element, const std::string &name,
                                  const Attributes &attrs)
{
    switch(element) {
    case SUMO_TAG_DISTRICT:
        openDistrict(attrs);
        break;
    case SUMO_TAG_DSOURCE:
        addSource(attrs);
        break;
    case SUMO_TAG_DSINK:
        addSink(attrs);
        break;
    }
}


void
ODDistrictHandler::myEndElement(int element, const std::string &name)
{
    if(element==SUMO_TAG_DISTRICT) {
        closeDistrict();
    }
}


void
ODDistrictHandler::myCharacters(int element, const std::string &name,
                                const std::string &chars)
{
}


void
ODDistrictHandler::openDistrict(const Attributes &attrs)
{
    _current = 0;
    try {
        _current = new ODDistrict(getString(attrs, SUMO_ATTR_ID));
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("A district without an id occured.");
    }
}


void
ODDistrictHandler::addSource(const Attributes &attrs)
{
    std::pair<std::string, SUMOReal> vals = getValues(attrs, "source");
    if(vals.second>=0) {
        _current->addSource(vals.first, vals.second);
    }
}


void
ODDistrictHandler::addSink(const Attributes &attrs)
{
    std::pair<std::string, SUMOReal> vals = getValues(attrs, "sink");
    if(vals.second>=0) {
        _current->addSink(vals.first, vals.second);
    }
}



std::pair<std::string, SUMOReal>
ODDistrictHandler::getValues(const Attributes &attrs, const std::string &type)
{
    // check the current district first
    if(_current==0) {
        return std::pair<std::string, SUMOReal>("", -1);
    }
    // get the id first
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(
            string("A ") + type
            + string(" without an id occured within district '")
            + _current->getID() + string("'."));
        return std::pair<std::string, SUMOReal>("", -1);
    }
    // get the weight
    SUMOReal weight = getFloatSecure(attrs, SUMO_ATTR_WEIGHT, -1);
    if(weight==-1) {
        MsgHandler::getErrorInstance()->inform(
            string("The weight of the ") + type + string(" '") + id
            + string("' within district '")
            + _current->getID()
            + string("' is not numeric."));
        return std::pair<std::string, SUMOReal>("", -1);
    }
    // return the values
    return std::pair<std::string, SUMOReal>(id, weight);
}


void
ODDistrictHandler::closeDistrict()
{
    _cont.add(_current->getID(), _current);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


