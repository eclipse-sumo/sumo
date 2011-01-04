/****************************************************************************/
/// @file    GUIDetectorBuilder.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 22 Jul 2003
/// @version $Id$
///
// Builds detectors for guisim
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIDetectorBuilder_h
#define GUIDetectorBuilder_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <microsim/MSNet.h>
#include <netload/NLDetectorBuilder.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSDetectorControl;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIDetectorBuilder
 * @brief Builds detectors for guisim
 *
 * This class overrides NLDetectorBuilder's detector creation methods in order
 *  to build guisim-classes instead of microsim-classes.
 *
 * @see NLDetectorBuilder
 */
class GUIDetectorBuilder : public NLDetectorBuilder {
public:
    /** @brief Constructor
     *
     * @param[in] net The network to which's detector control built detector shall be added
     */
    GUIDetectorBuilder(MSNet &net) throw();


    /// @brief Destructor
    ~GUIDetectorBuilder() throw();


    /// @name Detector creating methods
    ///
    /// Override NLDetectorBuilder methods.
    /// @{

    /** @brief Creates an instance of an e1-detector using the given values
     *
     * Simply calls the GUIInductLoop constructor
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
     * Simply calls the GUI_E2_ZS_Collector constructor
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
            SUMOReal haltingSpeedThreshold,
            SUMOReal jamDistThreshold) throw();


    /** @brief Creates an instance of an e2ol-detector using the given values
     *
     * Simply calls the GUI_E2_ZS_CollectorOverLanes constructor. After this call,
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
        SUMOTime haltingTimeThreshold,
        SUMOReal haltingSpeedThreshold,
        SUMOReal jamDistThreshold) throw();


    /** @brief Creates an instance of an e3-detector using the given values
     *
     * Simply calls the GUIE3Collector constructor.
     *
     * @param[in] id The id the detector shall have
     * @param[in] entries The list of this detector's entries
     * @param[in] exits The list of this detector's exits
     * @param[in] haltingSpeedThreshold Detector parameter: the speed a vehicle's speed must be below to be assigned as jammed
     * @param[in] haltingTimeThreshold Detector parameter: the time a vehicle's speed must be below haltingSpeedThreshold to be assigned as jammed
     */
    virtual MSE3Collector *createE3Detector(const std::string &id,
                                            const CrossSectionVector &entries,
                                            const CrossSectionVector &exits,
                                            SUMOReal haltingSpeedThreshold,
                                            SUMOTime haltingTimeThreshold) throw();
    /// @}


};


#endif

/****************************************************************************/

