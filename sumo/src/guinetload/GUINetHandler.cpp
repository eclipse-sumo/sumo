//---------------------------------------------------------------------------//
//                        GUINetHandler.cpp -
//  The XML-Handler for building networks within the gui-version derived
//      from NLNetHandler
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.11  2003/11/17 07:13:48  dkrajzew
// e2-detector over lanes merger added
//
// Revision 1.10  2003/10/08 14:48:56  dkrajzew
// new usage of MSAgentbased... impemented
//
// Revision 1.9  2003/10/02 14:51:20  dkrajzew
// visualisation of E2-detectors implemented
//
// Revision 1.8  2003/10/01 11:13:13  dkrajzew
// agent-based tl-logic allocation added
//
// Revision 1.7  2003/09/24 09:54:11  dkrajzew
// bug on building induct loops of an actuated tls within the gui patched
//
// Revision 1.6  2003/07/22 14:58:33  dkrajzew
// changes due to new detector handling
//
// Revision 1.5  2003/07/16 15:21:16  dkrajzew
// conversion tools splitted and relocated to avoid mandatory inclusion of unused files
//
// Revision 1.4  2003/07/07 08:13:15  dkrajzew
// first steps towards the usage of a real lane and junction geometry implemented
//
// Revision 1.3  2003/06/18 11:08:05  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.2  2003/02/13 15:59:00  dkrajzew
// unnecessary output of build edges id removed
//
// Revision 1.1  2003/02/07 10:38:19  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <iostream>
#include <sax/HandlerBase.hpp>
#include <sax/AttributeList.hpp>
#include <sax/SAXParseException.hpp>
#include <sax/SAXException.hpp>
#include <netload/NLNetHandler.h>
#include <netload/NLContainer.h>
#include <utils/convert/TplConvert.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/UtilExceptions.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include <utils/xml/XMLBuildingExceptions.h>
#include <microsim/MSAgentbasedTrafficLightLogic.h>
#include <guisim/GUIInductLoop.h>
#include <guisim/GUI_E2_ZS_Collector.h>
#include <guisim/GUI_E2_ZS_CollectorOverLanes.h>
#include "GUIContainer.h"
#include "GUIDetectorBuilder.h"
#include "GUINetHandler.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUINetHandler::GUINetHandler(const std::string &file,
                             NLContainer &container)
    : NLNetHandler(file, container)
{
}


GUINetHandler::~GUINetHandler()
{
}


void
GUINetHandler::myStartElement(int element, const std::string &name,
                                  const Attributes &attrs)
{
    NLNetHandler::myStartElement(element, name, attrs);
    if(wanted(LOADFILTER_NET) && element==SUMO_TAG_EDGEPOS) {
        addSourceDestinationInformation(attrs);
    }
}


void
GUINetHandler::addDetector(const Attributes &attrs) {
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
        try {
//             myContainer.addDetector(
                 GUIDetectorBuilder::buildInductLoop(id,
                     getString(attrs, SUMO_ATTR_LANE),
                     getFloat(attrs, SUMO_ATTR_POSITION),
                     getInt(attrs, SUMO_ATTR_SPLINTERVAL),
                     getString(attrs, SUMO_ATTR_STYLE),
                     getString(attrs, SUMO_ATTR_FILE),
                     _file);
        } catch (XMLBuildingException &e) {
            MsgHandler::getErrorInstance()->inform(e.getMessage("detector", id));
        } catch (InvalidArgument &e) {
            MsgHandler::getErrorInstance()->inform(e.msg());
        } catch (EmptyData) {
            MsgHandler::getErrorInstance()->inform(
                string("The description of the detector '")
                + id + string("' does not contain a needed value."));
        }
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(
            "Error in description: missing id of a detector-object.");
    }
}




void
GUINetHandler::myCharacters(int element, const std::string &name,
                                const std::string &chars)
{
    NLNetHandler::myCharacters(element, name, chars);
    if(wanted(LOADFILTER_NET)) {
        switch(element) {
        case SUMO_TAG_SHAPE:
            addJunctionShape(chars);
            break;
        case SUMO_TAG_LANE:
            addLaneShape(chars);
            break;
        default:
            break;
        }
    }
}


void
GUINetHandler::addSourceDestinationInformation(const Attributes &attrs) {
    try {
        string id = getString(attrs, SUMO_ATTR_ID);
        string from = getString(attrs, SUMO_ATTR_FROM);
        string to = getString(attrs, SUMO_ATTR_TO);
        static_cast<GUIContainer&>(myContainer).addSrcDestInfo(id, from, to);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(
            "Error in description: An edge has no information about the from/to-node");
    }
}


void
GUINetHandler::addJunctionShape(const std::string &chars)
{
    Position2DVector shape = GeomConvHelper::parseShape(chars);
    static_cast<GUIContainer&>(myContainer).addJunctionShape(shape);
}


void
GUINetHandler::addLaneShape(const std::string &chars)
{
    Position2DVector shape = GeomConvHelper::parseShape(chars);
    static_cast<GUIContainer&>(myContainer).addLaneShape(shape);
}


void
GUINetHandler::closeTrafficLightLogic()
{
    if(_tlLogicNo!=0) {
        return;
    }
    if(m_Type=="actuated") {
        MSActuatedTrafficLightLogic<GUIInductLoop, MSLaneState  >
            *tlLogic =
            new MSActuatedTrafficLightLogic<GUIInductLoop, MSLaneState > (
                    m_Key, m_ActivePhases, 0,
                    myContainer.getInLanes(), m_Offset, myContinuations);
        MSTrafficLightLogic::dictionary(m_Key, tlLogic);
        // !!! replacement within the dictionary
        m_ActivePhases.clear();
        myContainer.addTLLogic(tlLogic);
    } else if (m_Type=="agentbased") {
        MSAgentbasedTrafficLightLogic<GUI_E2_ZS_CollectorOverLanes>
            *tlLogic =
            new MSAgentbasedTrafficLightLogic<GUI_E2_ZS_CollectorOverLanes> (
                    m_Key, m_ActivePhases, 0,
                    myContainer.getInLanes(), m_Offset, myContinuations);
        MSTrafficLightLogic::dictionary(m_Key, tlLogic);
        // !!! replacement within the dictionary
        m_ActivePhases.clear();
        myContainer.addTLLogic(tlLogic);
	} else {
        MSTrafficLightLogic *tlLogic =
            new MSSimpleTrafficLightLogic(
                m_Key, m_ActivePhases, 0, m_Offset);
        MSTrafficLightLogic::dictionary(m_Key, tlLogic);
        // !!! replacement within the dictionary
        m_ActivePhases.clear();
        myContainer.addTLLogic(tlLogic);
    }
}





/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUINetHandler.icc"
//#endif

// Local Variables:
// mode:C++
// End:







