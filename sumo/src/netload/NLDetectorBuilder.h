#ifndef NLDetectorBuilder_h
#define NLDetectorBuilder_h
/***************************************************************************
                          NLDetectorBuilder.h
                          A building helper for the detectors
                             -------------------
    begin                : Mon, 15 Apr 2002
    copyright            : (C) 2001 by DLR http://ivf.dlr.de/
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
// Revision 1.16  2005/09/15 12:04:36  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.15  2005/07/12 12:36:12  dkrajzew
// made errors on detector building more readable
//
// Revision 1.14  2005/05/04 08:39:46  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.13  2004/12/16 12:23:03  dkrajzew
// got rid of an unnecessary detector parameter/debugging
//
// Revision 1.12  2004/11/23 10:12:46  dkrajzew
// new detectors usage applied
//
// Revision 1.11  2004/04/02 11:23:52  dkrajzew
// extended traffic lights are now no longer templates; MSNet now handles
//  all simulation-wide output
//
// Revision 1.10  2004/02/18 05:32:51  dkrajzew
// missing pass of lane continuation to detector builder added
//
// Revision 1.9  2004/02/16 13:49:08  dkrajzew
// loading of e2-link-dependent detectors added
//
// Revision 1.8  2004/01/26 07:07:36  dkrajzew
// work on detectors: e3-detectors loading and visualisation; variable offsets
//  and lengths for lsa-detectors; coupling of detectors to tl-logics;
//  different detector visualistaion in dependence to his controller
//
// Revision 1.7  2004/01/12 14:46:21  dkrajzew
// handling of e2-detectors within the gui added
//
// Revision 1.6  2004/01/12 14:37:32  dkrajzew
// reading of e2-detectors from files added
//
// Revision 1.5  2003/07/22 15:12:16  dkrajzew
// new usage of detectors applied
//
// Revision 1.4  2003/07/21 18:07:44  roessel
// Adaptions due to new MSInductLoop.
//
// Revision 1.3  2003/07/18 12:35:05  dkrajzew
// removed some warnings
//
// Revision 1.2  2003/02/07 11:18:56  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 15:36:47  dkrajzew
// moved from ROOT/sumo/netload to ROOT/src/netload; new format definition
// parseable in one step
//
// Revision 1.6  2002/07/31 17:34:50  roessel
// Changes since sourceforge cvs request.
//
// Revision 1.6  2002/07/26 10:49:42  dkrajzew
// Detector-output destination may now be specified using relative pathnames
//
// Revision 1.5  2002/06/11 14:39:24  dkrajzew
// windows eol removed
//
// Revision 1.4  2002/06/11 13:44:33  dkrajzew
// Windows eol removed
//
// Revision 1.3  2002/04/17 11:17:01  dkrajzew
// windows-newlines removed
//
// Revision 1.2  2002/04/16 06:50:20  dkrajzew
// documentation added; coding standard attachements added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <string>
#include <microsim/MSNet.h>
#include <microsim/output/e2_detectors/MSE2Collector.h>
#include <microsim/output/e3_detectors/MSE3Collector.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSInductLoop;
class MS_E2_ZS_CollectorOverLanes;
class MSTrafficLightLogic;
class MSDetectorControl;
class MELoop;
class MSLane;

#ifdef HAVE_MESOSIM
class MEInductLoop;
class MESegment;
#endif


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class NLDetectorBuilder
 * This class builds detectors and stores them within the given net
 */
class NLDetectorBuilder {
public:
    /// Definitions of a string vector
    typedef std::vector<std::string> StringVector;

    /// Definition of a map from string -> stringvector
    typedef std::map<std::string, StringVector> SSVMap;

public:

    /// Constructor
    NLDetectorBuilder(MSNet &net);

    /// Destructor
    virtual ~NLDetectorBuilder();

    /// builds an induct loop
    void buildInductLoop(const std::string &id,
        const std::string &lane, double pos, int splInterval,
        OutputDevice *device, const std::string &style="");

    /// builds a lane-based areal (E2-) detector with a fixed interval
    void buildE2Detector(const SSVMap &laneConts,
        const std::string &id,
        const std::string &lane, double pos, double length,
        bool cont, int splInterval,
        const std::string &/*style*/, OutputDevice *device,
        const std::string &measures,
        MSUnit::Seconds haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        MSUnit::Meters jamDistThreshold,
        SUMOTime deleteDataAfterSeconds);

    /// builds a lane-based areal (E2-) detector connected to a lsa
    void buildE2Detector(const SSVMap &laneConts,
        const std::string &id,
        const std::string &lane, double pos, double length,
        bool cont, MSTrafficLightLogic * const tll,
        const std::string &/*style*/, OutputDevice *device,
        const std::string &measures,
        MSUnit::Seconds haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        MSUnit::Meters jamDistThreshold,
        SUMOTime deleteDataAfterSeconds);

    /// builds a lane-based areal (E2-) detector connected to a link's state
    void buildE2Detector(const SSVMap &laneConts,
        const std::string &id,
        const std::string &lane, double pos, double length,
        bool cont, MSTrafficLightLogic * const tll,
        const std::string &tolane,
        const std::string &style, OutputDevice *device,
        const std::string &measures,
        MSUnit::Seconds haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        MSUnit::Meters jamDistThreshold,
        SUMOTime deleteDataAfterSeconds );

    /// builds a multi-od (E3-) detector
    void beginE3Detector(const std::string &id,
        OutputDevice *device, int splInterval,
        const std::string &measures,
        MSUnit::Seconds haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        SUMOTime deleteDataAfterSeconds);

    /// builds an entry point of an e3 detector
    void addE3Entry(const std::string &lane, double pos);

    /// builds an exit point of an e3 detector
    void addE3Exit(const std::string &lane, double pos);

    /// Builds of an e3-detector using collected values
    void endE3Detector();

    /// Makes some data conversion and calls the propriate building function
    MSDetectorFileOutput* buildE2(const std::string &id,
        const std::string &lane, double pos, double length, bool cont,
        const std::string &/*style*/, std::string filename,
        const std::string &basePath, const std::string &measures,
        MSUnit::Seconds haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        MSUnit::Meters jamDistThreshold,
        SUMOTime deleteDataAfterSeconds );


    /// Builds an e2-detector that lies on only one lane
    MSE2Collector *buildSingleLaneE2Det(const std::string &id,
        DetectorUsage usage, MSLane *lane, double pos, double length,
        MSUnit::Seconds haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        MSUnit::Meters jamDistThreshold,
        SUMOTime deleteDataAfterSeconds,
        const std::string &measures);

    /// Builds an e2-detector that continues on preceeding lanes
    MS_E2_ZS_CollectorOverLanes *buildMultiLaneE2Det(const SSVMap &laneConts,
        const std::string &id,
        DetectorUsage usage, MSLane *lane, double pos, double length,
        MSUnit::Seconds haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        MSUnit::Meters jamDistThreshold,
        SUMOTime deleteDataAfterSeconds,
        const std::string &measures="all");

    /// Definition of an E2-measures vector
    typedef std::vector<E2::DetType> E2MeasuresVector;

    /// Parses the measures an E2-detector shall compute
    E2MeasuresVector parseE2Measures(const std::string &measures);

    /// Definition of an E2-measures vector
    typedef std::vector<E3::DetType> E3MeasuresVector;

    /// Parses the measures an E3-detector shall compute
    E3MeasuresVector parseE3Measures(const std::string &measures);

    /// Creates the instance of an induct loop (overwritten by gui version)
    virtual MSInductLoop *createInductLoop(const std::string &id,
        MSLane *lane, double pos, int splInterval);

#ifdef HAVE_MESOSIM
    /// Creates the instance of an induct loop (overwritten by gui version)
    virtual MEInductLoop *createMEInductLoop(const std::string &id,
        MESegment *s, double pos, int splInterval);
#endif

    /// Creates the instance of a single-lane-e2-detector (overwritten by gui version)
    virtual MSE2Collector *createSingleLaneE2Detector(const std::string &id,
        DetectorUsage usage, MSLane *lane, double pos, double length,
        MSUnit::Seconds haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        MSUnit::Meters jamDistThreshold,
        SUMOTime deleteDataAfterSeconds);

    /// Creates the instance of a multi-lane-e2-detector (overwritten by gui version)
    virtual MS_E2_ZS_CollectorOverLanes *createMultiLaneE2Detector(
        const std::string &id, DetectorUsage usage, MSLane *lane, double pos,
        MSUnit::Seconds haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        MSUnit::Meters jamDistThreshold,
        SUMOTime deleteDataAfterSeconds);

    /// Creates the instance of an e3-detector (overwritten by gui version)
    virtual MSE3Collector *createE3Detector(const std::string &id,
        const Detector::CrossSections &entries,
        const Detector::CrossSections &exits,
        MSUnit::Seconds haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        SUMOTime deleteDataAfterSeconds);

    // converts the name of an output style into it's enumeration value
/*     static MSDetector::OutputStyle convertStyle(const std::string &id,
         const std::string &style);*/

    /**
     * @class E3DetectorDefinition
     * This class holds the incoming definitions of an e3 detector unless
     *  the detector is build.
     */
    class E3DetectorDefinition {
    public:
        /// Constructor
        E3DetectorDefinition(const std::string &id,
            OutputDevice *device,
            MSUnit::Seconds haltingTimeThreshold,
            MSUnit::MetersPerSecond haltingSpeedThreshold,
            SUMOTime deleteDataAfterSeconds,
            const E3MeasuresVector &measures,
            int splInterval);

        /// Destructor
        ~E3DetectorDefinition();

        /// The id of the detector
        std::string myID;
        /// The device the detector shall use
        OutputDevice *myDevice;
        //{ further detector descriptions
        MSUnit::Seconds myHaltingTimeThreshold;
        MSUnit::MetersPerSecond myHaltingSpeedThreshold;
        SUMOTime myDeleteDataAfterSeconds;
        E3MeasuresVector myMeasures;
        Detector::CrossSections myEntries;
        Detector::CrossSections myExits;
        int mySampleInterval;
        //}

    };

protected:
    /** @brief Returns the named lane;
        throws an exception if the lane does not exist */
    MSLane *getLaneChecking(const std::string &id,
        const std::string &detid);

    /// Converts the length and the position information for an uncontiuating detector
    void convUncontE2PosLength(const std::string &id, MSLane *clane,
        double pos, double length);

    /// Converts the length and the position information for an contiuating detector
    void convContE2PosLength(const std::string &id, MSLane *clane,
        double pos, double length);

protected:
    /// The net to fill
    MSNet &myNet;

private:
    /// definition of the currently parsed e3-detector
    E3DetectorDefinition *myE3Definition;

};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
//
