/***************************************************************************
                          NLHandlerCounter.cpp
			  The first-step - handler which counts the given
			  structures
                             -------------------
    project              : SUMO
    begin                : Mon, 9 Jul 2001
    copyright            : (C) 2001 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
namespace
{
     const char rcsid[] = "$Id$";
}
// $Log$
// Revision 1.5  2002/06/11 14:39:26  dkrajzew
// windows eol removed
//
// Revision 1.4  2002/06/11 13:44:32  dkrajzew
// Windows eol removed
//
// Revision 1.3  2002/04/17 11:17:49  dkrajzew
// windows-newlines removed
//
// Revision 1.2  2002/04/15 07:07:56  dkrajzew
// new loading paradigm implemented
//
// Revision 1.1.1.1  2002/04/08 07:21:24  traffic
// new project name
//
// Revision 2.0  2002/02/14 14:43:22  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.3  2002/02/13 15:40:42  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
// Revision 1.1  2001/12/06 13:36:07  traffic
// moved from netbuild
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <sax/HandlerBase.hpp>
#include <sax/AttributeList.hpp>
#include <sax/SAXParseException.hpp>
#include <sax/SAXException.hpp>
#include "NLContainer.h"
#include "NLHandlerCounter.h"
#include "NLSAXHandler.h"
#include "NLTags.h"
#include "NLLoadFilter.h"

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * method definitions
 * ======================================================================= */
NLHandlerCounter::NLHandlerCounter(NLContainer &container, LoadFilter filter)
    : NLSAXHandler(container, filter)
{
}

/// standard destructor
NLHandlerCounter::~NLHandlerCounter()
{
}

void
NLHandlerCounter::myStartElement(int element, const std::string &name, const Attributes &attrs)
{
    // process static net parts when wished
    if(wanted(LOADFILTER_NET)) {
        switch(element) {
        case NLTag_edge:
            myContainer.incEdges();
            break;
        case NLTag_lane:
            myContainer.incLanes();
            break;
        case NLTag_junction:
            myContainer.incJunctions();
            break;
        case NLTag_detector:
            myContainer.incDetectors();
            break;
        default:
            break;
        }
    }
    // process dynamic net parts when wished
    if(wanted(LOADFILTER_DYNAMIC)) {
        switch(element) {
        case NLTag_vtype:
            myContainer.incVehicleTypes();
            break;
        case NLTag_vehicle:
            myContainer.incVehicles();
            break;
        case NLTag_route:
            myContainer.incRoutes();
            break;
        default:
            break;
        }
    }
}

std::string
NLHandlerCounter::getMessage() const {
    return "Counting structures...";
}

void
NLHandlerCounter::changeLoadFilter(LoadFilter filter) {
    _filter = filter;
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NLHandlerCounter.icc"
//#endif

// Local Variables:
// mode:C++
// End:
