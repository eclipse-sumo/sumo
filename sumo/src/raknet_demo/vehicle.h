#pragma once

class Vehicle
{
	public:
		// Konstruktor
		Vehicle(void);

		// Dekonstruktor
		~Vehicle(void);

		static const char SERVER_IP;

		// Methoden
		void setStartPosition(int x, int y, int z);
		void setDestPosition(int x, int y, int z);
		void setViaPoint(int x, int y, int z);
		void setVelocity(double speed);
		void calculateRoute();
		void setPosition(int x, int y, int z);
		static void removeFromClient(int oc);

		// Objektzähler
		static unsigned int objectCounter;

	public:
		static void SendPacketToServer(int objID, int objHandle, int x, int y, int z);

		int intOC;
		unsigned int time;
};
