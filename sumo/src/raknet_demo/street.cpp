/****************************************************************************/
/// @file    street.cpp
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
#include <street.h>

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
RakClientInterface* streetClient;


const char Street::SERVER_IP = "127.0.0.1";
#include <fx.h>

/** Konstruktor */
Street::Street(void)
{
    // RakClient wird initialisiert
    streetClient = RakNetworkFactory::GetRakClientInterface();

    // RakClient mit dem Server verbinden
    char* pServerId = new char(SERVER_IP);
    streetClient->Connect(pServerId, OBJECT_PORT, 0, 0, 0);
    FXThread::sleep(1);
//	System::Threading::Thread::Sleep(100);
}
/** Dekonstruktor */
Street::~Street(void)
{
//	streetClient->Disconnect(300);
//    RakNetworkFactory::DestroyRakClientInterface(streetClient);
}

// methods

void Street::addLane(int objectID, int noPoints, float *streetPointsX, float *streetPointsZ)
{
    for (int i = 0; i < noPoints; i++) {
        f32 startX;
        f32 startZ;
        f32 destX;
        f32 destZ;
        f32 midX;
        f32 midZ;
        f32 dx;
        f32 dz;
        if (streetPointsX[i + 1] != NULL) {
            startX = (f32)streetPointsX[i];
            startZ = (f32)streetPointsZ[i];
            destX = (f32)streetPointsX[i + 1];
            destZ = (f32)streetPointsZ[i + 1];
            // calculate the middle of the road as position
            midX = (startX + destX) / 2;
            midZ = (startZ + destZ) / 2;
            // calculate the rotation
            dx = destX - startX;
            dz = destZ - startZ;

            if (streetClient->IsConnected()) {
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

                streetClient->Send(&dataStream, MEDIUM_PRIORITY, UNRELIABLE, 1);

                printf("Sending packet from client at time %i\n", time);
            }

//			SendPacketToServer(objectID, startX, startZ, destX, destZ, midX, midZ, dx, dz);
        }
    }
}
/*
void Street::SendPacketToServer(s32 objectID, f32 startX,
		f32 startZ, f32 destX, f32 destZ, f32 midX, f32 midZ, f32 dx, f32 dz)
{
	if (streetClient->IsConnected())
	{
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

		streetClient->Send(&dataStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0);

		printf("Sending packet from client at time %i\n", time);
	}
}



/****************************************************************************/

