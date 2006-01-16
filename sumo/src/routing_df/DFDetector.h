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
    void setID(const std::string &Id ){ id = Id; };
	void setLaneID(const std::string &laneId ){ laneid = laneId; };
	void setPos(const int Pos ){ pos = Pos; };
	void setType(const dfdetector_type T ){ type = T; };
    const std::string &getID() const { return id; };
	const std::string &getLaneID() const { return laneid; };
	int getPos() const { return pos; };
	dfdetector_type getType() const { return type; };
protected:
	std::string id;
	std::string laneid;
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