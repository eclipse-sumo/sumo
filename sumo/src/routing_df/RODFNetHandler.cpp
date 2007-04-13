/****************************************************************************/
/// @file    RODFNetHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 27.03.2006
/// @version $Id$
///
// The handler for SUMO-Networks
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
#include <utils/options/OptionsCont.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/UtilExceptions.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/geom/Position2DVector.h>
#include <utils/geom/GeomConvHelper.h>
#include "RODFNetHandler.h"
#include "RODFEdge.h"

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
RODFNetHandler::RODFNetHandler(OptionsCont &oc, RONet &net,
                               ROAbstractEdgeBuilder &eb)
        : RONetHandler(oc, net, eb)
{}


RODFNetHandler::~RODFNetHandler()
{}


void
RODFNetHandler::myCharacters(int element, const std::string&name,
                             const std::string &chars)
{
    RONetHandler::myCharacters(element, name, chars);
    switch (element) {
    case SUMO_TAG_LANE:
        // may be an unparsed internal lane
        if(_currentEdge!=0) {
            Position2DVector p = GeomConvHelper::parseShape(chars);
            static_cast<RODFEdge*>(_currentEdge)->setFromPosition(p[0]);
            static_cast<RODFEdge*>(_currentEdge)->setToPosition(p[-1]);
        }
        break;
    default:
        break;
    }
}



/****************************************************************************/

