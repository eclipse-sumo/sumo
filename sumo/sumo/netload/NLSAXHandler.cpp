/***************************************************************************
                          NLSAXHandler.cpp
			  Virtual handler that reacts on incoming tags; 
			  parent to the handlers that represent the parsing
			  steps
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
// Revision 2.1  2002/03/15 09:38:01  traffic
// New known tags (number of incoming lanes) included
//
// Revision 2.0  2002/02/14 14:43:25  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.4  2002/02/13 15:40:45  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
// Revision 1.1  2001/12/06 13:36:10  traffic
// moved from netbuild
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <strstream>
#include <sax/HandlerBase.hpp>
#include <sax/AttributeList.hpp>
#include <sax/SAXParseException.hpp>
#include <sax/SAXException.hpp>
#include "NLNetBuilder.h"
#include "NLContainer.h"
#include "../utils/XMLBuildingExceptions.h"
#include "NLSAXHandler.h"
#include "SErrorHandler.h"
#include "../utils/XMLConvert.h"
#include "NLTags.h"

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * method definitions
 * ======================================================================= */
NLSAXHandler::NLSAXHandler(NLContainer *container) : myContainer(container)
{
  m_Tags["simulation"] = NLTag_simulation;
  m_Tags["SIMULATION"] = NLTag_simulation;
  m_Tags["edge"] = NLTag_edge;
  m_Tags["EDGE"] = NLTag_edge;
  m_Tags["lane"] = NLTag_lane;
  m_Tags["LANE"] = NLTag_lane;
  m_Tags["lanes"] = NLTag_lanes;
  m_Tags["LANES"] = NLTag_lanes;
  m_Tags["CEDGE"] = NLTag_cedge;
  m_Tags["cedge"] = NLTag_cedge;
  m_Tags["junction"] = NLTag_junction;
  m_Tags["JUNCTION"] = NLTag_junction;
  m_Tags["inlanes"] = NLTag_inlane;
  m_Tags["INLANES"] = NLTag_inlane;
  m_Tags["logicitem"] = NLTag_logic;
  m_Tags["LOGICITEM"] = NLTag_logic;
  m_Tags["vehicle"] = NLTag_vehicle;
  m_Tags["VEHICLE"] = NLTag_vehicle;
  m_Tags["vtype"] = NLTag_vtype;
  m_Tags["VTYPE"] = NLTag_vtype;
  m_Tags["route"] = NLTag_route;
  m_Tags["ROUTE"] = NLTag_route;
  m_Tags["succ"] = NLTag_succ;
  m_Tags["SUCC"] = NLTag_succ;
  m_Tags["succlane"] = NLTag_succlane;
  m_Tags["SUCCLANE"] = NLTag_succlane;
  m_Tags["key"] = NLTag_key;
  m_Tags["KEY"] = NLTag_key;
  m_Tags["junctionlogic"] = NLTag_junctionlogic;
  m_Tags["JUNCTIONLOGIC"] = NLTag_junctionlogic;
  m_Tags["requestsize"] = NLTag_requestsize;
  m_Tags["REQUESTSIZE"] = NLTag_requestsize;
  m_Tags["responsesize"] = NLTag_responsesize;
  m_Tags["RESPONSESIZE"] = NLTag_responsesize;
  m_Tags["lanenumber"] = NLTag_lanenumber;
  m_Tags["LANENUMBER"] = NLTag_lanenumber;
  m_Tags["logicitem"] = NLTag_logicitem;
  m_Tags["LOGICTITEM"] = NLTag_logicitem;
  m_Tags["trafoitem"] = NLTag_trafoitem;
  m_Tags["TRAFOITEM"] = NLTag_trafoitem;
}

NLSAXHandler::~NLSAXHandler() 
{
}

NLTag 
NLSAXHandler::convert(const XMLCh* const name) 
{
  string str = XMLConvert::_2str(name);
  NLTag ret = m_Tags[str];
  return ret;
}

NLTag 
NLSAXHandler::convert(const string name) 
{
  NLTag ret = m_Tags[name];
  return ret;
}

void 
NLSAXHandler::startElement(const XMLCh* const name, AttributeList& attributes) 
{
  if(NLNetBuilder::check) {
    m_LastItem = XMLConvert::_2str(name);
    m_LastName = m_LastItem;
    int size=attributes.getLength();
    for(int i=0; i<size; i++) {
      string name, value;
      try {
      	name = XMLConvert::_2str(attributes.getName(i));
      	value = XMLConvert::_2str(attributes.getValue(i));
      } catch (XMLUngivenParameterException &e) {
      }
      m_LastItem = m_LastItem + name + "='" + value + "' ";
    }
  }
}

void 
NLSAXHandler::characters(const XMLCh* const chars, const unsigned int length) 
{
}

void 
NLSAXHandler::resetDocument() 
{
}

void 
NLSAXHandler::endDocument() 
{
}

void 
NLSAXHandler::endElement(const XMLCh* const name) 
{
  m_LastName = "";
}

void 
NLSAXHandler::processingInstruction(const   XMLCh* const    target, const XMLCh* const    data) 
{
}

void 
NLSAXHandler::warning(const SAXParseException& exception) 
{
    ostrstream buf;
    buf << "XML-Warning:" + XMLConvert::_2str(exception.getMessage()) << endl; 
    if(NLNetBuilder::check) {
      buf << " (At line/column " << exception.getLineNumber()+1 << '_' << exception.getColumnNumber();
      buf << ", near: " << m_LastItem << ')';
    }  
    SErrorHandler::add(buf.str());
}

void 
NLSAXHandler::error(const SAXParseException& exception) 
{
    ostrstream buf;
    buf << "XML-Error:" << XMLConvert::_2str(exception.getMessage()) << endl;
    if(NLNetBuilder::check) {
      buf << " (At line/column " << exception.getLineNumber()+1 << '_' << exception.getColumnNumber();
      buf << ", near: " << m_LastItem << ')';    
    }  
    SErrorHandler::add(buf.str());
}

void 
NLSAXHandler::fatalError(const SAXParseException& exception) 
{
    ostrstream buf;
    buf << "XML-Fatal Error:" << XMLConvert::_2str(exception.getMessage()) << endl;
    if(NLNetBuilder::check) {
      buf << " (At line/column " <<  exception.getLineNumber()+1 << '/' << exception.getColumnNumber();
      buf << ", near: " << m_LastItem << ')' << ends;
    }  
    SErrorHandler::add(buf.str());
    SErrorHandler::setFatal();
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NLSAXHandler.icc"
//#endif

// Local Variables:
// mode:C++
// End:



