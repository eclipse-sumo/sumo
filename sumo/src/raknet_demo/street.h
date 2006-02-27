#pragma once

// namespaces in Irrlicht
//using namespace irr;

class Street
{
public:
	// Konstruktor
	Street(void);

	// Dekonstruktor
	~Street(void);

	// Methoden
	void addLane(int objectID, int noPoints, float *streetPointsX,
		float *streetPointsZ);

private:
	/*
	void SendPacketToServer(s32 objectID, f32 startX,
	f32 startZ, f32 destX, f32 destZ, f32 midX, f32 midZ, f32 dx, f32 dz);
*/
	static const char SERVER_IP;

	unsigned int time;
};