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
// Revision 1.4  2002/04/24 10:38:22  dkrajzew
// False usage of strstream in error report changed into a corrected usage of stringstream
//
// Revision 1.3  2002/04/17 11:18:47  dkrajzew
// windows-newlines removed
//
// Revision 1.2  2002/04/15 07:07:56  dkrajzew
// new loading paradigm implemented
//
// Revision 1.1.1.1  2002/04/08 07:21:24  traffic
// new project name
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
#include <sstream>
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
#include "NLLoadFilter.h"

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


GenericSAX2Handler::Tag NLSAXHandler::_tags[21] =
{
/* 00 */  { "simulation", NLTag_simulation },
/* 01 */  { "edge", NLTag_edge },
/* 02 */  { "lane", NLTag_lane },
/* 03 */  { "lanes", NLTag_lanes },
/* 04 */  { "cedge", NLTag_cedge },
/* 05 */  { "junction", NLTag_junction },
/* 06 */  { "inlanes", NLTag_inlane },
/* 07 */  { "detector", NLTag_detector },
/* 08 */  { "vehicle", NLTag_vehicle },
/* 09 */  { "vtype", NLTag_vtype },
/* 10 */  { "route", NLTag_route },
/* 11 */  { "succ", NLTag_succ },
/* 12 */  { "succlane", NLTag_succlane },
/* 13 */  { "key", NLTag_key },
/* 14 */  { "junctionlogic", NLTag_junctionlogic },
/* 15 */  { "requestsize", NLTag_requestsize },
/* 16 */  { "responsesize", NLTag_responsesize },
/* 17 */  { "lanenumber", NLTag_lanenumber },
/* 18 */  { "logicitem", NLTag_logicitem },
/* 19 */  { "trafoitem", NLTag_trafoitem },
/* 20 */  { "bitsetlogic", NLTag_bitsetlogic },
};

/* =========================================================================
 * method definitions
 * ======================================================================= */
NLSAXHandler::NLSAXHandler(NLContainer &container, LoadFilter filter)
    : GenericSAX2Handler(_tags, 21), myContainer(container), _filter(filter)
{
}

NLSAXHandler::~NLSAXHandler()
{
}

void
NLSAXHandler::warning(const SAXParseException& exception)
{
    ostringstream buf;
    buf << "XML-Warning:" + XMLConvert::_2str(exception.getMessage()) << endl;
/*    if(NLNetBuilder::check) {*/
      buf << " (At line/column " << exception.getLineNumber()+1 << '_'
          << exception.getColumnNumber() << ')';
/*    }  */
    SErrorHandler::add(buf.str(), true);
    GenericSAX2Handler::warning(exception);
}

void
NLSAXHandler::error(const SAXParseException& exception)
{
    ostringstream buf;
    buf << "XML-Error:" << XMLConvert::_2str(exception.getMessage()) << endl;
/*    if(NLNetBuilder::check) {*/
      buf << " (At line/column " << exception.getLineNumber()+1 << '_'
          << exception.getColumnNumber() << ')';
/*    }  */
    SErrorHandler::add(buf.str(), true);
    GenericSAX2Handler::error(exception);
}

void
NLSAXHandler::fatalError(const SAXParseException& exception)
{
    ostringstream buf;
    buf << "XML-Fatal Error:" << XMLConvert::_2str(exception.getMessage()) << endl;
/*    if(NLNetBuilder::check) {*/
      buf << " (At line/column " <<  exception.getLineNumber()+1 << '/' <<
          exception.getColumnNumber() << ')';
/*    }  */
    SErrorHandler::add(buf.str(), true);
    SErrorHandler::setFatal();
    GenericSAX2Handler::fatalError(exception);
}

void
NLSAXHandler::myStartElement(int element, const std::string &name, const Attributes &attrs) {
}

void
NLSAXHandler::myEndElement(int element, const std::string &name) {
}

void
NLSAXHandler::myCharacters(int element, const std::string &name, const std::string &chars) {
}

bool
NLSAXHandler::wanted(LoadFilter filter) const {
    return (_filter&filter)!=0;
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NLSAXHandler.icc"
//#endif

// Local Variables:
// mode:C++
// End:



