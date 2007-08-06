/****************************************************************************/
/// @file    DFDetector.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 16.03.2006
/// @version $Id$
///
// Class representing a detector within the DFROUTER
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef DFDetector_h
#define DFDetector_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <map>
#include <string>
#include <vector>
#include <map>
#include <utils/common/SUMOTime.h>
#include <utils/helpers/RandomDistributor.h>
#include "DFRORouteCont.h"


// ===========================================================================
// class declarations
// ===========================================================================
class DFRORouteCont;
class DFDetectorFlows;
class ROEdge;
class DFDetectorCon;
class RODFNet;
struct DFRORouteDesc;


// ===========================================================================
// enumerations
// ===========================================================================
/**
 * @enum RODFDetectorType
 * @brief Numerical representation of different detector types
 */
enum RODFDetectorType
{
    /// A not yet defined detector
    TYPE_NOT_DEFINED = 0,

    /// A detector which had to be discarded (!!!)
    DISCARDED_DETECTOR,

    /// An in-between detector
    BETWEEN_DETECTOR,

    /// A source detector
    SOURCE_DETECTOR,
    SINK_DETECTOR
};


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class DFDetector
 * @brief Class representing a detector within the DFROUTER
 */
class DFDetector
{
public:
    DFDetector(const std::string &Id, const std::string &laneId,
               SUMOReal pos, const RODFDetectorType type);
    DFDetector(const std::string &Id, const DFDetector &f);
    ~DFDetector();
    const std::string &getID() const
    {
        return myID;
    };
    const std::string &getLaneID() const
    {
        return myLaneID;
    };
    SUMOReal getPos() const
    {
        return myPosition;
    };
    RODFDetectorType getType() const
    {
        return myType;
    };
    void setType(RODFDetectorType type);
    bool writeEmitterDefinition(const std::string &file,
                                const DFDetectorCon &detectors, const DFDetectorFlows &flows,
                                SUMOTime startTime, SUMOTime endTime, SUMOTime stepOffset,
                                const RODFNet &net,
                                bool includeUnusedRoutes, SUMOReal scale, int maxFollower,
                                bool emissionsOnly) const;

    void addRoute(const RODFNet &net, DFRORouteDesc *nrd);
    void addRoutes(DFRORouteCont *routes);
    bool hasRoutes() const;
    bool writeRoutes(std::vector<std::string> &saved,
                     std::ostream &os);
    void writeSingleSpeedTrigger(const std::string &file,
                                 const DFDetectorFlows &flows,
                                 SUMOTime startTime, SUMOTime endTime, SUMOTime stepOffset);
    void writeEndRerouterDetectors(const std::string &file);
    const std::vector<DFRORouteDesc*> &getRouteVector() const;
    void addPriorDetector(DFDetector *det);
    void addFollowingDetector(DFDetector *det);
    const std::vector<DFDetector*> &getPriorDetectors() const;
    const std::vector<DFDetector*> &getFollowerDetectors() const;
    SUMOReal getUsage(const DFDetectorCon &detectors,DFRORouteDesc*route, DFRORouteCont::RoutesMap *curr,
                     SUMOTime time, const DFDetectorFlows &flows) const;

protected:
    void buildDestinationDistribution(const DFDetectorCon &detectors,
                                      const DFDetectorFlows &flows,
                                      SUMOTime startTime, SUMOTime endTime, SUMOTime stepOffset,
                                      const RODFNet &net,
                                      std::map<size_t, RandomDistributor<size_t>* > &into,
                                      int maxFollower) const;
    int getFlowFor(const ROEdge *edge, SUMOTime time) const;
    SUMOReal computeDistanceFactor(const DFRORouteDesc &rd) const;

protected:
    std::string myID;
    std::string myLaneID;
    SUMOReal myPosition;
    RODFDetectorType myType;
    DFRORouteCont *myRoutes;
    std::vector<DFDetector*> myPriorDetectors, myFollowingDetectors;

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
    bool addDetector(DFDetector *dfd);
    void removeDetector(const std::string &id);
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
                       const RODFNet &net,
                       bool writeCalibrators, bool includeUnusedRoutes,
                       SUMOReal scale, int maxFollower,
                       bool emissionsOnly);

    void writeEmitterPOIs(const std::string &file,
                          const DFDetectorFlows &flows,
                          SUMOTime startTime, SUMOTime endTime, SUMOTime stepOffset);

    void writeSpeedTrigger(const std::string &file,
                           const DFDetectorFlows &flows,
                           SUMOTime startTime, SUMOTime endTime, SUMOTime stepOffset);

    void writeValidationDetectors(const std::string &file,
                                  bool includeSources, bool singleFile, bool friendly);
    void writeEndRerouterDetectors(const std::string &file);

    int getFlowFor(const ROEdge *edge, SUMOTime time,
                   const DFDetectorFlows &flows) const;

    int getAggFlowFor(const ROEdge *edge, SUMOTime time, SUMOTime period,
                      const DFDetectorFlows &flows) const;

    void guessEmptyFlows(DFDetectorFlows &flows);

    void mesoJoin(const std::string &nid, const std::vector<std::string> &oldids);


protected:
    std::vector<DFDetector*> myDetectors;
    std::map<std::string, DFDetector*> myDetectorMap;
    std::map<std::string, std::vector<DFDetector*> > myDetectorEdgeMap;

};


#endif

/****************************************************************************/

