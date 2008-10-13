/****************************************************************************/
/// @file    RODFDetector.h
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
#ifndef RODFDetector_h
#define RODFDetector_h


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
#include <utils/common/RandomDistributor.h>
#include "DFRORouteCont.h"


// ===========================================================================
// class declarations
// ===========================================================================
class DFRORouteCont;
class RODFDetectorFlows;
class ROEdge;
class RODFDetectorCon;
class RODFNet;
struct DFRORouteDesc;
class OutputDevice;


// ===========================================================================
// enumerations
// ===========================================================================
/**
 * @enum RORODFDetectorType
 * @brief Numerical representation of different detector types
 */
enum RORODFDetectorType {
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
 * @class RODFDetector
 * @brief Class representing a detector within the DFROUTER
 */
class RODFDetector
{
public:
    RODFDetector(const std::string &Id, const std::string &laneId,
                 SUMOReal pos, const RORODFDetectorType type);
    RODFDetector(const std::string &Id, const RODFDetector &f);
    ~RODFDetector();
    const std::string &getID() const {
        return myID;
    };
    const std::string &getLaneID() const {
        return myLaneID;
    };
    SUMOReal getPos() const {
        return myPosition;
    };
    RORODFDetectorType getType() const {
        return myType;
    };
    void setType(RORODFDetectorType type);
    bool writeEmitterDefinition(const std::string &file,
                                const RODFDetectorCon &detectors, const RODFDetectorFlows &flows,
                                SUMOTime startTime, SUMOTime endTime, SUMOTime stepOffset,
                                const RODFNet &net,
                                bool includeUnusedRoutes, SUMOReal scale, int maxFollower,
                                bool emissionsOnly) const;

    void addRoute(const RODFNet &net, DFRORouteDesc &nrd);
    void addRoutes(DFRORouteCont *routes);
    bool hasRoutes() const;
    bool writeRoutes(std::vector<std::string> &saved,
                     OutputDevice& out);
    void writeSingleSpeedTrigger(const std::string &file,
                                 const RODFDetectorFlows &flows,
                                 SUMOTime startTime, SUMOTime endTime, SUMOTime stepOffset);
    void writeEndRerouterDetectors(const std::string &file);
    const std::vector<DFRORouteDesc> &getRouteVector() const;
    void addPriorDetector(RODFDetector *det);
    void addFollowingDetector(RODFDetector *det);
    const std::vector<RODFDetector*> &getPriorDetectors() const;
    const std::vector<RODFDetector*> &getFollowerDetectors() const;
    SUMOReal getUsage(const RODFDetectorCon &detectors,const DFRORouteDesc &route, DFRORouteCont::RoutesMap *curr,
                      SUMOTime time, const RODFDetectorFlows &flows) const;

protected:
    void buildDestinationDistribution(const RODFDetectorCon &detectors,
                                      const RODFDetectorFlows &flows,
                                      SUMOTime startTime, SUMOTime endTime, SUMOTime stepOffset,
                                      const RODFNet &net,
                                      std::map<size_t, RandomDistributor<size_t>* > &into,
                                      int maxFollower) const;
    int getFlowFor(const ROEdge *edge, SUMOTime time) const;
    SUMOReal computeDistanceFactor(const DFRORouteDesc &rd) const;


    /** @brief Clears the given distributions map, deleting the timed distributions
     * @param[in] dists The distribution map to clear
     */
    void clearDists(std::map<size_t, RandomDistributor<size_t>* > &dists) const throw();


protected:
    std::string myID;
    std::string myLaneID;
    SUMOReal myPosition;
    RORODFDetectorType myType;
    DFRORouteCont *myRoutes;
    std::vector<RODFDetector*> myPriorDetectors, myFollowingDetectors;


private:
    /// @brief Invalidated copy constructor
    RODFDetector(const RODFDetector &src);

    /// @brief Invalidated assignment operator
    RODFDetector &operator=(const RODFDetector &src);

};


/**
 * @class RODFDetectorCon
 * @brief A container for RODFDetectors
 */
class RODFDetectorCon
{
public:
    RODFDetectorCon();
    ~RODFDetectorCon();
    //bool isDetector(std::string id);
    bool addDetector(RODFDetector *dfd);
    void removeDetector(const std::string &id);
    bool detectorsHaveCompleteTypes() const;
    bool detectorsHaveRoutes() const;
    const std::vector<RODFDetector*> &getDetectors() const;
    void save(const std::string &file) const;
    void saveAsPOIs(const std::string &file) const;
    void saveRoutes(const std::string &file) const;

    const RODFDetector &getDetector(const std::string &id) const;
    void writeEmitters(const std::string &file,
                       const RODFDetectorFlows &flows,
                       SUMOTime startTime, SUMOTime endTime, SUMOTime stepOffset,
                       const RODFNet &net,
                       bool writeCalibrators, bool includeUnusedRoutes,
                       SUMOReal scale, int maxFollower,
                       bool emissionsOnly);

    void writeEmitterPOIs(const std::string &file,
                          const RODFDetectorFlows &flows,
                          SUMOTime startTime, SUMOTime endTime, SUMOTime stepOffset);

    void writeSpeedTrigger(const std::string &file,
                           const RODFDetectorFlows &flows,
                           SUMOTime startTime, SUMOTime endTime, SUMOTime stepOffset);

    void writeValidationDetectors(const std::string &file,
                                  bool includeSources, bool singleFile, bool friendly);
    void writeEndRerouterDetectors(const std::string &file);

    int getFlowFor(const ROEdge *edge, SUMOTime time,
                   const RODFDetectorFlows &flows) const;

    int getAggFlowFor(const ROEdge *edge, SUMOTime time, SUMOTime period,
                      const RODFDetectorFlows &flows) const;

    void guessEmptyFlows(RODFDetectorFlows &flows);

    void mesoJoin(const std::string &nid, const std::vector<std::string> &oldids);


protected:
    std::vector<RODFDetector*> myDetectors;
    std::map<std::string, RODFDetector*> myDetectorMap;
    std::map<std::string, std::vector<RODFDetector*> > myDetectorEdgeMap;

private:
    /// @brief Invalidated copy constructor
    RODFDetectorCon(const RODFDetectorCon &src);

    /// @brief Invalidated assignment operator
    RODFDetectorCon &operator=(const RODFDetectorCon &src);

};


#endif

/****************************************************************************/

