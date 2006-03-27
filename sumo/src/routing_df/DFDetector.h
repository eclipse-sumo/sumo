#ifndef DFDETECTOR_h
#define DFDETECTOR_h
/***************************************************************************
                          DFDetector.h
                          Class representing a detector within the DFROUTER
                             -------------------
    project              : SUMO
    begin                : Thu, 16.03.2006
    copyright            : (C) 2006 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// $Log$
// Revision 1.13  2006/03/27 07:32:15  dkrajzew
// some further work...
//
// Revision 1.12  2006/03/17 09:04:25  dkrajzew
// class-documentation added/patched
//
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
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


/* =========================================================================
 * class declarations
 * ======================================================================= */
class DFRORouteCont;
class DFDetectorFlows;
class ROEdge;
class DFDetectorCon;


/* =========================================================================
 * enumerations
 * ======================================================================= */
/**
 * @enum dfdetector_type
 * @brief Numerical representation of different detector types
 */
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


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class DFDetector
 * @brief Class representing a detector within the DFROUTER
 */
class DFDetector
{
public:
	DFDetector(const std::string &Id, const std::string &laneId,
        SUMOReal pos, const dfdetector_type type);
	DFDetector(const std::string &Id, const DFDetector &f);
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
	void writeSingleSpeedTrigger(const std::string &file,
		const DFDetectorFlows &flows,
		SUMOTime startTime, SUMOTime endTime, SUMOTime stepOffset);
    void writeEndRerouterDetectors(const std::string &file);

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


/**
 * @class DFDetectorCon
 * @brief A container for DFDetectors
 */
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

	void writeSpeedTrigger(const std::string &file,
		const DFDetectorFlows &flows,
		SUMOTime startTime, SUMOTime endTime, SUMOTime stepOffset);

	void writeValidationDetectors(const std::string &file,
        bool includeSources, bool singleFile, bool friendly);
    void writeEndRerouterDetectors(const std::string &file);

    int getFlowFor(const ROEdge *edge, SUMOTime time,
        const DFDetectorFlows &flows) const;


protected:
	std::vector<DFDetector*> myDetectors;
	std::map<std::string, DFDetector*> myDetectorMap;
    std::map<std::string, std::vector<DFDetector*> > myDetectorEdgeMap;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

