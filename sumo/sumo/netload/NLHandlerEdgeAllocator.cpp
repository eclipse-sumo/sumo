/***************************************************************************
                          NLHandlerEdgeAllocator.cpp
			  The second-step - handler that is responsible for
			  the allocation of edges
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
// Revision 1.7  2002/06/21 10:53:49  dkrajzew
// inclusion of .cpp-files in .cpp files removed
//
// Revision 1.6  2002/06/11 14:39:26  dkrajzew
// windows eol removed
//
// Revision 1.5  2002/06/11 13:44:32  dkrajzew
// Windows eol removed
//
// Revision 1.4  2002/06/10 08:36:07  dkrajzew
// Conversion of strings generalized
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
// Revision 2.0  2002/02/14 14:43:23  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.3  2002/02/13 15:40:43  croessel
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
#include "NLHandlerEdgeAllocator.h"
#include "SErrorHandler.h"
#include "../utils/TplConvert.h"
#include "../utils/XMLBuildingExceptions.h"
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
NLHandlerEdgeAllocator::NLHandlerEdgeAllocator(NLContainer &container, LoadFilter filter)
    : NLSAXHandler(container, filter)
{
    _attrHandler.add(ATTR_ID, "id");
}

NLHandlerEdgeAllocator::~NLHandlerEdgeAllocator()
{
}

void
NLHandlerEdgeAllocator::myStartElement(int element, const std::string &name, const Attributes &attrs)
{
    if(wanted(LOADFILTER_NET)) {
        switch(element) {
        case NLTag_edge:
            addEdge(attrs);
            break;
        default:
            break;
        }
    }
}

void
NLHandlerEdgeAllocator::addEdge(const Attributes &attrs) {
    string id;
    try {
        id = _attrHandler.getString(attrs, ATTR_ID);
        myContainer.addEdge(id);
    } catch (XMLIdAlreadyUsedException &e) {
        SErrorHandler::add(e.getMessage("edge", id));
    } catch (EmptyData &e) {
        SErrorHandler::add("Error in description: missing id of an edge-object.");
    }
}

std::string
NLHandlerEdgeAllocator::getMessage() const{
    return "Loading edges...";
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NLHandlerEdgeAllocator.icc"
//#endif

// Local Variables:
// mode:C++
// End:
