/****************************************************************************/
/// @file    MSDetectorFileOutput.h
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    2004-11-23
/// @version $Id$
///
// Base of value-generating classes (detectors)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSDetectorFileOutput_h
#define MSDetectorFileOutput_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>

#include <utils/common/SUMOTime.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/common/Named.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUIDetectorWrapper;


// ===========================================================================
// class definitions
// ===========================================================================
enum DetectorUsage {
    DU_USER_DEFINED,
    DU_SUMO_INTERNAL,
    DU_TL_CONTROL
};

/**
 * @class MSDetectorFileOutput
 * @brief Base of value-generating classes (detectors)
 *
 * Pure virtual base class for classes (e.g. MSInductLoop) that should produce
 *  XML-output.
 */
class MSDetectorFileOutput : public Named {
public:
    /// @brief Constructor
    MSDetectorFileOutput(const std::string& id) : Named(id) { }


    /// @brief (virtual) destructor
    virtual ~MSDetectorFileOutput() { }


    /// @name Virtual methods to implement by derived classes
    /// @{

    /** @brief Write the generated output to the given device
     * @param[in] dev The output device to write the data into
     * @param[in] startTime First time step the data were gathered
     * @param[in] stopTime Last time step the data were gathered
     * @exception IOError If an error on writing occurs
     */
    virtual void writeXMLOutput(OutputDevice& dev,
                                SUMOTime startTime, SUMOTime stopTime) = 0;


    /** @brief Open the XML-output
     *
     * The implementing function should open an xml element using
     *  OutputDevice::writeXMLHeader.
     *
     * @param[in] dev The output device to write the root into
     * @exception IOError If an error on writing occurs
     */
    virtual void writeXMLDetectorProlog(OutputDevice& dev) const = 0;


    /** @brief Resets collected values
     *
     * Please note that this is only a "hack" for coupled-tls-outputs.
     *
     * @see Command_SaveTLCoupledLaneDet
     * @todo Reckeck/refactor
     */
    virtual void reset() { }


    /** @brief Updates the detector (computes values)
     *
     * @param[in] step The current time step
     */
    virtual void detectorUpdate(const SUMOTime step) {
        UNUSED_PARAMETER(step);
    }


    /** @brief Builds the graphical representation
     *
     * Meant to be overridden by graphical versions of the detectors
     * @return A wrapper for the detector which performs the user I/O within the GUI
     */
    virtual GUIDetectorWrapper* buildDetectorGUIRepresentation() {
        return 0;
    }


private:
    /// @brief Invalidated copy constructor.
    MSDetectorFileOutput(const MSDetectorFileOutput&);

    /// @brief Invalidated assignment operator.
    MSDetectorFileOutput& operator=(const MSDetectorFileOutput&);


};


#endif

/****************************************************************************/

