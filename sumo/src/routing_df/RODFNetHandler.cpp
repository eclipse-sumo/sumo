//---------------------------------------------------------------------------//
//                        RODFNetHandler.cpp -
//  The handler for SUMO-Networks
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Mon, 27.03.2006
//  copyright            : (C) 2006 by Daniel Krajzewicz
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
// Revision 1.1  2006/03/28 06:17:18  dkrajzew
// extending the dfrouter by distance/length factors
//
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
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/UtilExceptions.h>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include <utils/geom/Position2DVector.h>
#include <utils/geom/GeomConvHelper.h>
#include "RODFNetHandler.h"
#include "RODFEdge.h"

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
RODFNetHandler::RODFNetHandler(OptionsCont &oc, RONet &net,
                           ROAbstractEdgeBuilder &eb)
    : RONetHandler(oc, net, eb)
{
}


RODFNetHandler::~RODFNetHandler()
{
}


void
RODFNetHandler::myCharacters(int element, const std::string&name,
                           const std::string &chars)
{
    RONetHandler::myCharacters(element, name, chars);
    switch(element) {
    case SUMO_TAG_LANE:
        {
            Position2DVector p = GeomConvHelper::parseShape(chars);
            static_cast<RODFEdge*>(_currentEdge)->setFromPosition(p[0]);
            static_cast<RODFEdge*>(_currentEdge)->setToPosition(p[-1]);
        }
        break;
    default:
        break;
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


