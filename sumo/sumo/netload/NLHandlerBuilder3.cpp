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
// Revision 1.1  2002/04/08 07:21:24  traffic
// Initial revision
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
#include "../utils/XMLConvert.h"
#include "../utils/XMLBuildingExceptions.h"
#include "NLSAXHandler.h"
#include "NLNetBuilder.h"
#include "NLTags.h"

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * method definitions
 * ======================================================================= */
NLHandlerBuilder3::NLHandlerBuilder3(NLContainer *container, bool dynamicOnly) : NLSAXHandler(container), m_bDynamicOnly(dynamicOnly) {
}

NLHandlerBuilder3::~NLHandlerBuilder3() 
{
}

void 
NLHandlerBuilder3::startElement(const XMLCh* const name, AttributeList& attributes) 
{
  NLSAXHandler::startElement(name, attributes);
  switch(convert(name)) {
  case NLTag_lane:
    try {
      string id = XMLConvert::_2str(attributes.getValue("id")); 
      myContainer->openSuccLane(id);
      m_LaneId = id;
    } catch (XMLUngivenParameterException &e) {
      SErrorHandler::add(e.getMessage("succ", "(ID_UNKNOWN!)"));
    }
    break;
  case NLTag_succ:
    try {
      myContainer->setSuccJunction(
				   XMLConvert::_2str(attributes.getValue("junction")));
    } catch (XMLUngivenParameterException &e) {
      SErrorHandler::add(e.getMessage("edge", m_LaneId));
    }
    break;
  case NLTag_succlane:
    try {
      myContainer->addSuccLane(
			       XMLConvert::_2bool(attributes.getValue("yield")),
			       XMLConvert::_2str(attributes.getValue("lane")));
    } catch (XMLUngivenParameterException &e) {
      SErrorHandler::add(e.getMessage("edge", m_LaneId));
    } catch (XMLIdNotKnownException &e) {
      SErrorHandler::add(e.getMessage("", ""));
    }
  default:
    break;
  }
}

void 
NLHandlerBuilder3::endElement(const XMLCh* const name) 
{
  switch(convert(name)) {
  case NLTag_succ:
    try {
      myContainer->closeSuccLane();
    } catch (XMLIdNotKnownException &e) {
      SErrorHandler::add(e.getMessage("", ""));
    }
  default:
    break;
  }
  NLSAXHandler::endElement(name);
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NLHandlerBuilder3.icc"
//#endif

// Local Variables:
// mode:C++
// End:
