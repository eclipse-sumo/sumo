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
// Revision 1.1  2002/10/16 15:36:50  dkrajzew
// moved from ROOT/sumo/netload to ROOT/src/netload; new format definition parseable in one step
//
// Revision 1.10  2002/07/31 17:34:51  roessel
// Changes since sourceforge cvs request.
//
// Revision 1.10  2002/07/22 12:44:32  dkrajzew
// Source loading structures added
//
// Revision 1.9  2002/06/21 10:53:49  dkrajzew
// inclusion of .cpp-files in .cpp files removed
//
// Revision 1.8  2002/06/11 14:39:27  dkrajzew
// windows eol removed
//
// Revision 1.7  2002/06/11 13:44:33  dkrajzew
// Windows eol removed
//
// Revision 1.6  2002/06/10 08:36:07  dkrajzew
// Conversion of strings generalized
//
// Revision 1.5  2002/05/14 04:54:25  dkrajzew
// Unexisting files are now catched independent to the Xerces-error mechanism; error report generation moved to XMLConvert
//
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
#include <utils/xml/XMLBuildingExceptions.h>
#include "NLSAXHandler.h"
#include <utils/common/SErrorHandler.h>
#include <utils/convert/TplConvert.h>
#include "NLTags.h"
#include "NLLoadFilter.h"

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


GenericSAX2Handler::Tag NLSAXHandler::_tags[34] =
{
    /* 00 */    { "simulation", NLTag_simulation },
    /* 01 */    { "edge", NLTag_edge },
    /* 02 */    { "lane", NLTag_lane },
    /* 03 */    { "lanes", NLTag_lanes },
    /* 04 */    { "cedge", NLTag_cedge },
    /* 05 */    { "junction", NLTag_junction },
    /* 06 */    { "inlanes", NLTag_inlane },
    /* 07 */    { "detector", NLTag_detector },
    /* 08 */    { "vehicle", NLTag_vehicle },
    /* 09 */    { "vtype", NLTag_vtype },
    /* 10 */    { "route", NLTag_route },
    /* 11 */    { "succ", NLTag_succ },
    /* 12 */    { "succlane", NLTag_succlane },
    /* 13 */    { "key", NLTag_key },
    /* 14 */    { "junctionlogic", NLTag_junctionlogic }, // ?
    /* 15 */    { "requestsize", NLTag_requestsize },
    /* 16 */    { "responsesize", NLTag_responsesize },
    /* 17 */    { "lanenumber", NLTag_lanenumber },
    /* 18 */    { "logicitem", NLTag_logicitem },
    /* 19 */    { "trafoitem", NLTag_trafoitem },
    /* 20 */    { "row-logic", NLTag_rowlogic },
    /* 21 */    { "source", NLTag_source },
    /* 22 */    { "district", NLTag_district },
    /* 23 */    { "dsource", NLTag_dsource },
    /* 24 */    { "dsink", NLTag_dsink },
    /* 25 */    { "edges", NLTag_edges },
    /* 26 */    { "node_count", NLTag_nodecount },
    /* 27 */    { "tl-logic", NLTag_tllogic },
    /* 28 */    { "offset", NLTag_offset },
    /* 29 */    { "initstep", NLTag_initstep },
    /* 30 */    { "phaseno", NLTag_phaseno },
    /* 31 */    { "phase", NLTag_phase },
    /* 32 */    { "edgepos", NLTag_edgepos },
    /* 33 */    { "logicno", NLTag_logicno }
};

/* =========================================================================
 * method definitions
 * ======================================================================= */
NLSAXHandler::NLSAXHandler(NLContainer &container)
    : GenericSAX2Handler(_tags, 34), myContainer(container)
{
}


NLSAXHandler::~NLSAXHandler()
{
}


void
NLSAXHandler::warning(const SAXParseException& exception)
{
    setError("XML-Warning:", exception);
    GenericSAX2Handler::warning(exception);
}


void
NLSAXHandler::error(const SAXParseException& exception)
{
    setError("XML-Error:", exception);
    GenericSAX2Handler::error(exception);
}

void
NLSAXHandler::fatalError(const SAXParseException& exception)
{
    setError("XML-Fatal Error:", exception);
    SErrorHandler::setFatal();
    GenericSAX2Handler::fatalError(exception);
}

void
NLSAXHandler::setFileName(const std::string &file)
{
    _file = file;
}

void
NLSAXHandler::myStartElement(int element, const std::string &name,
                             const Attributes &attrs)
{
}

void
NLSAXHandler::myEndElement(int element, const std::string &name)
{
}

void
NLSAXHandler::myCharacters(int element, const std::string &name,
                           const std::string &chars)
{
}

void
NLSAXHandler::setError(const string &type,
                       const SAXParseException& exception)
{
    SErrorHandler::add(buildErrorMessage(_file, type, exception), true);
}

bool
NLSAXHandler::wanted(LoadFilter filter) const
{
    return (_filter&filter)!=0;
}


void
NLSAXHandler::setWanted(LoadFilter filter)
{
    _filter = filter;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NLSAXHandler.icc"
//#endif

// Local Variables:
// mode:C++
// End:



