/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    MSDetectorFileOutput.h
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    2004-11-23
///
// Base of value-generating classes (detectors)
/****************************************************************************/
#pragma once
#include <config.h>

#include <string>
#include <set>

#include <utils/common/Named.h>
#include <utils/common/Parameterised.h>
#include <utils/common/SUMOTime.h>
#include <microsim/MSNet.h>


// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;
class GUIDetectorWrapper;
class SUMOTrafficObject;
class MSTransportable;
class MSEdge;


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
class MSDetectorFileOutput : public Named, public Parameterised {
public:
    /// @brief Constructor
    MSDetectorFileOutput(const std::string& id, const std::string& vTypes, const std::string& nextEdges = "", const int detectPersons = false);

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


    /** @brief Checks whether the detector measures vehicles of the given type.
    *
    * @param[in] veh the vehicle of which the type is checked.
    * @return whether it should be measured
    */
    bool vehicleApplies(const SUMOTrafficObject& veh) const;

    bool personApplies(const MSTransportable& p, int dir) const;


    /** @brief Checks whether the detector is type specific.
    *
    * @return whether vehicle types are considered
    */
    bool isTyped() const {
        return !myVehicleTypes.empty();
    }

    const std::set<std::string>& getVehicleTypes() const {
        return myVehicleTypes;
    }

    inline bool detectPersons() const {
        return myDetectPersons != 0;
    }

    /** @brief Remove all vehicles before quick-loading state */
    virtual void clearState(SUMOTime /*step*/) {};

protected:
    /// @brief The vehicle types to look for (empty means all)
    std::set<std::string> myVehicleTypes;

    /// @brief The upcoming edges to filter by (empty means no filtering)
    std::vector<const MSEdge*> myNextEdges;

    /// @brief Whether pedestrians shall be detected instead of vehicles
    const int myDetectPersons;

private:
    /// @brief Invalidated copy constructor.
    MSDetectorFileOutput(const MSDetectorFileOutput&);

    /// @brief Invalidated assignment operator.
    MSDetectorFileOutput& operator=(const MSDetectorFileOutput&);


};
