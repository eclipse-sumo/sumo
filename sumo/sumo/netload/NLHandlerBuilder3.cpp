/***************************************************************************
                          NLHandlerBuilder3.cpp
			  The fifth-step - handler building structures
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
// Revision 1.8  2002/06/21 10:53:49  dkrajzew
// inclusion of .cpp-files in .cpp files removed
//
// Revision 1.7  2002/06/11 14:39:26  dkrajzew
// windows eol removed
//
// Revision 1.6  2002/06/11 13:44:32  dkrajzew
// Windows eol removed
//
// Revision 1.5  2002/06/10 08:36:07  dkrajzew
// Conversion of strings generalized
//
// Revision 1.4  2002/06/07 14:39:58  dkrajzew
// errors occured while building larger nets and adaption of new netconverting methods debugged
//
// Revision 1.3  2002/04/17 11:17:49  dkrajzew
// windows-newlines removed
//
// Revision 1.2  2002/04/15 07:05:36  dkrajzew
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
#include "NLHandlerBuilder3.h"
#include "SErrorHandler.h"
#include "../utils/TplConvert.h"
#include "../utils/XMLBuildingExceptions.h"
#include "../utils/AttributesHandler.h"
#include "NLLoadFilter.h"
#include "NLNetBuilder.h"
#include "NLTags.h"

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * method definitions
 * ======================================================================= */
NLHandlerBuilder3::NLHandlerBuilder3(NLContainer &container, LoadFilter filter)
    : NLSAXHandler(container, filter)
{
    _attrHandler.add(ATTR_ID, "id");
    _attrHandler.add(ATTR_JUNCTION, "junction");
    _attrHandler.add(ATTR_YIELD, "yield");
    _attrHandler.add(ATTR_LANE, "lane");
}

NLHandlerBuilder3::~NLHandlerBuilder3()
{
}

void
NLHandlerBuilder3::myStartElement(int element, const std::string &name, const Attributes &attrs)
{
    if(wanted(LOADFILTER_NET)) {
        switch(element) {
        case NLTag_lane:
            openSuccLane(attrs);
            break;
        case NLTag_succ:
            setSuccJunction(attrs);
            break;
        case NLTag_succlane:
            addSuccLane(attrs);
            break;
        default:
            break;
        }
    }
}


void
NLHandlerBuilder3::openSuccLane(const Attributes &attrs) {
    try {
        string id = _attrHandler.getString(attrs, ATTR_ID);
        myContainer.openSuccLane(id);
        m_LaneId = id;
    } catch (EmptyData &e) {
        SErrorHandler::add("Error in description: missing id of a succ-object.");
    }
}


void
NLHandlerBuilder3::setSuccJunction(const Attributes &attrs) {
    try {
        myContainer.setSuccJunction(_attrHandler.getString(attrs, ATTR_JUNCTION));
    } catch (EmptyData &e) {
        SErrorHandler::add("Error in description: missing id of a succ junction-object.");
    }
}

void
NLHandlerBuilder3::addSuccLane(const Attributes &attrs) {
    try {
        myContainer.addSuccLane(
            _attrHandler.getBool(attrs, ATTR_YIELD),
            _attrHandler.getString(attrs, ATTR_LANE));
    } catch (EmptyData &e) {
        SErrorHandler::add("Error in description: missing attribute in a succlane-object.");
    } catch (XMLIdNotKnownException &e) {
        SErrorHandler::add(e.getMessage("", ""));
        SErrorHandler::add(string(" While building lane '")+myContainer.getSuccingLaneName()+string("'"));
    }
}


void
NLHandlerBuilder3::myEndElement(int element, const std::string &name)
{
    if(wanted(LOADFILTER_NET)) {
        switch(element) {
        case NLTag_succ:
            closeSuccLane();
            break;
        default:
            break;
        }
    }
}

void
NLHandlerBuilder3::closeSuccLane() {
    try {
        myContainer.closeSuccLane();
    } catch (XMLIdNotKnownException &e) {
        SErrorHandler::add(e.getMessage("", ""));
    }
}

std::string
NLHandlerBuilder3::getMessage() const {
    return "Loading street connections...";
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NLHandlerBuilder3.icc"
//#endif

// Local Variables:
// mode:C++
// End:
