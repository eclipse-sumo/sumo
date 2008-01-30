/****************************************************************************/
/// @file    NLDetectorBuilder.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 15 Apr 2002
/// @version $Id$
///
// A building helper for detectors
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
#ifndef NLDetectorBuilder_h
#define NLDetectorBuilder_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/common/PhysicalTypeDefs.h>
#include <microsim/MSNet.h>
#include <microsim/output/MSE2Collector.h>
#include <microsim/output/MSE3Collector.h>
#include <microsim/traffic_lights/MSTLLogicControl.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSInductLoop;
class MS_E2_ZS_CollectorOverLanes;
class MSTrafficLightLogic;
class MSDetectorControl;
class MELoop;
class MSLane;
class MSEdgeContinuations;

#ifdef HAVE_MESOSIM
class MEInductLoop;
class MESegment;
#endif


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NLDetectorBuilder
 * @brief A building helper for detectors
 */
class NLDetectorBuilder
{
public:

    /// Constructor
    NLDetectorBuilder(MSNet &net);

    /// Destructor
    virtual ~NLDetectorBuilder();

    /// builds an induct loop
    void buildInductLoop(const std::string &id,
                         const std::string &lane, SUMOReal pos, int splInterval,
                         OutputDevice& device, bool friendly_pos);

    /// builds a lane-based areal (E2-) detector with a fixed interval
    void buildE2Detector(const MSEdgeContinuations &edgeContinuations,
                         const std::string &id,
                         const std::string &lane, SUMOReal pos, SUMOReal length,
                         bool cont, int splInterval,
                         OutputDevice& device,
                         SUMOTime haltingTimeThreshold,
                         MetersPerSecond haltingSpeedThreshold,
                         SUMOReal jamDistThreshold);

    /// builds a lane-based areal (E2-) detector connected to a lsa
    void buildE2Detector(const MSEdgeContinuations &edgeContinuations,
                         const std::string &id,
                         const std::string &lane, SUMOReal pos, SUMOReal length,
                         bool cont, const MSTLLogicControl::TLSLogicVariants &tlls,
                         OutputDevice& device,
                         SUMOTime haltingTimeThreshold,
                         MetersPerSecond haltingSpeedThreshold,
                         SUMOReal jamDistThreshold);

    /// builds a lane-based areal (E2-) detector connected to a link's state
    void buildE2Detector(const MSEdgeContinuations &edgeContinuations,
                         const std::string &id,
                         const std::string &lane, SUMOReal pos, SUMOReal length,
                         bool cont, const MSTLLogicControl::TLSLogicVariants &tlls,
                         const std::string &tolane,
                         OutputDevice& device,
                         SUMOTime haltingTimeThreshold,
                         MetersPerSecond haltingSpeedThreshold,
                         SUMOReal jamDistThreshold);

    /// builds a multi-od (E3-) detector
    void beginE3Detector(const std::string &id,
                         OutputDevice& device, int splInterval,
                         MetersPerSecond haltingSpeedThreshold,
                         SUMOTime haltingTimeThreshold);

    /// builds an entry point of an e3 detector
    void addE3Entry(const std::string &lane, SUMOReal pos);

    /// builds an exit point of an e3 detector
    void addE3Exit(const std::string &lane, SUMOReal pos);

    /// Builds of an e3-detector using collected values
    void endE3Detector();

    /// Returns the id of the currently built e3-detector
    std::string getCurrentE3ID() const;

    /// Builds an e2-detector that lies on only one lane
    MSE2Collector *buildSingleLaneE2Det(const std::string &id,
                                        DetectorUsage usage, MSLane *lane, SUMOReal pos, SUMOReal length,
                                        SUMOTime haltingTimeThreshold,
                                        MetersPerSecond haltingSpeedThreshold,
                                        SUMOReal jamDistThreshold);

    /// Builds an e2-detector that continues on preceeding lanes
    MS_E2_ZS_CollectorOverLanes *buildMultiLaneE2Det(const MSEdgeContinuations &edgeContinuations,
            const std::string &id,
            DetectorUsage usage, MSLane *lane, SUMOReal pos, SUMOReal length,
            SUMOTime haltingTimeThreshold,
            MetersPerSecond haltingSpeedThreshold,
            SUMOReal jamDistThreshold);

    /// Creates the instance of an induct loop (overwritten by gui version)
    virtual MSInductLoop *createInductLoop(const std::string &id,
                                           MSLane *lane, SUMOReal pos);

#ifdef HAVE_MESOSIM
    /// Creates the instance of an induct loop (overwritten by gui version)
    virtual MEInductLoop *createMEInductLoop(const std::string &id,
            MESegment *s, SUMOReal pos, int splInterval);
#endif

    /// Creates the instance of a single-lane-e2-detector (overwritten by gui version)
    virtual MSE2Collector *createSingleLaneE2Detector(const std::string &id,
            DetectorUsage usage, MSLane *lane, SUMOReal pos, SUMOReal length,
            SUMOTime haltingTimeThreshold,
            MetersPerSecond haltingSpeedThreshold,
            SUMOReal jamDistThreshold);

    /// Creates the instance of a multi-lane-e2-detector (overwritten by gui version)
    virtual MS_E2_ZS_CollectorOverLanes *createMultiLaneE2Detector(
        const std::string &id, DetectorUsage usage, MSLane *lane, SUMOReal pos,
        SUMOTime haltingTimeThreshold,
        MetersPerSecond haltingSpeedThreshold,
        SUMOReal jamDistThreshold);

    /// Creates the instance of an e3-detector (overwritten by gui version)
    virtual MSE3Collector *createE3Detector(const std::string &id,
                                            const CrossSectionVector &entries,
                                            const CrossSectionVector &exits,
                                            MetersPerSecond haltingSpeedThreshold,
                                            SUMOTime haltingTimeThreshold);

    /**
     * @class E3DetectorDefinition
     * This class holds the incoming definitions of an e3 detector unless
     *  the detector is build.
     */
    class E3DetectorDefinition
    {
    public:
        /// Constructor
        E3DetectorDefinition(const std::string &id,
                             OutputDevice& device,
                             MetersPerSecond haltingSpeedThreshold,
                             SUMOTime haltingTimeThreshold,
                             int splInterval);

        /// Destructor
        ~E3DetectorDefinition();

        /// The id of the detector
        std::string myID;
        /// The device the detector shall use
        OutputDevice& myDevice;
        /// @name further detector descriptions
        //@{
        MetersPerSecond myHaltingSpeedThreshold;
        SUMOTime myHaltingTimeThreshold;
        CrossSectionVector myEntries;
        CrossSectionVector myExits;
        int mySampleInterval;
        //@}

    private:
        /// @brief Invalidated copy constructor.
        E3DetectorDefinition(const E3DetectorDefinition&);

        /// @brief Invalidated assignment operator.
        E3DetectorDefinition& operator=(const E3DetectorDefinition&);

    };

protected:
    /** @brief Returns the named lane;
        throws an exception if the lane does not exist */
    MSLane *getLaneChecking(const std::string &id,
                            const std::string &detid);

    /// Converts the length and the position information for an uncontiuating detector
    void convUncontE2PosLength(const std::string &id, MSLane *clane,
                               SUMOReal &pos, SUMOReal &length);

    /// Converts the length and the position information for an contiuating detector
    void convContE2PosLength(const std::string &id, MSLane *clane,
                             SUMOReal &pos, SUMOReal &length);

protected:
    /// The net to fill
    MSNet &myNet;

private:
    /// definition of the currently parsed e3-detector
    E3DetectorDefinition *myE3Definition;

private:
    /// @brief Invalidated copy constructor.
    NLDetectorBuilder(const NLDetectorBuilder&);

    /// @brief Invalidated assignment operator.
    NLDetectorBuilder& operator=(const NLDetectorBuilder&);

};


#endif

/****************************************************************************/

