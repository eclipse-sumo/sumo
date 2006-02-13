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
#include <map>
#include <utils/common/SUMOTime.h>
#include <utils/helpers/RandomDistributor.h>

class DFRORouteCont;
class DFDetectorFlows;
class ROEdge;
class DFDetectorCon;


enum dfdetector_type
{
    TYPE_NOT_DEFINED = 0,
	DISCARDED_DETECTOR,
	BETWEEN_DETECTOR,
	SOURCE_DETECTOR,
	HIGHWAY_SOURCE_DETECTOR,
	SINK_DETECTOR,
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
	void writeEmitterDefinition(const std::string &file,
        const DFDetectorCon &detectors, const DFDetectorFlows &flows,
		SUMOTime startTime, SUMOTime endTime, SUMOTime stepOffset) const;
	void addRoutes(DFRORouteCont *routes);
	bool hasRoutes() const;
	bool writeRoutes(std::vector<std::string> &saved,
		std::ostream &os);

protected:
    void buildDestinationDistribution(const DFDetectorCon &detectors,
        const DFDetectorFlows &flows,
        SUMOTime startTime, SUMOTime endTime, SUMOTime stepOffset,
        std::map<size_t, RandomDistributor<size_t>* > &into) const;
    int getFlowFor(const ROEdge *edge, SUMOTime time) const;

protected:
	std::string myID;
	std::string myLaneID;
	SUMOReal myPosition;
	dfdetector_type myType;
	DFRORouteCont *myRoutes;

};

class DFDetectorCon
{
public:
	DFDetectorCon();
	~DFDetectorCon();
	//bool isDetector(std::string id);
	bool addDetector(DFDetector *dfd );
    bool detectorsHaveCompleteTypes() const;
    bool detectorsHaveRoutes() const;
    const std::vector<DFDetector*> &getDetectors() const;
    void save(const std::string &file) const;
	void saveAsPOIs(const std::string &file) const;
	void saveRoutes(const std::string &file) const;

	const DFDetector &getDetector(const std::string &id) const;
	void writeEmitters(const std::string &file,
		const DFDetectorFlows &flows,
		SUMOTime startTime, SUMOTime endTime, SUMOTime stepOffset,
		bool writeCalibrators);

    int getFlowFor(const ROEdge *edge, SUMOTime time,
        const DFDetectorFlows &flows) const;



protected:
	std::vector<DFDetector*> myDetectors;
	std::map<std::string, DFDetector*> myDetectorMap;
    std::map<std::string, std::vector<DFDetector*> > myDetectorEdgeMap;

};




#endif