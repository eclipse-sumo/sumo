/****************************************************************************/
/// @file    NLDetectorBuilder.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 15 Apr 2002
/// @version $Id$
///
// Builds detectors for microsim
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

#ifdef _MESSAGES
#include <microsim/output/MSMsgInductLoop.h>
#endif


// ===========================================================================
// class declarations
// ===========================================================================
class MSInductLoop;
class MS_E2_ZS_CollectorOverLanes;
class MSTrafficLightLogic;
class MSDetectorControl;
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
 * @brief Builds detectors for microsim
 *
 * The building methods may be overridden, to build guisim-instances of the triggers,
 *  for example.
 */
class NLDetectorBuilder
{
public:
    /** @brief Constructor
     *
     * @param[in] net The network to which's detector control built detector shall be added
     */
    NLDetectorBuilder(MSNet &net) throw();


    /// @brief Destructor
    virtual ~NLDetectorBuilder() throw();


    /// @name Value parsing and detector building methods
    /// @{

#ifdef _MESSAGES
	/** @brief Builds an e4-detector and adds it to the net
	 *
	 *	@param[in] id The id the detector shall have
	 *	@param[in] lane The name of the lane the detector is placed at
	 */
	void
	buildMsgDetector(const std::string &id,
					 const std::string &lane, SUMOReal pos, int splInterval,
					 const std::string &msg,
					 OutputDevice& device, bool friendlyPos) throw(InvalidArgument);
#endif
    
	/** @brief Builds an e1-detector and adds it to the net
     *
     * Checks the given values, first. If one of the values is invalid
     *  (lane is not known, sampling frequency<=0, position is larger
     *  than lane's length, the id is already in use), an InvalidArgument is thrown.
     *
     * Otherwise the e1-detector is built by calling "createInductLoop".
     *
     * Internally, there is also a distinction whether a mesosim e1-detector
     *  shall be built.
     *
     * @param[in] id The id the detector shall have
     * @param[in] lane The name of the lane the detector is placed at
     * @param[in] pos The definition of the position on the lane the detector shall be placed at
     * @param[in] splInterval The aggregation time span the detector shall use
     * @param[in] device The output device the detector shall write into
     * @param[in] friendlyPos Whether the position information shall be used "friendly" (see user docs)
     * @exception InvalidArgument If one of the values is invalid
     */
    void buildInductLoop(const std::string &id,
                         const std::string &lane, SUMOReal pos, int splInterval,
                         OutputDevice& device, bool friendlyPos) throw(InvalidArgument);


    /** @brief Builds an e2-detector with a fixed interval and adds it to the net
     *
     * Checks the given values, first. If one of the values is invalid
     *  (lane is not known, sampling frequency<=0, position is larger
     *  than lane's length, length is too large, the id is already in use),
     *  an InvalidArgument is thrown.
     *
     * Otherwise the e2-detector is built, either by calling "buildMultiLaneE2Det"
     *  if the detector shall continue on consecutive lanes, or by calling
     *  "buildSingleLaneE2Det" if it is a one-lane detector.
     *
     * @param[in] edgeContinuations The edge continuations
     * @param[in] id The id the detector shall have
     * @param[in] lane The name of the lane the detector is placed at
     * @param[in] pos The definition of the position on the lane the detector shall be placed at
     * @param[in] length The definition of the length the detector shall have
     * @param[in] cont Whether the detector shall continue on predeceeding lanes
     * @param[in] splInterval The aggregation time span the detector shall use
     * @param[in] device The output device the detector shall write into
     * @param[in] haltingTimeThreshold Detector parameter: the time a vehicle's speed must be below haltingSpeedThreshold to be assigned as jammed
     * @param[in] haltingSpeedThreshold Detector parameter: the speed a vehicle's speed must be below to be assigned as jammed
     * @param[in] jamDistThreshold Detector parameter: the distance between two vehicles in order to not count them to one jam
     * @param[in] friendlyPos Whether the position information shall be used "friendly" (see user docs)
     * @exception InvalidArgument If one of the values is invalid
     */
    void buildE2Detector(const MSEdgeContinuations &edgeContinuations,
                         const std::string &id, const std::string &lane, SUMOReal pos, SUMOReal length,
                         bool cont, int splInterval, OutputDevice& device, SUMOTime haltingTimeThreshold,
                         MetersPerSecond haltingSpeedThreshold, SUMOReal jamDistThreshold,
                         bool friendlyPos) throw(InvalidArgument);


    /** @brief Builds an e2-detector connected to a lsa
     *
     * Checks the given values, first. If one of the values is invalid
     *  (lane is not known, position is larger than lane's length, length is too large,
     *  the tls is not known, the id is already in use),
     *  an InvalidArgument is thrown.
     *
     * Otherwise the e2-detector is built, either by calling "buildMultiLaneE2Det"
     *  if the detector shall continue on consecutive lanes, or by calling
     *  "buildSingleLaneE2Det" if it is a one-lane detector.
     *
     * @param[in] edgeContinuations The edge continuations
     * @param[in] id The id the detector shall have
     * @param[in] lane The name of the lane the detector is placed at
     * @param[in] pos The definition of the position on the lane the detector shall be placed at
     * @param[in] length The definition of the length the detector shall have
     * @param[in] cont Whether the detector shall continue on predeceeding lanes
     * @param[in] tlls The tls the detector is assigned to
     * @param[in] device The output device the detector shall write into
     * @param[in] haltingTimeThreshold Detector parameter: the time a vehicle's speed must be below haltingSpeedThreshold to be assigned as jammed
     * @param[in] haltingSpeedThreshold Detector parameter: the speed a vehicle's speed must be below to be assigned as jammed
     * @param[in] jamDistThreshold Detector parameter: the distance between two vehicles in order to not count them to one jam
     * @param[in] friendlyPos Whether the position information shall be used "friendly" (see user docs)
     * @exception InvalidArgument If one of the values is invalid
     */
    void buildE2Detector(const MSEdgeContinuations &edgeContinuations,
                         const std::string &id, const std::string &lane, SUMOReal pos, SUMOReal length,
                         bool cont, MSTLLogicControl::TLSLogicVariants &tlls,
                         OutputDevice& device, SUMOTime haltingTimeThreshold,
                         MetersPerSecond haltingSpeedThreshold, SUMOReal jamDistThreshold,
                         bool friendlyPos) throw(InvalidArgument);


    /** @brief Builds an e2-detector connected to a link's state
     *
     * Checks the given values, first. If one of the values is invalid
     *  (lane is not known, position is larger than lane's length, length is too large,
     *  the tls or the destination lane is not known, the id is already in use),
     *  an InvalidArgument is thrown.
     *
     * Otherwise the e2-detector is built, either by calling "buildMultiLaneE2Det"
     *  if the detector shall continue on consecutive lanes, or by calling
     *  "buildSingleLaneE2Det" if it is a one-lane detector.
     *
     * @param[in] edgeContinuations The edge continuations
     * @param[in] id The id the detector shall have
     * @param[in] lane The name of the lane the detector is placed at
     * @param[in] pos The definition of the position on the lane the detector shall be placed at
     * @param[in] length The definition of the length the detector shall have
     * @param[in] cont Whether the detector shall continue on predeceeding lanes
     * @param[in] tlls The tls the detector is assigned to
     * @param[in] tolane The name of the lane to which the link to which the detector to build shall be assigned to points
     * @param[in] device The output device the detector shall write into
     * @param[in] haltingTimeThreshold Detector parameter: the time a vehicle's speed must be below haltingSpeedThreshold to be assigned as jammed
     * @param[in] haltingSpeedThreshold Detector parameter: the speed a vehicle's speed must be below to be assigned as jammed
     * @param[in] jamDistThreshold Detector parameter: the distance between two vehicles in order to not count them to one jam
     * @param[in] friendlyPos Whether the position information shall be used "friendly" (see user docs)
     * @exception InvalidArgument If one of the values is invalid
     */
    void buildE2Detector(const MSEdgeContinuations &edgeContinuations,
                         const std::string &id, const std::string &lane, SUMOReal pos, SUMOReal length,
                         bool cont, MSTLLogicControl::TLSLogicVariants &tlls, const std::string &tolane,
                         OutputDevice& device, SUMOTime haltingTimeThreshold,
                         MetersPerSecond haltingSpeedThreshold, SUMOReal jamDistThreshold,
                         bool friendlyPos) throw(InvalidArgument);


    /** @brief Stores temporary the initial information about an e3-detector to build
     *
     * If the given sample interval is < 0, an InvalidArgument is thrown. Otherwise,
     *  the values are stored in a new instance of E3DetectorDefinition within
     *  "myE3Definition".
     *
     * @param[in] id The id the detector shall have
     * @param[in] device The output device the detector shall write into
     * @param[in] splInterval The aggregation time span the detector shall use
     * @param[in] haltingTimeThreshold Detector parameter: the time a vehicle's speed must be below haltingSpeedThreshold to be assigned as jammed
     * @param[in] haltingSpeedThreshold Detector parameter: the speed a vehicle's speed must be below to be assigned as jammed
     * @exception InvalidArgument If one of the values is invalid
     */
    void beginE3Detector(const std::string &id, OutputDevice& device, int splInterval,
                         MetersPerSecond haltingSpeedThreshold, SUMOTime haltingTimeThreshold) throw(InvalidArgument);


    /** @brief Builds an entry point of an e3 detector
     *
     * If the lane is not known or the position information is not within the lane,
     *  an InvalidArgument is thrown. Otherwise a MSCrossSection is built
     *  using the obtained values and added to the list of entries of the e3-definition
     *  stored in "myE3Definition".
     *
     * @param[in] lane The id of the lane the entry shall be placed at
     * @param[in] pos The position on the lane the entry shall be placed at
     * @param[in] friendlyPos Whether the position information shall be used "friendly" (see user docs)
     * @exception InvalidArgument If one of the values is invalid
     */
    void addE3Entry(const std::string &lane, SUMOReal pos, bool friendlyPos) throw(InvalidArgument);


    /** @brief Builds an exit point of an e3 detector
     *
     * If the lane is not known or the position information is not within the lane,
     *  an InvalidArgument is thrown. Otherwise a MSCrossSection is built
     *  using the obtained values and added to the list of exits of the e3-definition
     *  stored in "myE3Definition".
     *
     * @param[in] lane The id of the lane the exit shall be placed at
     * @param[in] pos The position on the lane the exit shall be placed at
     * @param[in] friendlyPos Whether the position information shall be used "friendly" (see user docs)
     * @exception InvalidArgument If one of the values is invalid
     */
    void addE3Exit(const std::string &lane, SUMOReal pos, bool friendlyPos) throw(InvalidArgument);


    /** @brief Builds of an e3-detector using collected values
     *
     * The parameter collected are used to build an e3-detector using
     *  "createE3Detector". The resulting detector is added to the net.
     *
     * @param[in] lane The id of the lane the exit shall be placed at
     * @param[in] pos The position on the lane the exit shall be placed at
     * @exception InvalidArgument If one of the values is invalid
     */
    void endE3Detector() throw(InvalidArgument);


    /** @brief Returns the id of the currently built e3-detector
     *
     * This is used for error-message generation only. If no id is known,
     *  "<unknown>" is returned.
     *
     * @return The id of the currently processed e3-detector
     */
    std::string getCurrentE3ID() const throw();


    /** @brief Builds a vtypeprobe and adds it to the net
     *
     * Checks the given values, first. If one of the values is invalid
     *  (sampling frequency<=0), an InvalidArgument is thrown.
     *
     * Otherwise the vtypeprobe is built (directly).
     *
     * @param[in] id The id the detector shall have
     * @param[in] vtype The name of the vehicle type the detector shall oberve
     * @param[in] frequency The reporting frequency
     * @param[in] device The output device the detector shall write into
     * @exception InvalidArgument If one of the values is invalid
     */
    void buildVTypeProbe(const std::string &id,
                         const std::string &vtype, SUMOTime frequency,
                         OutputDevice& device) throw(InvalidArgument);
    /// @}



    /// @name Detector creating methods
    ///
    /// Virtual, so they may be overwritten, for generating gui-versions of the detectors, for example.
    /// @{

#ifdef _MESSAGES
	/** @brief Creates an instance of an e4-detector using the given values
	 *
	 * Simply calls the MSMsgInductLoop constructor
	 */
	virtual MSMsgInductLoop *createMsgInductLoop(const std::string &id, const std::string &msg,
												 MSLane *lane, SUMOReal pos) throw();
#endif
    
	/** @brief Creates an instance of an e1-detector using the given values
     *
     * Simply calls the MSInductLoop constructor
     *
     * @param[in] id The id the detector shall have
     * @param[in] lane The lane the detector is placed at
     * @param[in] pos The position on the lane the detector is placed at
     */
    virtual MSInductLoop *createInductLoop(const std::string &id,
                                           MSLane *lane, SUMOReal pos) throw();


#ifdef HAVE_MESOSIM
    /** @brief Creates an instance of a mesoscopic e1-detector using the given values
     *
     * Simply calls the MEInductLoop constructor
     *
     * @param[in] id The id the detector shall have
     * @param[in] s The segment the detector is placed at
     * @param[in] pos ?
     * @todo Position is not used, herein!?
     */
    virtual MEInductLoop *createMEInductLoop(const std::string &id,
            MESegment *s, SUMOReal pos) throw();
#endif


    /** @brief Creates an instance of an e2-detector using the given values
     *
     * Simply calls the MSE2Collector constructor
     *
     * @param[in] id The id the detector shall have
     * @param[in] lane The lane the detector is placed at
     * @param[in] pos The position on the lane the detector is placed at
     * @param[in] length The length the detector has
     * @param[in] haltingTimeThreshold Detector parameter: the time a vehicle's speed must be below haltingSpeedThreshold to be assigned as jammed
     * @param[in] haltingSpeedThreshold Detector parameter: the speed a vehicle's speed must be below to be assigned as jammed
     * @param[in] jamDistThreshold Detector parameter: the distance between two vehicles in order to not count them to one jam
     */
    virtual MSE2Collector *createSingleLaneE2Detector(const std::string &id,
            DetectorUsage usage, MSLane *lane, SUMOReal pos, SUMOReal length,
            SUMOTime haltingTimeThreshold,
            MetersPerSecond haltingSpeedThreshold,
            SUMOReal jamDistThreshold) throw();


    /** @brief Creates an instance of an e2ol-detector using the given values
     *
     * Simply calls the MS_E2_ZS_CollectorOverLanes constructor. After this call,
     *  the detector must be initialised.
     *
     * @param[in] id The id the detector shall have
     * @param[in] lane The lane the detector is placed at
     * @param[in] pos The position on the lane the detector is placed at
     * @param[in] length The length the detector has
     * @param[in] haltingTimeThreshold Detector parameter: the time a vehicle's speed must be below haltingSpeedThreshold to be assigned as jammed
     * @param[in] haltingSpeedThreshold Detector parameter: the speed a vehicle's speed must be below to be assigned as jammed
     * @param[in] jamDistThreshold Detector parameter: the distance between two vehicles in order to not count them to one jam
     */
    virtual MS_E2_ZS_CollectorOverLanes *createMultiLaneE2Detector(
        const std::string &id, DetectorUsage usage, MSLane *lane, SUMOReal pos,
        SUMOTime haltingTimeThreshold, MetersPerSecond haltingSpeedThreshold,
        SUMOReal jamDistThreshold) throw();


    /** @brief Creates an instance of an e3-detector using the given values
     *
     * Simply calls the MSE3Collector constructor.
     *
     * @param[in] id The id the detector shall have
     * @param[in] entries The list of this detector's entries
     * @param[in] exits The list of this detector's exits
     * @param[in] haltingSpeedThreshold Detector parameter: the speed a vehicle's speed must be below to be assigned as jammed
     * @param[in] haltingTimeThreshold Detector parameter: the time a vehicle's speed must be below haltingSpeedThreshold to be assigned as jammed
     */
    virtual MSE3Collector *createE3Detector(const std::string &id,
                                            const CrossSectionVector &entries, const CrossSectionVector &exits,
                                            MetersPerSecond haltingSpeedThreshold, SUMOTime haltingTimeThreshold) throw();
    /// @}



    /** @brief Builds an e2-detector that lies on only one lane
     *
     * @param[in] id The id the detector shall have
     * @param[in] usage Information how the detector is used within the simulation
     * @param[in] lane The lane the detector is placed at
     * @param[in] pos The position on the lane the detector is placed at
     * @param[in] length The length the detector has
     * @param[in] haltingTimeThreshold Detector parameter: the time a vehicle's speed must be below haltingSpeedThreshold to be assigned as jammed
     * @param[in] haltingSpeedThreshold Detector parameter: the speed a vehicle's speed must be below to be assigned as jammed
     * @param[in] jamDistThreshold Detector parameter: the distance between two vehicles in order to not count them to one jam
     * @todo Check whether this method is really needful
     */
    MSE2Collector *buildSingleLaneE2Det(const std::string &id,
                                        DetectorUsage usage, MSLane *lane, SUMOReal pos, SUMOReal length,
                                        SUMOTime haltingTimeThreshold, MetersPerSecond haltingSpeedThreshold,
                                        SUMOReal jamDistThreshold) throw();


    /** @brief Builds an e2-detector that continues on preceeding lanes
     *
     * @param[in] id The id the detector shall have
     * @param[in] usage Information how the detector is used within the simulation
     * @param[in] lane The lane the detector is placed at
     * @param[in] pos The position on the lane the detector is placed at
     * @param[in] length The length the detector has
     * @param[in] haltingTimeThreshold Detector parameter: the time a vehicle's speed must be below haltingSpeedThreshold to be assigned as jammed
     * @param[in] haltingSpeedThreshold Detector parameter: the speed a vehicle's speed must be below to be assigned as jammed
     * @param[in] jamDistThreshold Detector parameter: the distance between two vehicles in order to not count them to one jam
     * @todo Check whether this method is really needful
     */
    MS_E2_ZS_CollectorOverLanes *buildMultiLaneE2Det(const MSEdgeContinuations &edgeContinuations,
            const std::string &id, DetectorUsage usage, MSLane *lane, SUMOReal pos, SUMOReal length,
            SUMOTime haltingTimeThreshold, MetersPerSecond haltingSpeedThreshold,
            SUMOReal jamDistThreshold) throw();


protected:
    /**
     * @class E3DetectorDefinition
     * @brief Holds the incoming definitions of an e3 detector unless the detector is build.
     */
    class E3DetectorDefinition
    {
    public:
        /** @brief Constructor
         * @param[in] id The id the detector shall have
         * @param[in] device The output device the detector shall write into
         * @param[in] haltingSpeedThreshold Detector parameter: the speed a vehicle's speed must be below to be assigned as jammed
         * @param[in] haltingTimeThreshold Detector parameter: the time a vehicle's speed must be below haltingSpeedThreshold to be assigned as jammed
         * @param[in] splInterval The aggregation time span the detector shall use
         */
        E3DetectorDefinition(const std::string &id,
                             OutputDevice& device, MetersPerSecond haltingSpeedThreshold,
                             SUMOTime haltingTimeThreshold, int splInterval) throw();

        /// @brief Destructor
        ~E3DetectorDefinition() throw();

        /// @brief The id of the detector
        std::string myID;
        /// @brief The device the detector shall use
        OutputDevice& myDevice;
        /// @brief The speed a vehicle's speed must be below to be assigned as jammed
        MetersPerSecond myHaltingSpeedThreshold;
        /// @brief The time a vehicle's speed must be below haltingSpeedThreshold to be assigned as jammed
        SUMOTime myHaltingTimeThreshold;
        /// @brief List of detector's entries
        CrossSectionVector myEntries;
        /// @brief List of detector's exits
        CrossSectionVector myExits;
        /// @brief The aggregation interval
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
     *
     * Throws an InvalidArgument if the lane does not exist
     *
     * @param[in] id The id of the lane
     * @param[in] detid The id of the currently built detector (for error message generation)
     * @exception InvalidArgument If the named lane is not known
     */
    MSLane *getLaneChecking(const std::string &id,
                            const std::string &detid) throw(InvalidArgument);


    /** @brief Computes the position to use
     *
     * At first, it is checked whether the given position is negative. If so, the
     *  position is added to the lane's length to obtain the position counted
     *  backwards.
     *
     * If the resulting position is beyond or in front (<0) of the lane, it is either
     *  set to the according lane's boundary (.1 or length-.1) if friendlyPos
     *  is set, or, if friendly_pos is not set, an InvalidArgument is thrown.
     *
     * @param[in] pos Definition of the position on the lane
     * @param[in] lane The lane the position must be valid for
     * @param[in] friendlyPos Whether false positions shall be made acceptable
     * @param[in] detid The id of the currently built detector (for error message generation)
     * @exception InvalidArgument If the defined position is invalid
     */
    SUMOReal getPositionChecking(SUMOReal pos, MSLane *lane, bool friendlyPos,
                                 const std::string &detid) throw(InvalidArgument);


    /** @brief Converts the length and the position information for an e2-detector
     *
     * @param[in] id The id of the currently built detector (for error message generation)
     * @param[in] clane The lane the detector is placed at
     * @param[in, out] pos The position definition to convert
     * @param[in, out] length The length definition to convert
     * @exception InvalidArgument If the defined position or the defined length is invalid
     */
    void convUncontE2PosLength(const std::string &id, MSLane *clane,
                               SUMOReal &pos, SUMOReal &length, bool frinedly_pos) throw(InvalidArgument);


    /** @brief Converts the length and the position information for an e2ol-detector
     *
     * @param[in] id The id of the currently built detector (for error message generation)
     * @param[in] clane The lane the detector is placed at
     * @param[in, out] pos The position definition to convert
     * @param[in, out] length The length definition to convert
     * @exception InvalidArgument If the defined position or the defined length is invalid
     */
    void convContE2PosLength(const std::string &id, MSLane *clane,
                             SUMOReal &pos, SUMOReal &length, bool frinedly_pos) throw(InvalidArgument);


protected:
    /// @brief The net to fill
    MSNet &myNet;


private:
    /// @brief definition of the currently parsed e3-detector
    E3DetectorDefinition *myE3Definition;


private:
    /// @brief Invalidated copy constructor.
    NLDetectorBuilder(const NLDetectorBuilder&);

    /// @brief Invalidated assignment operator.
    NLDetectorBuilder& operator=(const NLDetectorBuilder&);

};


#endif

/****************************************************************************/

