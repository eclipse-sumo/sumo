#ifndef MS_E2_ZS_CollectorOverLanes_h
#define MS_E2_ZS_CollectorOverLanes_h
//---------------------------------------------------------------------------//
//                        MS_E2_ZS_CollectorOverLanes.h -
//  A detector which joins E2Collectors over consecutive lanes (backward)
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Oct 2003
//  copyright            : (C) 2003 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.4  2005/05/04 08:11:51  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.3  2005/02/01 10:10:44  dkrajzew
// got rid of MSNet::Time
//
// Revision 1.2  2004/12/16 12:15:00  dkrajzew
// got rid of an unnecessary detector parameter/debugging
//
// Revision 1.1  2004/11/23 10:14:28  dkrajzew
// all detectors moved to microscim/output; new detectors usage applied
//
// Revision 1.8  2004/02/05 16:34:25  dkrajzew
// made the usage of the detector output end more usable
//
// Revision 1.7  2004/01/26 07:31:22  dkrajzew
// differnt detector usage types added
//
// Revision 1.6  2004/01/12 15:04:16  dkrajzew
// more wise definition of lane predeccessors implemented
//
// Revision 1.5  2004/01/12 14:35:10  dkrajzew
// documentation added; allowed the writing to files
// $Log$
// Revision 1.4  2005/05/04 08:11:51  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.3  2005/02/01 10:10:44  dkrajzew
// got rid of MSNet::Time
//
// Revision 1.2  2004/12/16 12:15:00  dkrajzew
// got rid of an unnecessary detector parameter/debugging
//
// Revision 1.1  2004/11/23 10:14:28  dkrajzew
// all detectors moved to microscim/output; new detectors usage applied
//
// Revision 1.8  2004/02/05 16:34:25  dkrajzew
// made the usage of the detector output end more usable
//
// Revision 1.7  2004/01/26 07:31:22  dkrajzew
// differnt detector usage types added
//
// Revision 1.6  2004/01/12 15:04:16  dkrajzew
// more wise definition of lane predeccessors implemented
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "MSE2Collector.h"
#include <utils/iodevices/XMLDevice.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class MS_E2_ZS_CollectorOverLanes
 * This class is somekind of a wrapper over several MSE2Collectors.
 * For some reasons it may be necessary to use MSE2Collectors that are
 *  longer than the lane they begin at. In this case, this class should be
 *  used. MSE2Collectors are laid on consecutive lanes backwards, building
 *  a virtual detector for each lane combination.
 * There are still some problems with it: we do not know how the different
 *  combinations shall be treated. Really verified is only the
 *  CURRENT_HALTING_DURATION_SUM_PER_VEHICLE-detector.
 */
class MS_E2_ZS_CollectorOverLanes :
    public MSDetectorFileOutput
{
public:
    /// Definition of a E2-collector storage
    typedef std::vector< MSE2Collector* > CollectorCont;

    /// !!!
    typedef std::map<std::string, std::vector<std::string> > LaneContinuations;

    /// Constructor
    MS_E2_ZS_CollectorOverLanes( std::string id,
        DetectorUsage usage, MSLane* lane, MSUnit::Meters startPos,
        MSUnit::Seconds haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        MSUnit::Meters jamDistThreshold,
        SUMOTime deleteDataAfterSeconds);

    /** @brief Builds the consecutive E2-detectors
        This is not done within the constructor to allow overriding of
        most functions but the building of detectors itself which in fact
        is depending on whether the normal or the gui-version is used */
    void init(MSLane *lane, MSUnit::Meters detLength,
        const LaneContinuations &laneContinuations);

    /// Destructor
    virtual ~MS_E2_ZS_CollectorOverLanes( void );

    /// Returns this detector's current value for the measure of the given type
    double getCurrent( E2::DetType type );

    /// Returns this detector's aggregated value for the given measure
    double getAggregate( E2::DetType type, MSUnit::Seconds lastNSeconds );

    /// Returns the information whether the given type is computed
    bool hasDetector( E2::DetType type );

    /// Adds the measure of the given type
    void addDetector( E2::DetType type, std::string detId = "" );

    /// Returns this detector's id
    const std::string &getId() const;

    /// Returns the id of the lane this detector starts at
    const std::string &getStartLaneID() const;

    /// ... have to override this method
    void resetQueueLengthAheadOfTrafficLights( void );


    /**
     * @name Inherited MSDetectorFileOutput methods.
     *
     */
    //@{
    /**
     * Returns a string indentifying an object of this class. Used for
     * distinct filenames.
     */
    std::string  getNamePrefix( void ) const
        {
            return std::string("MS_E2_ZS_CollectorOverLanes");
        }

    /**
     * Get a header for file output which shall contain some
     * explanation of the output generated by getXMLOutput.
     */
    void writeXMLHeader( XMLDevice &dev ) const
        {
            dev.writeString(xmlHeaderM);
        }

    /**
     * Get the XML-formatted output of the concrete detector.
     *
     * @param lastNTimesteps Generate data out of the interval
     * (now-lastNTimesteps, now].
     */
    void writeXMLOutput( XMLDevice &dev,
        SUMOTime startTime, SUMOTime stopTime );

    /**
     * Get an opening XML-element containing information about the detector.
     */
    void writeXMLDetectorInfoStart( XMLDevice &dev ) const;

    void writeXMLDetectorInfoEnd( XMLDevice &dev ) const
    {
        dev.writeString(infoEndM);
    }

    /**
     * Get the data-clean up interval in timesteps.
     */
    SUMOTime getDataCleanUpSteps( void ) const
        {
            return deleteDataAfterSecondsM; // !!! Konvertierung
        }
    //@}

    /// Returns this detector's length
    MSUnit::Meters getLength() const {
        return myLength;
    }

protected:
    /** @brief This method extends the current length up to the given
        This method is called consecutively until all paths have the
        desired length */
    void extendTo(double length,
        const LaneContinuations &laneContinuations);

    /// Builds an id for one of the E2-collectors this detector uses
    std::string  makeID( const std::string &baseID,
        size_t c, size_t r ) const;

    /// Builds a single E2-collector
    virtual MSE2Collector *buildCollector(size_t c, size_t r,
        MSLane *l, double start, double end);


    std::vector<MSLane*> getLanePredeccessorLanes(MSLane *l,
        const LaneContinuations &laneContinuations);

protected:
	/// The position the collector starts at
    MSUnit::Meters startPosM;

	/// The length of the collector
    MSUnit::Meters myLength;

	/// The information for how many seconds data shall be saved
    SUMOTime deleteDataAfterSecondsM;

	/// Describes how long a vehicle shall stay before being assigned to a jam
    MSUnit::Steps haltingTimeThresholdM;

    /// Describes how slow a vehicle must be before being assigned to a jam
    MSUnit::CellsPerStep haltingSpeedThresholdM;

    /// Describes how long a jam must be before being recognized
    MSUnit::Cells jamDistThresholdM;

    /// Definition of a lane storage
    typedef std::vector<MSLane*> LaneVector;

    /// Definition of a storage for lane vectors
    typedef std::vector<LaneVector> LaneVectorVector;

    /// Definition of a detector storage
    typedef std::vector<MSE2Collector*> DetectorVector;

    /// Definition of astorage for detector vectors
    typedef std::vector<DetectorVector> DetectorVectorVector;

    /// Definition of a double storage
    typedef std::vector<double> DoubleVector;

    /// Definition of a storage for double vectors
    typedef DoubleVector LengthVector;

    /** @brief Storage for lane combinations
        Each lane combination is a vector of consecutive lanes (backwards) */
    LaneVectorVector myLaneCombinations;

    /** @brief Storage for detector combinations
        Each detector combination is a vector of consecutive lanes (backwards) */
    DetectorVectorVector myDetectorCombinations;
    /** @brief Storage for length combinations
        Each length combination is a vector of consecutive lanes (backwards) */
    LengthVector myLengths;

    /// The string that is printed in front of the output file
    static std::string xmlHeaderM;

    /// The id of this detector
    std::string myID;

    /// The id of the lane this detector starts at
    std::string myStartLaneID;

    /// Definition of a map from a lane to the detector lying on it
    typedef std::map<MSLane*, MSE2Collector*> LaneDetMap;

    /// Storage for detectors which already have been build for a single lane
    LaneDetMap myAlreadyBuild;

    DetectorUsage myUsage;

private:
    static std::string infoEndM;

};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

