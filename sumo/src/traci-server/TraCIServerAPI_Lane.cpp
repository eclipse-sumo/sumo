/****************************************************************************/
/// @file    TraCIServerAPI_Lane.cpp
/// @author  Daniel Krajzewicz
/// @date    07.05.2009
/// @version $Id: TraCIServerAPI_Lane.cpp 6907 2009-03-13 12:13:38Z dkrajzew $
///
// APIs for getting/setting lane values via TraCI
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

#include "TraCIConstants.h"
#include <microsim/MSLane.h>
#include "TraCIServerAPIHelper.h"
#include "TraCIServerAPI_Lane.h"

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
TraCIServerAPI_Lane::processGet(tcpip::Storage &inputStorage,
                                tcpip::Storage &outputStorage) throw(TraCIException) {
    Storage tmpResult;
    string warning = "";	// additional description for response
    // variable
    int variable = inputStorage.readUnsignedByte();
    string id = inputStorage.readString();
    // check variable
    if (variable!=ID_LIST&&variable!=LANE_LINK_NUMBER&&variable!=LANE_EDGE_ID&&variable!=VAR_LENGTH
        &&variable!=VAR_MAXSPEED&&variable!=LANE_LINKS&&variable!=VAR_SHAPE
        &&variable!=LANE_ALLOWED&&variable!=LANE_DISALLOWED) {
        TraCIServerAPIHelper::writeStatusCmd(CMD_GET_LANE_VARIABLE, RTYPE_ERR, "Unsupported variable specified", outputStorage);
        return false;
    }
    // begin response building
    Storage tempMsg;
    //  response-code, variableID, objectID
    tempMsg.writeUnsignedByte(RESPONSE_GET_LANE_VARIABLE);
    tempMsg.writeUnsignedByte(variable);
    tempMsg.writeString(id);
    if (variable==ID_LIST) {
        std::vector<std::string> ids;
        MSLane::insertIDs(ids);
        tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
        tempMsg.writeStringList(ids);
    } else {
        MSLane *lane = MSLane::dictionary(id);
        if (lane==0) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_GET_LANE_VARIABLE, RTYPE_ERR, "Lane '" + id + "' is not known", outputStorage);
            return false;
        }
        switch (variable) {
        case LANE_LINK_NUMBER:
            tempMsg.writeUnsignedByte(TYPE_UBYTE);
            tempMsg.writeUnsignedByte((int) lane->getLinkCont().size());
            break;
        case LANE_EDGE_ID:
            tempMsg.writeUnsignedByte(TYPE_STRING);
            tempMsg.writeString(lane->getEdge().getID());
            break;
        case VAR_LENGTH:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat(lane->getLength());
            break;
        case VAR_MAXSPEED:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat(lane->getMaxSpeed());
            break;
        case LANE_LINKS: {
            tempMsg.writeUnsignedByte(TYPE_COMPOUND);
            Storage tempContent;
            unsigned int cnt = 0;
            tempContent.writeUnsignedByte(TYPE_INTEGER);
            const MSLinkCont &links = lane->getLinkCont();
            tempContent.writeInt((int) links.size());
            ++cnt;
            for (MSLinkCont::const_iterator i=links.begin(); i!=links.end(); ++i) {
                MSLink *link = (*i);
                // approached non-internal lane (if any)
                tempContent.writeUnsignedByte(TYPE_STRING);
                tempContent.writeString(link->getLane()!=0 ? link->getLane()->getID() : "");
                ++cnt;
                // approached "via", internal lane (if any)
                tempContent.writeUnsignedByte(TYPE_STRING);
#ifdef HAVE_INTERNAL_LANES
                tempContent.writeString(link->getViaLane()!=0 ? link->getViaLane()->getID() : "");
#else
                tempContent.writeString("");
#endif
                ++cnt;
                // priority
                tempContent.writeUnsignedByte(TYPE_UBYTE);
                tempContent.writeUnsignedByte(link->havePriority() ? 1 : 0);
                ++cnt;
                // opened
                tempContent.writeUnsignedByte(TYPE_UBYTE);
                tempContent.writeUnsignedByte(link->opened() ? 1 : 0);
                ++cnt;
                // approaching foe
                tempContent.writeUnsignedByte(TYPE_UBYTE);
                tempContent.writeUnsignedByte(link->hasApproachingFoe() ? 1 : 0);
                ++cnt;
                // state (not implemented, yet)
                tempContent.writeUnsignedByte(TYPE_STRING);
                tempContent.writeString("");
                ++cnt;
                // direction (not implemented, yet)
                tempContent.writeUnsignedByte(TYPE_STRING);
                tempContent.writeString("");
                ++cnt;
                // length
                tempContent.writeUnsignedByte(TYPE_FLOAT);
                tempContent.writeFloat(link->getLength());
                ++cnt;
            }
            tempMsg.writeInt((int) cnt);
            tempMsg.writeStorage(tempContent);
        }
        break;
        case LANE_ALLOWED: {
            const std::vector<SUMOVehicleClass> &allowed = lane->getAllowedClasses();
            std::vector<std::string> allowedS;
            for(std::vector<SUMOVehicleClass>::const_iterator i=allowed.begin(); i!=allowed.end(); ++i) {
                allowedS.push_back(getVehicleClassName(*i));
            }
            tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
            tempMsg.writeStringList(allowedS);
        }            
        case LANE_DISALLOWED: {
            const std::vector<SUMOVehicleClass> &disallowed = lane->getNotAllowedClasses();
            std::vector<std::string> disallowedS;
            for(std::vector<SUMOVehicleClass>::const_iterator i=disallowed.begin(); i!=disallowed.end(); ++i) {
                disallowedS.push_back(getVehicleClassName(*i));
            }
            tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
            tempMsg.writeStringList(disallowedS);
        }            
        break;
        case VAR_SHAPE:
            tempMsg.writeUnsignedByte(TYPE_POLYGON);
            tempMsg.writeUnsignedByte(MIN2(static_cast<size_t>(255),lane->getShape().size()));
            for (int iPoint=0; iPoint < MIN2(static_cast<size_t>(255),lane->getShape().size()); ++iPoint) {
                tempMsg.writeFloat(lane->getShape()[iPoint].x());
                tempMsg.writeFloat(lane->getShape()[iPoint].y());
            }
            break;
        default:
            break;
        }
    }
    TraCIServerAPIHelper::writeStatusCmd(CMD_GET_LANE_VARIABLE, RTYPE_OK, warning, outputStorage);
    // send response
    outputStorage.writeUnsignedByte(0); // command length -> extended
    outputStorage.writeInt(1 + 4 + tempMsg.size());
    outputStorage.writeStorage(tempMsg);
    return true;
}


/****************************************************************************/

