//---------------------------------------------------------------------------//
//                        GUINetHandler.cpp -
//  The XML-Handler for building networks within the gui-version derived
//      from NLNetHandler
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
// Revision 1.1  2003/02/07 10:38:19  dkrajzew
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
#include <iostream>
#include <sax/HandlerBase.hpp>
#include <sax/AttributeList.hpp>
#include <sax/SAXParseException.hpp>
#include <sax/SAXException.hpp>
#include <netload/NLNetHandler.h>
#include <netload/NLContainer.h>
#include <utils/common/SErrorHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include "GUIContainer.h"
#include "GUINetHandler.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUINetHandler::GUINetHandler(bool verbose, bool warn, const std::string &file,
                             NLContainer &container)
    : NLNetHandler(verbose, warn, file, container)
{
}


GUINetHandler::~GUINetHandler()
{
}


void
GUINetHandler::myStartElement(int element, const std::string &name,
                                  const Attributes &attrs)
{
    NLNetHandler::myStartElement(element, name, attrs);
    if(wanted(LOADFILTER_NET) && element==SUMO_TAG_EDGEPOS) {
        addSourceDestinationInformation(attrs);
    }
}


void
GUINetHandler::addSourceDestinationInformation(const Attributes &attrs) {
    try {
        string id = getString(attrs, SUMO_ATTR_ID);
        cout << id << endl;
        string from = getString(attrs, SUMO_ATTR_FROM);
        string to = getString(attrs, SUMO_ATTR_TO);
        static_cast<GUIContainer&>(myContainer).addSrcDestInfo(id, from, to);
    } catch (EmptyData) {
        SErrorHandler::add(
            "Error in description: An edge has no information about the from/to-node");
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUINetHandler.icc"
//#endif

// Local Variables:
// mode:C++
// End:







