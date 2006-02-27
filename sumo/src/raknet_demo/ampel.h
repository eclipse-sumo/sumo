#pragma once

class Ampel
{
	public:
		// Konstruktor
		Ampel(void);

		// Dekonstruktor
		~Ampel(void);

		static const char SERVER_IP = "127.0.0.1";

		// Methoden
		void addTrafficLight(int objectID, float x, float y, float z, float rot);
		void setTrafficLightState(int objectID, int state);

		// Objektzähler
		static unsigned int objectCounter;

	private:
		void SendPacketToServer(int objID, int objHandle, int x, int y, int z);

		int intOC;
		unsigned int time;
};