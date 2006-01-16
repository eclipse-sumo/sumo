#ifndef DFDETECTOR_h
#define DFDETECTOR_h

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <map>
#include <string>
#include <vector>



enum dfdetector_type
{
    TYPE_NOT_DEFINED = 0,
	BETWEEN_DETECTOR,
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
	void setPos(const SUMOReal Pos ){ pos = Pos; };
	void setType(const dfdetector_type T ){ type = T; };
    const std::string &getID() const { return id; };
	const std::string &getLaneID() const { return laneid; };
	SUMOReal getPos() const { return pos; };
	dfdetector_type getType() const { return type; };
protected:
	std::string id;
	std::string laneid;
	SUMOReal pos;
	dfdetector_type type;
};

class DFDetectorCon
{
public:
	DFDetectorCon();
	~DFDetectorCon();
	void addDetector(const DFDetector & dfd );
    bool detectorsHaveCompleteTypes() const;
    std::vector< DFDetector > &getDetectors();
    void save(const std::string &file) const;



protected:
	std::vector< DFDetector > myDetectors;

};




#endif