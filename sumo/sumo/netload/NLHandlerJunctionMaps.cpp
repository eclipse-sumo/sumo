/***************************************************************************
                          NLHandlerJunctionMaps.cpp
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
// Revision 2.0  2002/02/14 14:43:24  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.1  2002/02/13 15:40:44  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
// Revision 1.1  2001/12/06 13:36:08  traffic
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
#include "NLHandlerJunctionMaps.h"
#include "SErrorHandler.h"
#include "../utils/XMLConvert.h"
#include "../utils/XMLBuildingExceptions.h"
#include "NLHandlerJunctionLogics.h"
#include "NLNetBuilder.h"
#include "NLTags.h"
#include "../microsim/MSMapLogic.h"

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * method definitions
 * ======================================================================= */
NLHandlerJunctionMaps::NLHandlerJunctionMaps(NLContainer *container) : 
  NLHandlerJunctionLogics(container)
{
}

NLHandlerJunctionMaps::~NLHandlerJunctionMaps() 
{
}

void NLHandlerJunctionMaps::init(string key, string file) {
  NLHandlerJunctionLogics::init(key, file);
   m_pActiveLogic = new MSMapLogic::Logic();
}

void 
NLHandlerJunctionMaps::startElement(const XMLCh* const name, AttributeList& attributes) 
{
  NLSAXHandler::startElement(name, attributes);
  string request = "";
  string respond = "";
  switch(convert(name)) {
  case NLTag_logicitem:
    try {
      request = XMLConvert::_2str(attributes.getValue("request"));
    } catch (XMLUngivenParameterException &e) {
      SErrorHandler::add("Missing request key...");
    }
    try {
      respond = XMLConvert::_2str(attributes.getValue("respond"));
    } catch (XMLUngivenParameterException &e) {
      SErrorHandler::add("Missing respond for the request '" + request+ "'");
    }
    if(request!="" && respond!="")
      addMapLogic(request.c_str(), respond.c_str());
  break;
  default:
    break;
  }
}

void 
NLHandlerJunctionMaps::characters(const XMLCh* const chars, const unsigned int length) 
{
  string key = "";
  switch(convert(m_LastName)) {
  case NLTag_key:
    key = XMLConvert::_2str(chars);
    if(key!=m_Key)
      SErrorHandler::add("Key mismatch in '" + m_File + "'!");
    break;
  default:
    break;
  }
}

void 
NLHandlerJunctionMaps::addMapLogic(const char *request, const char *respond) 
{
  vector<bool> *crequest = convertBoolVector(request);
  vector<bool> *crespond = convertBoolVector(respond);
  _links = crequest->size();
  _inlanes = crespond->size();
  if(NLNetBuilder::check) {
    MSMapLogic::Logic::iterator i1=m_pActiveLogic->find(*crequest);
    if(i1!=m_pActiveLogic->end())
      throw XMLKeyDuplicateException();
  }
  m_pActiveLogic->insert(MSMapLogic::Logic::value_type(*crequest, *crespond));
  delete crequest;
  delete crespond;
}

MSJunctionLogic *
NLHandlerJunctionMaps::build() {
  return new MSMapLogic(_links, _inlanes, m_pActiveLogic);
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NLHandlerJunctionMaps.icc"
//#endif

// Local Variables:
// mode:C++
// End:
