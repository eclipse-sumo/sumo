/****************************************************************************/
/// @file    TraCIServerAPI_GUI.cpp
/// @author  Daniel Krajzewicz
/// @date    07.05.2009
/// @version $Id: TraCIServerAPI_GUI.cpp 8911 2010-08-03 09:32:26Z dkrajzew $
///
// APIs for getting/setting GUI values via TraCI
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifndef NO_TRACI

#include <fx.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/gui/windows/GUIGlChildWindow.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/windows/GUIPerspectiveChanger.h>
#include <traci-server/TraCIConstants.h>
#include "TraCIServerAPI_GUI.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;
using namespace traci;
using namespace tcpip;


// ===========================================================================
// method definitions
// ===========================================================================
bool
TraCIServerAPI_GUI::processGet(TraCIServer &server, tcpip::Storage &inputStorage,
							   tcpip::Storage &outputStorage) throw(TraCIException, std::invalid_argument) {
    std::string warning = ""; // additional description for response
    // variable & id
    int variable = inputStorage.readUnsignedByte();
    std::string id = inputStorage.readString();
    // check variable
    if (variable!=ID_LIST
		&&variable!=VAR_VIEW_ZOOM&&variable!=VAR_VIEW_OFFSET&&variable!=VAR_VIEW_SCHEMA&&variable!=VAR_VIEW_BOUNDARY
		&&variable!=VAR_VIEW_BACKGROUNDCOLOR
       ) {
        server.writeStatusCmd(CMD_GET_GUI_VARIABLE, RTYPE_ERR, "Get GUI Variable: unsupported variable specified", outputStorage);
        return false;
    }
    // begin response building
    Storage tempMsg;
    //  response-code, variableID, objectID
    tempMsg.writeUnsignedByte(RESPONSE_GET_GUI_VARIABLE);
    tempMsg.writeUnsignedByte(variable);
    tempMsg.writeString(id);
    // process request
    if (variable==ID_LIST) {
        std::vector<std::string> ids = ((GUIMainWindow*) FXApp::instance()->getActiveWindow())->getViewIDs();
        tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
        tempMsg.writeStringList(ids);
    } else {
		GUIGlChildWindow *w = (GUIGlChildWindow*) ((GUIMainWindow*) FXApp::instance()->getActiveWindow())->getViewByID(id);
        if (w==0) {
            server.writeStatusCmd(CMD_GET_GUI_VARIABLE, RTYPE_ERR, "View '" + id + "' is not known", outputStorage);
            return false;
        }
		GUISUMOAbstractView &v = w->getView();
        switch (variable) {
        case VAR_VIEW_ZOOM:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
			tempMsg.writeFloat(v.getChanger().getZoom());
            break;
        case VAR_VIEW_OFFSET:
            tempMsg.writeUnsignedByte(POSITION_2D);
			tempMsg.writeFloat(v.getChanger().getXPos());
			tempMsg.writeFloat(v.getChanger().getYPos());
            break;
        case VAR_VIEW_SCHEMA:
            break;
        case VAR_VIEW_BOUNDARY:
            break;
        case VAR_VIEW_BACKGROUNDCOLOR:
            break;
        default:
            break;
        }
    }
        server.writeStatusCmd(CMD_GET_GUI_VARIABLE, RTYPE_OK, warning, outputStorage);
    // send response
    outputStorage.writeUnsignedByte(0); // command length -> extended
    outputStorage.writeInt(1 + 4 + tempMsg.size());
    outputStorage.writeStorage(tempMsg);
    return true;
}


bool
TraCIServerAPI_GUI::processSet(TraCIServer &server, tcpip::Storage &inputStorage,
							   tcpip::Storage &outputStorage) throw(TraCIException, std::invalid_argument) {
    std::string warning = ""; // additional description for response
    // variable
    int variable = inputStorage.readUnsignedByte();
    if (variable!=VAR_VIEW_ZOOM&&variable!=VAR_VIEW_OFFSET&&variable!=VAR_VIEW_SCHEMA&&variable!=VAR_VIEW_BOUNDARY
		&&variable!=VAR_VIEW_BACKGROUNDCOLOR&&variable!=VAR_SCREENSHOT
       ) {
        server.writeStatusCmd(CMD_SET_GUI_VARIABLE, RTYPE_ERR, "Change GUI State: unsupported variable specified", outputStorage);
        return false;
    }
    // id
    std::string id = inputStorage.readString();
	GUIGlChildWindow *w = (GUIGlChildWindow*) ((GUIMainWindow*) FXApp::instance()->getActiveWindow())->getViewByID(id);
    if (w==0) {
        server.writeStatusCmd(CMD_SET_GUI_VARIABLE, RTYPE_ERR, "View '" + id + "' is not known", outputStorage);
        return false;
    }
	GUISUMOAbstractView &v = w->getView();
    // process
    int valueDataType = inputStorage.readUnsignedByte();
    switch (variable) {
    case VAR_VIEW_ZOOM: 
        if (valueDataType!=TYPE_FLOAT) {
            server.writeStatusCmd(CMD_SET_GUI_VARIABLE, RTYPE_ERR, "The zoom must be given as a float.", outputStorage);
            return false;
        }
		v.setViewport(inputStorage.readFloat(), v.getChanger().getXPos(), v.getChanger().getYPos());
    break;
    case VAR_VIEW_OFFSET: {
        if (valueDataType!=POSITION_2D) {
            server.writeStatusCmd(CMD_SET_GUI_VARIABLE, RTYPE_ERR, "The view port must be given as a position.", outputStorage);
            return false;
        }
		v.setViewport(v.getChanger().getZoom(), inputStorage.readFloat(), v.getChanger().getYPos());
		v.setViewport(v.getChanger().getZoom(), v.getChanger().getXPos(), inputStorage.readFloat());
    }
    break;
    case VAR_VIEW_SCHEMA:
    break;
    case VAR_VIEW_BOUNDARY:
    break;
    case VAR_VIEW_BACKGROUNDCOLOR:
    break;
    case VAR_SCREENSHOT:
    break;
    default:
        break;
    }
    server.writeStatusCmd(CMD_SET_GUI_VARIABLE, RTYPE_OK, warning, outputStorage);
    return true;
}


#endif


/****************************************************************************/

