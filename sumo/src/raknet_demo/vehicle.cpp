#include <vehicle.h>

// RakNet header files
#include <PacketEnumerations.h>
#include <RakNetworkFactory.h>
#include <NetworkTypes.h>
#include <RakClientInterface.h>
#include <RakPeerInterface.h>
//#include <BitStream.h>
#include "GetTime.h"

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

// namespace in RakNet
using namespace RakNet;

// Vehicle::objectCounterjektzähler
unsigned int Vehicle::objectCounter;
int intOC;
// Variables
RakClientInterface* vehicleClient;
const char Vehicle::SERVER_IP = "127.0.0.1";


// Variablen für die Routenplanung
int startX = 0;
int startY = 0;
int startZ = 0;
int destX = 0;
int destY = 0;
int destZ = 0;
double velocity, distPix, distReal, time;
float pixelRatio = 10;
double diffX, diffY, diffZ;
// Intervall in Pakete pro Sekunde
double interval = 1;

/** Konstruktor */
Vehicle::Vehicle(void)
{
	// Objektzähler wird initialisiert
	intOC = Vehicle::objectCounter++;

	// RakClient wird initialisiert
	vehicleClient = RakNetworkFactory::GetRakClientInterface();

	// RakClient mit dem Server verbinden
	char* pServerId = new char(SERVER_IP);
	vehicleClient->Connect(pServerId, OBJECT_PORT, 0, 0, 0);

//	System::Threading::Thread::Sleep(100);
}
/** Dekonstruktor */
Vehicle::~Vehicle(void)
{
//	vehicleClient->Disconnect(300);
//    RakNetworkFactory::DestroyRakClientInterface(vehicleClient);
}

void Vehicle::setStartPosition(int x, int y, int z)
{
	startX = x;
	startY = y;
	startZ = z;
}
void Vehicle::setDestPosition(int x, int y, int z)
{
	destX = x;
	destY = y;
	destZ = z;
}

void Vehicle::setViaPoint(int x, int y, int z){}

void Vehicle::setVelocity(double v)
{
	velocity = v;
}

void Vehicle::calculateRoute()
{
	/**
	 * Jede Sekunde wird ein Paket geschickt. Bei einer Geschwindigkeit von 72 km/h oder 20 m/s
	 * legt dass Auto bei einer angenommenen Pixelgröße von 10 cm in jedem Schritt 200 Pixel zurück.
	 * Man berechnet zuerst die Gesamtentfernung zwischen zwei Positionen in Pixeln, danach die
	 * Anzahl der Schritte die es benötigt um von a nach b zu kommen und dann Stück für Stück die
	 * einzelnen abzufahrenden Punkte.
	 */

	// Distanzbestimmung
	/**
	 * Der Verbindungsvektor von P nach Q ist durch die Differenz der Ortsvektoren Q - P gegeben.
	 * Der Abstand zweier Punkte ist der Betrag ihres Verbindungsvektors.
	 * Betrag eines Vektors
	 * Ein Vektor kann durch Pfeile dargestellt werden, die alle in dieselbe Richtung zeigen und
	 * dieselbe Länge haben. Letztere wird als Betrag des Vektors bezeichnet und mit demselben Symbol
	 * wie der Absoutbetrag von Zahlen bezeichnet.
	 * Für zweikomponentige Vektoren a = (a1, a2) ist |a| = (a1^2 + a2^2)^1/2,
	 * für dreikomponentige Vektoren a = (a1, a2, a3) gilt |a| = (a1^2 + a2^2 + a3^2)^1/2.
	*/

	// Quadrate der Vektorkomponenten
	int x2, y2, z2;
	x2 = (startX - destX) * (startX - destX);
	y2 = (startY - destY) * (startY - destY);
	z2 = (startZ - destZ) * (startZ - destZ);

	// Achtung: Distanz liegt schon in Pixeln vor
	distPix = sqrt((double) (x2 + y2 + z2));
	distReal = distPix * pixelRatio;
	// Zeit liegt in Sekunden vor. m/s / m = s
	time = velocity / distReal;

	// Bewegung in bestimmten Abschnitten
	/**
	 * Aus der Zeit und der Entfernung läßt sich nun berechnen, welche Distanz das Objekt in einer
	 * Zeiteinheit zurücklegt. Gesucht sind die Pixel pro Zeiteinheit.
	 */

	diffX = sqrt((double) x2) / (time * interval);
	diffY = sqrt((double) y2) / (time * interval);
	diffZ = sqrt((double) z2) / (time * interval);

	int currentX, currentY, currentZ;
	int temp = (int) time;
	for (int i = 0; i < 1000; i++)
	{
		/**
		* Die nächste Position des Objektes muss stetig neu berechnet werden, abhängig
		* von der bisher vergangenen Zeit und zurückgelegten Wegstrecke
		*/
		currentX = (int) (startX + (i * diffX));
		currentY = (int) (startY + (i * diffY));
		currentZ = (int) (startZ + (i * diffZ));
		SendPacketToServer(intOC, PACKET_OBJECT_NEW_OR_MODIFIED , currentX, currentY, currentZ);
//		System::Threading::Thread::Sleep(1000 / (int)interval);
	}
}

void Vehicle::setPosition(int x, int y, int z)
{
	SendPacketToServer(intOC, PACKET_OBJECT_NEW_OR_MODIFIED , x, y ,z);
}
void Vehicle::removeFromClient(int oc)
{
	SendPacketToServer(oc, PACKET_OBJECT_REMOVED, 0, 0, 0);
}
void Vehicle::SendPacketToServer(int objID, int objHandle, int x, int y, int z)
{
	if (vehicleClient->IsConnected())
	{
		// Datenstrom zum Server
		RakNet::BitStream dataStream;

		// When writing a timestamp, the first byte is ID_TIMESTAMP
		// The next 4 bytes is the timestamp itself.
		dataStream.Write((unsigned char)ID_TIMESTAMP);
		unsigned int mtime=RakNet::GetTime();
		dataStream.Write(mtime);

		dataStream.Write(PACKET_ID_CAR);
		dataStream.Write(objID);
		dataStream.Write(objHandle);
		dataStream.Write(x);
		dataStream.Write(y);
		dataStream.Write(z);

		vehicleClient->Send(&dataStream, HIGH_PRIORITY, RELIABLE_ORDERED, objID);

		printf("Sending packet from client at time %i\n", mtime);
	}
}
