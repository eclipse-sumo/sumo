/****************************************************************************/
/// @file    GUIDetectorBuilder.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Tue, 22 Jul 2003
/// @version $Id$
///
// Builds detectors for guisim
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
#include <netload/NLDetectorBuilder.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSNet;


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
    GUIDetectorBuilder(MSNet& net);


    /// @brief Destructor
    ~GUIDetectorBuilder();


    /// @name Detector creating methods
    ///
    /// Override NLDetectorBuilder methods.
    /// @{

    /** @brief Creates an instance of an e1 detector using the given values
     *
     * Simply calls the GUIInductLoop constructor
     *
     * @param[in] id The id the detector shall have
     * @param[in] lane The lane the detector is placed at
     * @param[in] pos The position on the lane the detector is placed at
     * @param[in] vTypes which vehicle types are considered
     * @param[in] show Whether to show the detector in the gui if available
     */
    virtual MSDetectorFileOutput* createInductLoop(const std::string& id,
            MSLane* lane, double pos, const std::string& vTypes, bool show = true);


    /** @brief Creates an instance of an e1 detector using the given values
     *
     * Simply calls the MSInductLoop constructor
     *
     * @param[in] id The id the detector shall have
     * @param[in] lane The lane the detector is placed at
     * @param[in] pos The position on the lane the detector is placed at
     * @param[in] od The output device the loop shall use
     */
    virtual MSDetectorFileOutput* createInstantInductLoop(const std::string& id,
            MSLane* lane, double pos, const std::string& od, const std::string& vTypes);

    /** @brief Creates a GUIE2Collector instance, overrides MSE2Collector::createE2Detector()
     *
     * Simply calls the GUIE2Collector constructor
     *
     *  @see  GUIE2Collector Constructor documentation
     */
    virtual MSE2Collector* createE2Detector(const std::string& id,
                                            DetectorUsage usage, MSLane* lane, double pos, double endPos, double length,
                                            SUMOTime haltingTimeThreshold, double haltingSpeedThreshold, double jamDistThreshold,
                                            const std::string& vTypes, bool showDetector);

    virtual MSE2Collector* createE2Detector(const std::string& id,
                                            DetectorUsage usage, std::vector<MSLane*> lanes, double pos, double endPos,
                                            SUMOTime haltingTimeThreshold, double haltingSpeedThreshold, double jamDistThreshold,
                                            const std::string& vTypes, bool showDetector);


    /** @brief Creates an instance of an e3 detector using the given values
     *
     * Simply calls the GUIE3Collector constructor.
     *
     * @param[in] id The id the detector shall have
     * @param[in] entries The list of this detector's entries
     * @param[in] exits The list of this detector's exits
     * @param[in] haltingSpeedThreshold Detector parameter: the speed a vehicle's speed must be below to be assigned as jammed
     * @param[in] haltingTimeThreshold Detector parameter: the time a vehicle's speed must be below haltingSpeedThreshold to be assigned as jammed
     */
    virtual MSDetectorFileOutput* createE3Detector(const std::string& id,
            const CrossSectionVector& entries,
            const CrossSectionVector& exits,
            double haltingSpeedThreshold,
            SUMOTime haltingTimeThreshold, const std::string& vTypes);
    /// @}


};


#endif

/****************************************************************************/

