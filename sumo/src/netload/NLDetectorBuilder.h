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
// Revision 1.10  2004/02/18 05:32:51  dkrajzew
// missing pass of lane continuation to detector builder added
//
// Revision 1.9  2004/02/16 13:49:08  dkrajzew
// loading of e2-link-dependent detectors added
//
// Revision 1.8  2004/01/26 07:07:36  dkrajzew
// work on detectors: e3-detectors loading and visualisation; variable offsets and lengths for lsa-detectors; coupling of detectors to tl-logics; different detector visualistaion in dependence to his controller
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
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <microsim/MSNet.h>
#include <microsim/MSE2Collector.h>
#include <microsim/MSE3Collector.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSInductionLoop;
class MS_E2_ZS_CollectorOverLanes;
class MSTrafficLightLogic;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class NLDetectorBuilder
 * This class builds the detectors from their descriptions
 */
class NLDetectorBuilder {
public:
    /// Definitions of a string vector
    typedef std::vector<std::string> StringVector;

    /// Definition of a map from string -> stringvector
    typedef std::map<std::string, StringVector> SSVMap;

public:
    /// Constructor
    NLDetectorBuilder();

    /// Destructor
    virtual ~NLDetectorBuilder();

    /// builds an induct loop
    void buildInductLoop(const std::string &id,
        const std::string &lane, float pos, int splInterval,
        const std::string &style, std::string filename);

    /// builds a lane-based areal (E2-) detector with a fixed interval
    void buildE2Detector(const SSVMap &laneConts,
        const std::string &id,
        const std::string &lane, float pos, float length,
        bool cont, int splInterval,
        const std::string &/*style*/, std::string filename,
        const std::string &measures,
        MSUnit::Seconds haltingTimeThreshold = 1,
        MSUnit::MetersPerSecond haltingSpeedThreshold =5.0/3.6,
        MSUnit::Meters jamDistThreshold = 10,
        MSUnit::Seconds deleteDataAfterSeconds = 1800);

    /// builds a lane-based areal (E2-) detector connected to a lsa
    void buildE2Detector(const SSVMap &laneConts,
        const std::string &id,
        const std::string &lane, float pos, float length,
        bool cont, MSTrafficLightLogic *tll,
        const std::string &/*style*/, std::string filename,
        const std::string &measures,
        MSUnit::Seconds haltingTimeThreshold = 1,
        MSUnit::MetersPerSecond haltingSpeedThreshold =5.0/3.6,
        MSUnit::Meters jamDistThreshold = 10,
        MSUnit::Seconds deleteDataAfterSeconds = 1800);

    /// builds a lane-based areal (E2-) detector connected to a link's state
    void buildE2Detector(const SSVMap &laneConts,
        const std::string &id,
        const std::string &lane, float pos, float length,
        bool cont, MSTrafficLightLogic *tll,
        const std::string &tolane,
        const std::string &style, std::string filename,
        const std::string &measures,
        MSUnit::Seconds haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        MSUnit::Meters jamDistThreshold,
        MSUnit::Seconds deleteDataAfterSeconds );

    /// builds a multi-od (E3-) detector
    void beginE3Detector(const std::string &id,
        std::string filename, int splInterval,
        const std::string &measures,
        MSUnit::Seconds haltingTimeThreshold = 1,
        MSUnit::MetersPerSecond haltingSpeedThreshold =5.0/3.6,
        MSUnit::Seconds deleteDataAfterSeconds = 1800);

    void addE3Entry(const std::string &lane, float pos);
    void addE3Exit(const std::string &lane, float pos);

    /// Builds of an e3-detector using collected values
    void endE3Detector();

protected:
    /// Makes some data conversion and calls the propriate building function
    MSDetectorFileOutput* buildE2(const std::string &id,
        const std::string &lane, float pos, float length, bool cont,
        const std::string &/*style*/, std::string filename,
        const std::string &basePath, const std::string &measures,
        MSUnit::Seconds haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        MSUnit::Meters jamDistThreshold,
        MSUnit::Seconds deleteDataAfterSeconds );


    /// Builds an e2-detector that lies on only one lane
    MSDetectorFileOutput *buildSingleLaneE2Det(const std::string &id,
        DetectorUsage usage, MSLane *lane, float pos, float length,
        int splInterval,
        MSUnit::Seconds haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        MSUnit::Meters jamDistThreshold,
        MSUnit::Seconds deleteDataAfterSeconds,
        const std::string &measures);

    /// Builds an e2-detector that continues on preceeding lanes
    MSDetectorFileOutput *buildMultiLaneE2Det(const SSVMap &laneConts,
        const std::string &id,
        DetectorUsage usage, MSLane *lane, float pos, float length,
        int splInterval,
        MSUnit::Seconds haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        MSUnit::Meters jamDistThreshold,
        MSUnit::Seconds deleteDataAfterSeconds,
        const std::string &measures);

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
        MSLane *lane, double pos);

    /// Creates the instance of a single-lane-e2-detector (overwritten by gui version)
    virtual MSE2Collector *createSingleLaneE2Detector(const std::string &id,
        DetectorUsage usage, MSLane *lane, float pos, float length,
        MSUnit::Seconds haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        MSUnit::Meters jamDistThreshold,
        MSUnit::Seconds deleteDataAfterSeconds);

    /// Creates the instance of a multi-lane-e2-detector (overwritten by gui version)
    virtual MS_E2_ZS_CollectorOverLanes *createMultiLaneE2Detector(
        const std::string &id, DetectorUsage usage, MSLane *lane, float pos,
        MSUnit::Seconds haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        MSUnit::Meters jamDistThreshold,
        MSUnit::Seconds deleteDataAfterSeconds);

    /// Creates the instance of an e3-detector (overwritten by gui version)
    virtual MSE3Collector *createE3Detector(const std::string &id,
        const Detector::CrossSections &entries,
        const Detector::CrossSections &exits,
        MSUnit::Seconds haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        MSUnit::Seconds deleteDataAfterSeconds);

    // converts the name of an output style into it's enumeration value
/*     static MSDetector::OutputStyle convertStyle(const std::string &id,
         const std::string &style);*/

    class E3DetectorDefinition {
    public:
        E3DetectorDefinition(const std::string &id,
            const std::string &filename,
            MSUnit::Seconds haltingTimeThreshold,
            MSUnit::MetersPerSecond haltingSpeedThreshold,
            MSUnit::Seconds deleteDataAfterSeconds,
            const E3MeasuresVector &measures,
            int splInterval);

        ~E3DetectorDefinition();

        std::string myID;
        std::string myFileName;
        MSUnit::Seconds myHaltingTimeThreshold;
        MSUnit::MetersPerSecond myHaltingSpeedThreshold;
        MSUnit::Seconds myDeleteDataAfterSeconds;
        E3MeasuresVector myMeasures;
        Detector::CrossSections myEntries;
        Detector::CrossSections myExits;
        int mySampleInterval;

    };

protected:
    /// @brief Returns the named lane; throws an exception if the lane does not exist
    MSLane *getLaneChecking(const std::string &id);

    /// Converts the length and the position information for an uncontiuating detector
    void convUncontE2PosLength(const std::string &id, MSLane *clane,
        float &pos, float &length);

    /// Converts the length and the position information for an contiuating detector
    void convContE2PosLength(const std::string &id, MSLane *clane,
        float &pos, float &length);

private:
    E3DetectorDefinition *myE3Definition;

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
//
