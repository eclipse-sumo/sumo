/****************************************************************************/
/// @file    TraCIServerAPI_POI.cpp
/// @author  Daniel Krajzewicz
/// @date    07.05.2009
/// @version $Id: TraCIServerAPI_POI.cpp 6907 2009-03-13 12:13:38Z dkrajzew $
///
// APIs for getting/setting POI values via TraCI
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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

#include <microsim/MSNet.h>
#include <utils/shapes/PointOfInterest.h>
#include <utils/shapes/ShapeContainer.h>
#include "TraCIConstants.h"
#include "TraCIServerAPIHelper.h"
#include "TraCIServerAPI_POI.h"

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
TraCIServerAPI_POI::processGet(tcpip::Storage &inputStorage, 
                                         tcpip::Storage &outputStorage) throw(TraCIException)
{
    string warning = ""; // additional description for response
    // variable & id
    int variable = inputStorage.readUnsignedByte();
    string id = inputStorage.readString();
    // check variable
    if (variable!=ID_LIST&&variable!=VAR_TYPE&&variable!=VAR_COLOR&&variable!=VAR_POSITION) {
        TraCIServerAPIHelper::writeStatusCmd(CMD_GET_POI_VARIABLE, RTYPE_ERR, "Unsupported variable specified", outputStorage);
        return false;
    }
    // begin response building
    Storage tempMsg;
    //  response-code, variableID, objectID
    tempMsg.writeUnsignedByte(RESPONSE_GET_POI_VARIABLE);
    tempMsg.writeUnsignedByte(variable);
    tempMsg.writeString(id);
    // process request
    if (variable==ID_LIST) {
        std::vector<std::string> ids;
        ShapeContainer& shapeCont = MSNet::getInstance()->getShapeContainer();
        for (int i = shapeCont.getMinLayer(); i <= shapeCont.getMaxLayer(); i++) {
            shapeCont.getPOICont(i).insertIDs(ids);
        }
        tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
        tempMsg.writeStringList(ids);
    } else {
        PointOfInterest *p = 0;
        ShapeContainer& shapeCont = MSNet::getInstance()->getShapeContainer();
        for (int i = shapeCont.getMinLayer(); i <= shapeCont.getMaxLayer()&&p==0; i++) {
            p = shapeCont.getPOICont(i).get(id);
        }
        if(p==0) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_GET_POI_VARIABLE, RTYPE_ERR, "POI '" + id + "' is not known", outputStorage);
            return false;
        }
        switch (variable) {
        case VAR_TYPE:
            tempMsg.writeUnsignedByte(TYPE_STRING);
            tempMsg.writeString(p->getType());
            break;
        case VAR_COLOR:
            tempMsg.writeUnsignedByte(TYPE_COLOR);
            tempMsg.writeUnsignedByte((int) (p->red()*255.));
            tempMsg.writeUnsignedByte((int) (p->green()*255.));
            tempMsg.writeUnsignedByte((int) (p->blue()*255.));
            tempMsg.writeUnsignedByte(255);
            break;
        case VAR_POSITION:
            tempMsg.writeUnsignedByte(TYPE_POSITION2D);
            tempMsg.writeFloat(p->x());
            tempMsg.writeFloat(p->y());
            break;
        default:
        break;
        }
    }
    TraCIServerAPIHelper::writeStatusCmd(CMD_GET_POI_VARIABLE, RTYPE_OK, warning, outputStorage);
    // send response
    outputStorage.writeUnsignedByte(0); // command length -> extended
    outputStorage.writeInt(1 + 4 + tempMsg.size());
    outputStorage.writeStorage(tempMsg);
    return true;
}


bool 
TraCIServerAPI_POI::processSet(tcpip::Storage &inputStorage, 
                                         tcpip::Storage &outputStorage) throw(TraCIException)
{
    return true;
}



/****************************************************************************/

