/***************************************************************************
                          NLHandlerJunctionBitsets.cpp
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
// Revision 2.3  2002/03/20 08:17:55  dkrajzew
// more proper usage of the SAX-API
//
// Revision 2.2  2002/03/15 09:34:12  traffic
// Loading of logics debugged (trafo-matrix is now loaded correctly)
//
// Revision 2.1  2002/02/21 19:49:09  croessel
// MSVC++ Linking-problems solved, hopefully.
//
// Revision 2.0  2002/02/14 14:43:23  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.2  2002/02/13 16:32:17  croessel
// Changed MSBitSet to predefined type MSBitset.
//
// Revision 1.1  2002/02/13 15:40:43  croessel
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
#include <bitset>
#include <sax/HandlerBase.hpp>
#include <sax/AttributeList.hpp>
#include <sax/SAXParseException.hpp>
#include <sax/SAXException.hpp>
#include "NLContainer.h"
#include "NLHandlerJunctionBitsets.h"
#include "SErrorHandler.h"
#include "../utils/XMLConvert.h"
#include "../utils/XMLBuildingExceptions.h"
#include "NLHandlerJunctionLogics.h"
#include "NLNetBuilder.h"
#include "NLTags.h"
#include "../microsim/MSBitSetLogic.h"

//#ifdef EXTERNAL_TEMPLATE_DEFINITION
#include "../microsim/MSBitSetLogic.cpp"
//#endif // EXTERNAL_TEMPLATE_DEFINITION

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * method definitions
 * ======================================================================= */
NLHandlerJunctionBitsets::NLHandlerJunctionBitsets(NLContainer *container) : 
  NLHandlerJunctionLogics(container)
{
}

NLHandlerJunctionBitsets::~NLHandlerJunctionBitsets() 
{
}

void NLHandlerJunctionBitsets::init(string key, string file) {
  NLHandlerJunctionLogics::init(key, file);
  m_pActiveLogic = new MSBitsetLogic::Logic();
  m_pActiveTrafo = new MSBitsetLogic::Link2LaneTrafo();
  _requestSize = -1;
  _responseSize = -1;
  _laneNo = -1;
  _trafoItems = 0;
  _requestItems = 0;
}

void 
NLHandlerJunctionBitsets::startElement(const XMLCh* const name, AttributeList& attributes) 
{
  NLSAXHandler::startElement(name, attributes);
  switch(convert(name)) {
  case NLTag_logicitem:
    if(_responseSize>0&&_requestSize>0) {
      int request = -1; 
      string response;
      try {
        request = XMLConvert::_2int(attributes.getValue("request"));
      } catch (XMLUngivenParameterException &e) {
        SErrorHandler::add("Missing request key...");
      } catch (XMLNumericFormatException e) {
        SErrorHandler::add("The request key is not numeric.");
      }
      try {
        response = XMLConvert::_2str(attributes.getValue("response"));
      } catch (XMLUngivenParameterException &e) {
        SErrorHandler::add("Missing respond for a request");
      }
      if(request>=0 && response.length()>0)
        addLogicItem(request, response);
    } else {
      SErrorHandler::add("The request size,  the response size or the number of lanes is not given! Contact your net supplier");
    }
    break;
  case NLTag_trafoitem:
    {
      int lane = -1; 
      string links;
      try {
        lane = XMLConvert::_2int(attributes.getValue("to"));
      } catch (XMLUngivenParameterException &e) {
        SErrorHandler::add("Missing lane number...");
      } catch (XMLNumericFormatException e) {
        SErrorHandler::add("The lane number is not numeric.");
      }
      try {
        links = XMLConvert::_2str(attributes.getValue("from"));
      } catch (XMLUngivenParameterException &e) {
        SErrorHandler::add("Missing links in a lane transformation.");
      }
      if(lane>=0 && links.length()>0)
        addTrafoItem(links, lane);
    }
  default:
    break;
  }
}

void 
NLHandlerJunctionBitsets::characters(const XMLCh* const chars, const unsigned int length) 
{
    string key = "";
    switch(convert(m_LastName)) {
    case NLTag_requestsize:
        try {
            _requestSize = XMLConvert::_2int(chars, length);
            m_pActiveLogic->reserve(_requestSize);
        } catch (XMLUngivenParameterException &e) {
            SErrorHandler::add("Missing request size.");
        } catch (XMLNumericFormatException e) {
            SErrorHandler::add("request size is not numeric! Contact your netconvert-programmer.");
        }
        break;
    case NLTag_responsesize:
        try {
            _responseSize = XMLConvert::_2int(chars, length);
            m_pActiveTrafo->reserve(_responseSize);
        } catch (XMLUngivenParameterException &e) {
            SErrorHandler::add("Missing response size.");
        } catch (XMLNumericFormatException e) {
            SErrorHandler::add("Response size is not numeric! Contact your netconvert-programmer.");
        }
        break;
    case NLTag_lanenumber:
        try {
            _laneNo = XMLConvert::_2int(chars, length);
            m_pActiveTrafo->reserve(_responseSize);
        } catch (XMLUngivenParameterException &e) {
            SErrorHandler::add("Missing lane number.");
        } catch (XMLNumericFormatException e) {
            SErrorHandler::add("Lane number is not numeric! Contact your netconvert-programmer.");
        }
        break;
    case NLTag_key:
        break;
    default:
        break;
    }
}

void 
NLHandlerJunctionBitsets::addLogicItem(int request, string response) {
  bitset<64> use(response);
  (*m_pActiveLogic)[request] = use;
  _requestItems++;
}

void 
NLHandlerJunctionBitsets::addTrafoItem(string links, int lane) {
  bitset<64> use(links);
  (*m_pActiveTrafo)[lane] = use;
  _trafoItems++;
}

MSJunctionLogic *
NLHandlerJunctionBitsets::build() {
  if(_trafoItems!=_laneNo||_requestItems!=_requestSize) {
    SErrorHandler::add(
	string("The description for the junction logic '") +
	m_Key +
	string("' is malicious."));
    return 0;
  }
  return new MSBitsetLogic(_requestSize, _laneNo, m_pActiveLogic, m_pActiveTrafo); 
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NLHandlerJunctionBitsets.icc"
//#endif

// Local Variables:
// mode:C++
// End:
