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
	DFDetector(const std::string &Id, const std::string &laneId,
        SUMOReal pos, const dfdetector_type type);
	~DFDetector();
    const std::string &getID() const { return myID; };
	const std::string &getLaneID() const { return myLaneID; };
	SUMOReal getPos() const { return myPosition; };
	dfdetector_type getType() const { return myType; };
    void setType(dfdetector_type type);
protected:
	std::string myID;
	std::string myLaneID;
	SUMOReal myPosition;
	dfdetector_type myType;
};

class DFDetectorCon
{
public:
	DFDetectorCon();
	~DFDetectorCon();
	bool addDetector(const DFDetector & dfd );
    bool detectorsHaveCompleteTypes() const;
    std::vector< DFDetector > &getDetectors();
    void save(const std::string &file) const;



protected:
	std::vector< DFDetector > myDetectors;

};




#endif