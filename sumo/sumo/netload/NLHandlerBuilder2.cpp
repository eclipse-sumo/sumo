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
// Revision 1.5  2002/06/07 14:39:58  dkrajzew
// errors occured while building larger nets and adaption of new netconverting methods debugged
//
// Revision 1.4  2002/04/17 11:17:49  dkrajzew
// windows-newlines removed
//
// Revision 1.3  2002/04/16 06:06:02  dkrajzew
// Error report on errors while building detectors added.
//
// Revision 1.2  2002/04/15 07:05:36  dkrajzew
// new loading paradigm implemented
//
// Revision 1.1.1.1  2002/04/08 07:21:24  traffic
// new project name
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
#include "../utils/AttributesHandler.h"
#include "../utils/UtilExceptions.h"
#include "NLDetectorBuilder.h"
#include "NLSAXHandler.h"
#include "NLNetBuilder.h"
#include "NLLoadFilter.h"
#include "NLTags.h"

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * method definitions
 * ======================================================================= */
NLHandlerBuilder2::NLHandlerBuilder2(NLContainer &container, LoadFilter filter) 
    : NLSAXHandler(container, filter)
{
    _attrHandler.add(ATTR_ID, "id");
    _attrHandler.add(ATTR_KEY, "key");
    _attrHandler.add(ATTR_TYPE, "type");
    _attrHandler.add(ATTR_ROUTE, "route");
    _attrHandler.add(ATTR_DEPART, "depart");
    _attrHandler.add(ATTR_LANE, "lane");
    _attrHandler.add(ATTR_POSITION, "pos");
    _attrHandler.add(ATTR_SPLINTERVAL, "freq");
    _attrHandler.add(ATTR_STYLE, "style");
    _attrHandler.add(ATTR_FILE, "file");
}

NLHandlerBuilder2::~NLHandlerBuilder2() 
{
}

void 
NLHandlerBuilder2::myStartElement(int element, const std::string &name, const Attributes &attrs) 
{
    // process the net elements when wished
    if(wanted(LOADFILTER_NET)) {
        switch(element) {
        case NLTag_junction:
            openJunction(attrs);
            break;
        default:
            break;
        }
    }
    // process the dynamic components when wished
    if(wanted(LOADFILTER_DYNAMIC)) {
        switch(element) {
        case NLTag_vehicle:
            addVehicle(attrs);
            break;
        default:
            break;
        }
    }
    // process detectors when wished
    if(wanted(LOADFILTER_DETECTORS)&&element==NLTag_detector) {
        addDetector(attrs);
    }
}

void
NLHandlerBuilder2::openJunction(const Attributes &attrs) {
    string id;
    try {
        id = _attrHandler.getString(attrs, ATTR_ID);
        try {
            myContainer.openJunction(id, 
                _attrHandler.getStringSecure(attrs, ATTR_KEY, ""),
                _attrHandler.getString(attrs, ATTR_TYPE));
        } catch (XMLUngivenParameterException &e) {
            SErrorHandler::add(e.getMessage("junction", id));
        }
    } catch (XMLUngivenParameterException &e) {
        SErrorHandler::add(e.getMessage("junction", "(ID_UNKNOWN!)"));
    }
}


void
NLHandlerBuilder2::addVehicle(const Attributes &attrs) {
    string id;
    try {
        id = _attrHandler.getString(attrs, ATTR_ID);
        try {
            myContainer.addVehicle(id, 
                _attrHandler.getString(attrs, ATTR_TYPE),
                _attrHandler.getString(attrs, ATTR_ROUTE),
                _attrHandler.getLong(attrs, ATTR_DEPART));
        } catch (XMLUngivenParameterException &e) {
            SErrorHandler::add(e.getMessage("vehicle", id));
        } catch(XMLIdNotKnownException &e) {
            SErrorHandler::add(e.getMessage("", ""));
        } catch(XMLIdAlreadyUsedException &e) {
            SErrorHandler::add(e.getMessage("vehicle", id));
        }  
    } catch (XMLUngivenParameterException &e) {
        SErrorHandler::add(e.getMessage("vehicle", "(ID_UNKNOWN!)"));
    }  
}

void
NLHandlerBuilder2::addDetector(const Attributes &attrs) {
    string id;
    try {
        id = _attrHandler.getString(attrs, ATTR_ID);
        try {
            myContainer.addDetector(
                NLDetectorBuilder::buildInductLoop(id,
                    _attrHandler.getString(attrs, ATTR_LANE),
                    _attrHandler.getFloat(attrs, ATTR_POSITION),
                    _attrHandler.getFloat(attrs, ATTR_SPLINTERVAL),
                    _attrHandler.getString(attrs, ATTR_STYLE),
                    _attrHandler.getString(attrs, ATTR_FILE)));
        } catch (XMLBuildingException &e) {
            SErrorHandler::add(e.getMessage("detector", id));
        } catch (InvalidArgument &e) {
            SErrorHandler::add(e.msg());
        }
    } catch (XMLUngivenParameterException &e) {
        SErrorHandler::add(e.getMessage("detector", "(ID_UNKNOWN!)"));
    }  
}


void 
NLHandlerBuilder2::myEndElement(int element, const std::string &name)
{
    if(wanted(LOADFILTER_NET)) {
        switch(element) {
        case NLTag_junction:
            closeJunction();
            break;
        default:
            break;
        }
    }
}

void 
NLHandlerBuilder2::myCharacters(int element, const std::string &name, const std::string &chars) 
{
    if(wanted(LOADFILTER_NET)) {
        switch(element) {
        case NLTag_inlane:
            addInLanes(chars);
            break;
        default:
            break;
        }
    }
}


void
NLHandlerBuilder2::closeJunction() {
    try {
        myContainer.closeJunction();
    } catch (XMLIdAlreadyUsedException &e) {
        SErrorHandler::add(e.getMessage("junction", ""));
    } catch (XMLIdNotKnownException &e) {
        SErrorHandler::add(e.getMessage("junction", ""));
    }
}


void
NLHandlerBuilder2::addInLanes(const std::string &chars) {
    StringTokenizer st(chars);
    while(st.hasNext()) {
        string set = st.next();
        try {
            myContainer.addInLane(set);
        } catch (XMLIdNotKnownException &e) {
            SErrorHandler::add(e.getMessage("lane", set));
        }
    }
}

std::string
NLHandlerBuilder2::getMessage() const {
    return "Loading junctions, detectors and vehicles...";
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NLHandlerBuilder2.icc"
//#endif

// Local Variables:
// mode:C++
// 

