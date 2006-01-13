#ifndef DFDETECTOR_h
#define DFDETECTOR_h

#include <map>
enum dfdetector_type
{
	BETWEEN_DETECTOR = 0,
	SOURCE_DETECTOR,
	HIGHWAY_SOURCE_DETECTOR,
	SINK_DETEKTOR,
	HIGHWAY_SINK_DETECTOR
};


class DFDetector
{
public:
	DFDetector();
	~DFDetector();
	void setID(const int Id ){ id = Id; };
	void setLaneID(const int laneId ){ laneid = laneId; };
	void setPos(const int Pos ){ pos = Pos; };
	void setType(const dfdetector_type T ){ type = T; };
	int getID(){ return id; };
	int getLaneID(){ return laneid; };
	int getPos(){ return pos; };
	dfdetector_type getType(){ return type; };
protected:
	int id;
	int laneid;
	int pos;
	dfdetector_type type;
};

class DFDetectorCon
{
public:
	DFDetectorCon();
	~DFDetectorCon();
	void addDetector(const int &id, const DFDetector & dfd );

protected:
	std::map< int, DFDetector > _detmap;

};




#endif