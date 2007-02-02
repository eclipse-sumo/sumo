/****************************************************************************/
/// @file    ampel.cpp
/// @author  Marc Gurczik
/// @date    unknown_date
/// @version $Id: $
///
// missing_desc
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#include <ampel.h>

// RakNet header files
#include <PacketEnumerations.h>
#include <RakNetworkFactory.h>
#include <NetworkTypes.h>
#include <RakClientInterface.h>
#include <RakPeerInterface.h>
#include <GetTime.h>  // for timestamping

// standard header files
#include <windows.h> // for Sleep()
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include <wchar.h>
#include <math.h>

// Irrlicht header files
#include <irrlicht.h>
#include <IrrlichtDevice.h>
#include <SIrrCreationParameters.h>
#include <IVideoDriver.h>

//#using <mscorlib.dll>

// namespaces in Irrlicht
using namespace irr;
using namespace gui;
using namespace io;
using namespace core;
using namespace scene;
using namespace video;

// ===========================================================================
// used namespaces
// ===========================================================================
// namespace in RakNet
using namespace RakNet;

// Variables
RakClientInterface* ampelClient;

#include <fx.h>

/** Konstruktor */
Ampel::Ampel(void)
{
    // RakClient wird initialisiert
    ampelClient = RakNetworkFactory::GetRakClientInterface();

    // RakClient mit dem Server verbinden
    char* pServerId = new char(SERVER_IP);
    ampelClient->Connect(pServerId, OBJECT_PORT, 0, 0, 0);

    FXThread::sleep(1);
}
/** Dekonstruktor */
Ampel::~Ampel(void)
{
//	ampelClient->Disconnect(300);
//    RakNetworkFactory::DestroyRakClientInterface(ampelClient);
}

void Ampel::addTrafficLight(int objectID, float x, float y, float z, float rot)
{
    if (ampelClient->IsConnected()) {
        // Datenstrom zum Server
        RakNet::BitStream dataStream;

        // When writing a timestamp, the first byte is ID_TIMESTAMP
        // The next 4 bytes is the timestamp itself.
        dataStream.Write((unsigned char)ID_TIMESTAMP);
        time=RakNet::GetTime();
        dataStream.Write(time);

        // write content to the data stream
        dataStream.Write(PACKET_ID_TRAFFICLIGHT);
        dataStream.Write(objectID);
        dataStream.Write(PACKET_TRAFFIC_LIGHT_NEW);
        dataStream.Write(x);
        dataStream.Write(y);
        dataStream.Write(z);
        dataStream.Write(rot);

        ampelClient->Send(&dataStream, LOW_PRIORITY, UNRELIABLE, 2);

        printf("Sending packet from client at time %i\n", time);
    }
}

void Ampel::setTrafficLightState(int objectID, int state)
{
    if (ampelClient->IsConnected()) {
        // Datenstrom zum Server
        RakNet::BitStream dataStream;

        // When writing a timestamp, the first byte is ID_TIMESTAMP
        // The next 4 bytes is the timestamp itself.
        dataStream.Write((unsigned char)ID_TIMESTAMP);
        time=RakNet::GetTime();
        dataStream.Write(time);

        // write content to the data stream
        dataStream.Write(PACKET_ID_TRAFFICLIGHT);
        dataStream.Write(objectID);
        dataStream.Write(PACKET_TRAFFIC_LIGHT_MODIFIED);
        dataStream.Write(state);

        ampelClient->Send(&dataStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0);

        printf("Sending packet from client at time %i\n", time);
    }
}

/**
void Ampel::SendPacketToServer(int objID, int objHandle, int x, int y, int z)
{
	if (ampelClient->IsConnected())
	{
		/**
		// Datenstrom zum Server
		RakNet::BitStream dataStream;

		// When writing a timestamp, the first byte is ID_TIMESTAMP
		// The next 4 bytes is the timestamp itself.
		dataStream.Write((unsigned char)ID_TIMESTAMP);
		time=RakNet::GetTime();
		dataStream.Write(time);

		// write content to the data stream
		dataStream.Write(PACKET_ID_STREET);
		dataStream.Write(objectID);
		dataStream.Write(PACKET_OBJECT_NEW_OR_MODIFIED);
		dataStream.Write(startX);
		dataStream.Write(startZ);
		dataStream.Write(destX);
		dataStream.Write(destZ);
		dataStream.Write(midX);
		dataStream.Write(midZ);
		dataStream.Write(dx);
		dataStream.Write(dz);

		ampelClient->Send(&dataStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0);

		printf("Sending packet from client at time %i\n", time);

	}
 * ======================================================================= */
* included modules
/* =========================================================================
}*/


/****************************************************************************/

