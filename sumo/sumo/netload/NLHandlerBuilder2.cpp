/***************************************************************************
                          NLHandlerBuilder2.cpp
			  The fourth-step - handler building remaining 
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
// Revision 1.1  2002/04/08 07:21:24  traffic
// Initial revision
//
// Revision 2.1  2002/03/20 08:16:54  dkrajzew
// strtok replaced by a StringTokenizer; NumericFormatException handling added
//
// Revision 2.0  2002/02/14 14:43:22  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.4  2002/02/13 15:40:42  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
// Revision 1.1  2001/12/06 13:36:06  traffic
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
#include "NLHandlerBuilder2.h"
#include "SErrorHandler.h"
#include "../utils/XMLConvert.h"
#include "../utils/XMLBuildingExceptions.h"
#include "../utils/StringTokenizer.h"
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
NLHandlerBuilder2::NLHandlerBuilder2(NLContainer *container, bool vehiclesOnly) : NLSAXHandler(container), m_bDynamicOnly(vehiclesOnly) {
}

NLHandlerBuilder2::~NLHandlerBuilder2() 
{
}

void 
NLHandlerBuilder2::startElement(const XMLCh* const name, AttributeList& attributes) 
{
  NLSAXHandler::startElement(name, attributes);
  string id = "";
  switch(convert(name)) {
  case NLTag_junction:
    if(!m_bDynamicOnly) {
      try {
        id = XMLConvert::_2str(attributes.getValue("id"));
	      try {
	        string key = XMLConvert::_2str(attributes.getValue("key"));
          string type = XMLConvert::_2str(attributes.getValue("type"));
    	    myContainer->openJunction(id, key, type);
        } catch (XMLUngivenParameterException &e) {
          SErrorHandler::add(e.getMessage("junction", id));
        }
      } catch (XMLUngivenParameterException &e) {
        SErrorHandler::add(e.getMessage("junction", "(ID_UNKNOWN!)"));
      }
    }
    break;
/*  case NLTag_logic:
    if(!m_bDynamicOnly) {
      try {
	      char *request = XMLConvert::_2char(attributes.getValue("request"));
	      char *respond = XMLConvert::_2char(attributes.getValue("respond"));
	      myContainer->addLogic(request, respond);
      } catch (XMLKeyDuplicateException &e) {
	      SErrorHandler::add(e.getMessage("logic", ""));
      } catch (XMLUngivenParameterException &e) {
		    SErrorHandler::add(e.getMessage("edge", id));
	    }
    }
    break;*/
  case NLTag_vehicle:
    {
      try {
         id = XMLConvert::_2str(attributes.getValue("id"));
         try {
            myContainer->addVehicle(id, 
               XMLConvert::_2str(attributes.getValue("type")),
               XMLConvert::_2str(attributes.getValue("route")),
               XMLConvert::_2long(attributes.getValue("depart")));		
         } catch (XMLUngivenParameterException &e) {
            SErrorHandler::add(e.getMessage("vehicle", id));
         } catch(XMLIdNotKnownException &e) {
            SErrorHandler::add(e.getMessage("", ""));
         } catch(XMLIdAlreadyUsedException &e) {
            SErrorHandler::add(e.getMessage("vehicle", id));
         }  
      } catch(XMLIdAlreadyUsedException &e) {
         SErrorHandler::add(e.getMessage("vehicle", "(ID_UNKNOWN!)"));
      }  
    }
    break;
  default:
    break;
  }
}

void 
NLHandlerBuilder2::endElement(const XMLCh* const name) 
{
  switch(convert(name)) {
  case NLTag_junction:
    if(!m_bDynamicOnly) {
      try {
	myContainer->closeJunction();
      } catch (XMLIdAlreadyUsedException &e) {
	SErrorHandler::add(e.getMessage("junction", ""));
      } catch (XMLIdNotKnownException &e) {
        SErrorHandler::add(e.getMessage("junction", ""));
      }
    }
    break;
  default:
    break;
  }
  NLSAXHandler::endElement(name);
}

void 
NLHandlerBuilder2::characters(const XMLCh* const chars, const unsigned int length) 
{
    switch(convert(m_LastName)) {
    case NLTag_inlane:
        if(!m_bDynamicOnly) {
            StringTokenizer st(XMLConvert::_2str(chars, length));
            while(st.hasNext()) {
                string set = st.next();
                try {
	                myContainer->addInLane(set);
	            } catch (XMLIdNotKnownException &e) {
	                SErrorHandler::add(e.getMessage("lane", set));
	            }
            }
        }
        break;
    default:
        break;
    }
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NLHandlerBuilder2.icc"
//#endif

// Local Variables:
// mode:C++
// End:
