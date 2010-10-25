/****************************************************************************/
/// @file    MS_E2_ZS_CollectorOverLanes.h
/// @author  Daniel Krajzewicz
/// @date    Oct 2003
/// @version $Id$
///
// A detector which joins E2Collectors over consecutive lanes (backward)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MS_E2_ZS_CollectorOverLanes_h
#define MS_E2_ZS_CollectorOverLanes_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <microsim/output/MSDetectorFileOutput.h>
#include <utils/common/UtilExceptions.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSLane;
class SUMOVehicle;
class OutputDevice;
class MSE2Collector;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MS_E2_ZS_CollectorOverLanes
 * @brief A detector which joins E2Collectors over consecutive lanes (backward)
 *
 * This class is somekind of a wrapper over several MSE2Collectors.
 *
 * For some reasons it may be necessary to use MSE2Collectors that are
 *  longer than the lane they begin at. In this case, this class should be
 *  used. MSE2Collectors are laid on consecutive lanes backwards, building
 *  a virtual detector for each lane combination.
 *
 * There are still some problems with it: we do not know how the different
 *  combinations shall be treated.
 */
class MS_E2_ZS_CollectorOverLanes :
            public MSDetectorFileOutput {
public:
    /// Definition of a E2-collector storage
    typedef std::vector< MSE2Collector* > CollectorCont;

    /** @brief Constructor
     *
     * @param[in] id The id of the detector
     * @param[in] usage Information how the detector is used
     * @param[in] lane The lane the detector starts at
     * @param[in] startPos Begin position of the detector
     * @param[in] detLength Length of the detector
     * @param[in] haltingTimeThreshold The time a vehicle's speed must be below haltingSpeedThreshold to be assigned as jammed
     * @param[in] haltingSpeedThreshold The speed a vehicle's speed must be below to be assigned as jammed
     * @param[in] jamDistThreshold The distance between two vehicles in order to not count them to one jam
     * @todo The lane should not be given as a pointer
     */
    MS_E2_ZS_CollectorOverLanes(const std::string &id,
                                DetectorUsage usage, MSLane* lane, SUMOReal startPos,
                                SUMOTime haltingTimeThreshold, SUMOReal haltingSpeedThreshold,
                                SUMOReal jamDistThreshold) throw();


    /** @brief Builds the consecutive E2-detectors
     *
     * This is not done within the constructor to allow overriding of
     *  most functions but the building of detectors itself which in fact
     *  is depending on whether the normal or the gui-version is used
     * @param[in] lane The lane the detector starts at
     * @param[in] startPos Begin position of the detector
     */
    void init(MSLane *lane, SUMOReal detLength) throw();


    /// @brief Destructor
    virtual ~MS_E2_ZS_CollectorOverLanes() throw();


    /** @brief Returns this detector's id
     *
     * @return The id of this detector
     */
    const std::string &getID() const throw();


    /** @brief Returns the id of the lane this detector starts at
     *
     * @return The id of the lane this detector starts at
     * @todo Check whether this is needed
     */
    const std::string &getStartLaneID() const throw();



    /// @name Methods inherited from MSDetectorFileOutput.
    /// @{

    /** @brief Writes collected values into the given stream
     *
     * @param[in] dev The output device to write the data into
     * @param[in] startTime First time step the data were gathered
     * @param[in] stopTime Last time step the data were gathered
     * @see MSDetectorFileOutput::writeXMLOutput
     * @exception IOError If an error on writing occurs (!!! not yet implemented)
     */
    void writeXMLOutput(OutputDevice &dev,
                        SUMOTime startTime, SUMOTime stopTime) throw(IOError);


    /** @brief Opens the XML-output using "detector" as root element
     *
     * @param[in] dev The output device to write the root into
     * @see MSDetectorFileOutput::writeXMLDetectorProlog
     * @todo What happens with the additional information if several detectors use the same output?
     * @exception IOError If an error on writing occurs (!!! not yet implemented)
     */
    void writeXMLDetectorProlog(OutputDevice &dev) const throw(IOError);
    /// @}



    /** @brief Returns this detector's length [m]
     *
     * @return This detector's length in meters
     */
    SUMOReal getLength() const  throw() {
        return myLength;
    }

protected:
    /** @brief This method extends the current length up to the given
     *
     * This method is called consecutively until all paths have the desired length
     *
     * @param[in] length !!!
     * @todo Describe length's usage
     */
    void extendTo(SUMOReal length) throw();


    /** @brief Builds an id for one of the E2-collectors this detector uses
     *
     * @param[in] baseID The id of the parent detector
     * @param[in] c !!!
     * @param[in] r !!!
     * @todo Describe!
     */
    std::string  makeID(const std::string &baseID,
                        size_t c, size_t r) const throw();


    /** @brief Builds a single collector
     *
     * @param[in] c !!!
     * @param[in] r !!!
     * @param[in] start !!!
     * @param[in] end !!!
     * @todo Describe!
     */
    virtual MSE2Collector *buildCollector(size_t c, size_t r,
                                          MSLane *l, SUMOReal start, SUMOReal end) throw();


    /** @brief Returns the list of lanes predecessing the given one
     *
     * @param[in] l The lane to return predecessors of
     * @return List of lanes predecessing the given one
     */
    std::vector<MSLane*> getLanePredeccessorLanes(MSLane *l) throw();

protected:
    /// @brief The position the collector starts at
    SUMOReal startPosM;

    /// @brief The length of the collector
    SUMOReal myLength;

    /// @brief Describes how long a vehicle shall stay before being assigned to a jam
    SUMOTime haltingTimeThresholdM;

    /// @brief Describes how slow a vehicle must be before being assigned to a jam
    SUMOReal haltingSpeedThresholdM;

    /// @brief Describes how long a jam must be before being recognized
    SUMOReal jamDistThresholdM;

    /// @brief Definition of a lane storage
    typedef std::vector<MSLane*> LaneVector;

    /// @brief Definition of a storage for lane vectors
    typedef std::vector<LaneVector> LaneVectorVector;

    /// @brief Definition of a detector storage
    typedef std::vector<MSE2Collector*> DetectorVector;

    /// @brief Definition of a storage for detector vectors
    typedef std::vector<DetectorVector> DetectorVectorVector;

    /// @brief Definition of a SUMOReal storage
    typedef std::vector<SUMOReal> DoubleVector;

    /// @brief Definition of a storage for SUMOReal vectors
    typedef DoubleVector LengthVector;


    /** @brief Storage for lane combinations
     *
     * Each lane combination is a vector of consecutive lanes (backwards) */
    LaneVectorVector myLaneCombinations;


    /** @brief Storage for detector combinations
     *
     * Each detector combination is a vector of consecutive lanes (backwards) */
    DetectorVectorVector myDetectorCombinations;


    /** @brief Storage for length combinations
     *
     * Each length combination is a vector of consecutive lanes (backwards) */
    LengthVector myLengths;


    /// @brief The id of this detector
    std::string myID;

    /// @brief The id of the lane this detector starts at
    std::string myStartLaneID;

    /// @brief Definition of a map from a lane to the detector lying on it
    typedef std::map<MSLane*, MSE2Collector*> LaneDetMap;

    /// @brief Storage for detectors which already have been build for a single lane
    LaneDetMap myAlreadyBuild;

    /// @brief Information about how this detector is used
    DetectorUsage myUsage;


private:
    /// @brief Invalidated copy constructor.
    MS_E2_ZS_CollectorOverLanes(const MS_E2_ZS_CollectorOverLanes&);

    /// @brief Invalidated assignment operator.
    MS_E2_ZS_CollectorOverLanes& operator=(const MS_E2_ZS_CollectorOverLanes&);


};


#endif

/****************************************************************************/

