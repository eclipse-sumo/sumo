/***************************************************************************
                          NLHandlerBuilder1.cpp
			  The third-step - handler building structures
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
// Revision 2.0  2002/02/14 14:43:21  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.4  2002/02/13 15:40:41  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
// Revision 1.1  2001/12/06 13:36:05  traffic
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
#include "NLHandlerBuilder1.h"
#include "SErrorHandler.h"
#include "../utils/XMLConvert.h"
#include "../utils/XMLBuildingExceptions.h"
#include "../utils/StringTokenizer.h"
#include "NLSAXHandler.h"
#include "NLNetBuilder.h"
#include "NLTags.h"

/* =========================================================================
 * using namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * method definitions
 * ======================================================================= */
NLHandlerBuilder1::NLHandlerBuilder1(NLContainer *container, bool dynamicOnly) : NLSAXHandler(container), m_bDynamicOnly(dynamicOnly) {
}

NLHandlerBuilder1::~NLHandlerBuilder1() 
{
}

void 
NLHandlerBuilder1::startElement(const XMLCh* const name, AttributeList& attributes) 
{
  NLSAXHandler::startElement(name, attributes);
  string id = "";
  switch(convert(name)) {
  case NLTag_edge:
    if(!m_bDynamicOnly) {
      try {
        id = XMLConvert::_2str(attributes.getValue("id"));
        myContainer->chooseEdge(id);
      } catch (XMLUngivenParameterException &e) {
        SErrorHandler::add(e.getMessage("edge", "(ID_UNKNOWN!)"));
      } catch (XMLIdNotKnownException &e) {
        SErrorHandler::add(e.getMessage("edge", id));
      }
    }
    break;
  case NLTag_lane:
    if(!m_bDynamicOnly) {
      try {
        id = XMLConvert::_2str(attributes.getValue("id"));
        try {
          myContainer->addLane(id, 
            XMLConvert::_2bool(attributes.getValue("depart")), 
            XMLConvert::_2float(attributes.getValue("maxspeed")), 
            XMLConvert::_2float(attributes.getValue("length")), 
            XMLConvert::_2float(attributes.getValue("changeurge")));
        } catch (XMLInvalidParentException &e) {
          SErrorHandler::add(e.getMessage("lane", id));
        } catch (XMLIdAlreadyUsedException &e) {
          SErrorHandler::add(e.getMessage("lane", id));
        } catch (XMLDepartLaneDuplicationException &e) {
          SErrorHandler::add(e.getMessage("lane", id));
        } catch (XMLUngivenParameterException &e) {
          SErrorHandler::add(e.getMessage("edge", id));
        } catch (XMLNumericFormatException &e) {
          SErrorHandler::add(e.getMessage("edge", id));
        }
      } catch (XMLUngivenParameterException &e) {
        SErrorHandler::add(e.getMessage("edge", "(ID_UNKNOWN!)"));
      }
    }
    break;
  case NLTag_cedge:
    if(!m_bDynamicOnly) {
      try {
        id = XMLConvert::_2str(attributes.getValue("id"));
        myContainer->openAllowedEdge(id);
      } catch (XMLIdNotKnownException &e) {
        SErrorHandler::add(e.getMessage("cedge", id));
      } catch (XMLUngivenParameterException &e) {
        SErrorHandler::add(e.getMessage("edge", "(ID_UNKNOWN!)"));
      }
    }
    break;
  case NLTag_vtype:
    {
      try {
        id = XMLConvert::_2str(attributes.getValue("id"));
        try {
          myContainer->addVehicleType(id,
            XMLConvert::_2float(attributes.getValue("length")),
            XMLConvert::_2float(attributes.getValue("maxspeed")),
            XMLConvert::_2float(attributes.getValue("bmax")),
            XMLConvert::_2float(attributes.getValue("dmax")),
            XMLConvert::_2float(attributes.getValue("sigma")));
        } catch (XMLIdAlreadyUsedException &e) {
          SErrorHandler::add(e.getMessage("vehicletype", id));
        } catch (XMLUngivenParameterException &e) {
          SErrorHandler::add(e.getMessage("vehicletype", id));
        } catch (XMLNumericFormatException &e) {
          SErrorHandler::add(e.getMessage("edge", id));
        }
      } catch (XMLUngivenParameterException &e) {
        SErrorHandler::add(e.getMessage("vehicletype", "(ID_UNKNOWN!)"));
      }
    }
    break;
  case NLTag_route:
    {
      try {
        id = XMLConvert::_2str(attributes.getValue("id"));
        myContainer->openRoute(id);
      } catch (XMLUngivenParameterException &e) {
        SErrorHandler::add(e.getMessage("route", id));
      } catch (XMLIdNotKnownException &e) {
        SErrorHandler::add(e.getMessage("route", "(ID_UNKNOWN!)"));
      }
    }
    break;
  case NLTag_junction:
    {
      try {
        string key = XMLConvert::_2str(attributes.getValue("key"));
        myContainer->addKey(key);
      } catch (XMLUngivenParameterException &e) {
	SErrorHandler::add(e.getMessage("key", id));
      }
    }
  default:
    break;
  }
}

void 
NLHandlerBuilder1::endElement(const XMLCh* const name) 
{
  switch(convert(name)) {
  case NLTag_edge:
    if(!m_bDynamicOnly)
      myContainer->closeEdge();
    break;
  case NLTag_lanes:
    if(!m_bDynamicOnly) 
      myContainer->closeLanes();
  case NLTag_cedge:
    if(!m_bDynamicOnly)
      myContainer->closeAllowedEdge();
    break;
  case NLTag_route:
    try {
      myContainer->closeRoute();
    } catch (XMLListEmptyException &e) {
      SErrorHandler::add(e.getMessage("route", ""));
    } catch (XMLIdAlreadyUsedException &e) {
      SErrorHandler::add(e.getMessage("route", ""));
    } 
    break;
  default:
    break;
  }
  NLSAXHandler::endElement(name);
}


void 
NLHandlerBuilder1::characters(const XMLCh* const chars, const unsigned int length) 
{
    switch(convert(m_LastName)) {
    case NLTag_cedge:
        if(!m_bDynamicOnly) {
            StringTokenizer st(XMLConvert::_2str(chars, length));
            while(st.hasNext()) {
                string set = st.next();
                try {
                    myContainer->addAllowed(set);
                } catch (XMLIdNotKnownException &e) {
	                SErrorHandler::add(e.getMessage("clane", set));
                } catch (XMLInvalidChildException &e) {
	                SErrorHandler::add(e.getMessage("clane", set));
                }
            }
        }
        break;
    case NLTag_route:
        {
            StringTokenizer st(XMLConvert::_2str(chars, length));
            if(NLNetBuilder::check&&st.size()==0) 
                SErrorHandler::add("Empty route (" + m_LastItem + ")");
            else {
                while(st.hasNext()) {
                    string set = st.next();
                    try {
	                    myContainer->addRoutesEdge(set);
                    } catch (XMLIdNotKnownException &e) {
	                    SErrorHandler::add(e.getMessage("routes edge", ""));
                    }
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
//#include "NLHandlerBuilder1.icc"
//#endif

// Local Variables:
// mode:C++
// End:
