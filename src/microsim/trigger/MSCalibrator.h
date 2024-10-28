/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2005-2024 German Aerospace Center (DLR) and others.
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
/// @file    MSCalibrator.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Tue, May 2005
///
// Calibrates the flow on an edge by removing an inserting vehicles
/****************************************************************************/
#pragma once
#include <config.h>

#include <string>
#include <vector>
#include <utils/common/Command.h>
#include <microsim/MSRouteHandler.h>
#include <microsim/output/MSMeanData_Net.h>
#include <microsim/output/MSDetectorFileOutput.h>


// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;
class MSRouteProbe;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSCalibrator
 * @brief Calibrates the flow on a segment to a specified one
 */
class MSCalibrator : public MSRouteHandler, public Command, public MSDetectorFileOutput {
public:
    /** constructor */
    MSCalibrator(const std::string& id,
                 MSEdge* const edge,
                 MSLane* const lane,
                 MSJunction* const node,
                 const double pos,
                 const std::string& aXMLFilename,
                 const std::string& outputFilename,
                 const SUMOTime freq, const double length,
                 const MSRouteProbe* probe,
                 const double invalidJamThreshold,
                 const std::string& vTypes,
                 const bool local,
                 const bool addLaneMeanData);

    /** destructor */
    virtual ~MSCalibrator();

    /** @brief Write the generated output to the given device
     * @param[in] dev The output device to write the data into
     * @param[in] startTime First time step the data were gathered
     * @param[in] stopTime Last time step the data were gathered
     * @exception IOError If an error on writing occurs
     */
    void writeXMLOutput(OutputDevice& dev, SUMOTime startTime, SUMOTime stopTime);


    /** @brief Open the XML-output
     *
     * The implementing function should open an xml element using
     *  OutputDevice::writeXMLHeader.
     *
     * @param[in] dev The output device to write the root into
     * @exception IOError If an error on writing occurs
     */
    void writeXMLDetectorProlog(OutputDevice& dev) const;

    /** the implementation of the MSTrigger / Command interface.
        Calibrating takes place here. */
    virtual SUMOTime execute(SUMOTime currentTime);

    /// @brief cleanup remaining data structures
    static void cleanup();

    /// @brief return all calibrator instances
    static const std::map<std::string, MSCalibrator*>& getInstances() {
        return myInstances;
    }

    struct AspiredState {
        AspiredState() : begin(-1), end(-1), q(-1.), v(-1.), vehicleParameter(0) {}
        SUMOTime begin;
        SUMOTime end;
        double q;
        double v;
        SUMOVehicleParameter* vehicleParameter;
    };

    AspiredState getCurrentStateInterval() const;

    const MSEdge* getEdge() const {
        return myEdge;
    }

    const MSLane* getLane() const {
        return myLane;
    }

    const MSRouteProbe* getRouteProbe() const {
        return myProbe;
    }

    inline virtual int passed() const {
        // calibrator measures at start of segment
        // vehicles drive to the end of an edge by default so they count as passed
        // but vaporized vehicles do not count
        // if the calibrator is located on a short edge, the vehicles are
        // vaporized on the next edge so we cannot rely on myEdgeMeanData.nVehVaporized
        return myEdgeMeanData.nVehEntered + myEdgeMeanData.nVehDeparted - myClearedInJam - myRemoved;
    }

    int getInserted() const {
        return myEdgeMeanData.nVehDeparted;
    }

    int getRemoved() const {
        return myClearedInJam + myRemoved;
    }

    void setFlow(SUMOTime begin, SUMOTime end, double vehsPerHour, double speed, SUMOVehicleParameter vehicleParameter);

protected:
    class CalibratorCommand : public Command {
    public:
        CalibratorCommand(MSCalibrator* cali) :
            myCalibrator(cali) {}

        SUMOTime execute(SUMOTime currentTime) {
            return myCalibrator->execute(currentTime);
        }

        /** @brief Reschedule or deschedule the command when quick-loading state
         *
         * The implementations should return -1 if the command shall not be re-scheduled,
         *  or a value >= 0 that describe the new time at which the command
         *  shall be executed again.
         *
         * @param[in] currentTime The current simulation time
         * @param[in] execTime The time at which the command would have been executed
         * @param[in] newTime The simulation time at which the simulation is restarted
         * @return The time at which the command shall be executed again
         */
        SUMOTime shiftTime(SUMOTime currentTime, SUMOTime execTime, SUMOTime newTime);

    private:
        MSCalibrator* myCalibrator;
    };


    /// @name inherited from GenericSAXHandler
    //@{

    /** @brief Called on the opening of a tag;
     *
     * @param[in] element ID of the currently opened element
     * @param[in] attrs Attributes within the currently opened element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myStartElement
     */
    virtual void myStartElement(int element,
                                const SUMOSAXAttributes& attrs);

    /** @brief Called on the closing of a tag;
     *
     * @param[in] element ID of the currently closed element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myEndElement
     */
    virtual void myEndElement(int element);
    //@}



    class VehicleRemover : public MSMoveReminder {
    public:
        VehicleRemover(MSLane* const lane, MSCalibrator* const parent, const bool undo = false) :
            MSMoveReminder(parent->getID(), lane, true), myParent(parent), myUndoCalibration(undo) {}

        /// @name inherited from MSMoveReminder
        //@{
        /** @brief Checks whether the reminder is activated by a vehicle entering the lane
         *
         * Lane change means in this case that the vehicle changes to the lane
         *  the reminder is placed at.
         *
         * @param[in] veh The entering vehicle.
         * @param[in] reason how the vehicle enters the lane
         * @return True if vehicle enters the reminder.
         * @see Notification
         */
        //@}
        virtual bool notifyEnter(SUMOTrafficObject& veh, MSMoveReminder::Notification reason, const MSLane* enteredLane = nullptr);

        /** @brief Checks whether any calibrations need to be undone
         *
         * @param[in] veh The leaving vehicle.
         * @param[in] lastPos Position on the lane when leaving.
         * @param[in] reason how the vehicle leaves the lane
         * @see Notification
         *
         * @return True if the reminder wants to receive further info.
         */
        virtual bool notifyLeave(SUMOTrafficObject& veh, double lastPos, Notification reason, const MSLane* enteredLane = nullptr);

        void disable() {
            myParent = nullptr;
        }

    private:
        MSCalibrator* myParent;
        const bool myUndoCalibration;
    };
    friend class GUICalibrator;

    // @return whether the current state is active (GUI)
    bool isActive() const {
        return myAmActive;
    }

protected:

    void intervalEnd();

    bool isCurrentStateActive(SUMOTime time);

    bool tryEmit(MSLane* lane, MSVehicle* vehicle);

    void init();

    /// @brief number of vehicles expected to pass this interval
    int totalWished() const;

    /// @brief flow in the current interval in veh/h
    double currentFlow() const;

    /// @brief measured speed in the current interval
    double currentSpeed() const;

    /* @brief returns whether the lane is jammed although it should not be
     * @param[in] lane The lane to check or all for negative values
     */
    bool invalidJam(int laneIndex) const;

    inline int inserted() const {
        return myInserted;
    }
    inline int removed() const {
        return myRemoved;
    }
    inline int clearedInJam() const {
        return myClearedInJam;
    }

    /* @brief returns the number of vehicles (of the current type) that still
     * fit on the given lane
     * @param[in] lane The lane to check (return the maximum of all lanes for negative values)
     */
    int remainingVehicleCapacity(int laneIndex) const;

    /// @brief reset collected vehicle data
    virtual void reset();

    /// @brief aggregate lane values
    virtual void updateMeanData();

    /** @brief try to schedule the given vehicle for removal. return true if it
     * isn't already scheduled */
    bool scheduleRemoval(SUMOTrafficObject* veh) {
        return myToRemove.insert(veh->getID()).second;
    }


    /** @brief remove any vehicles which are scheduled for removal.
     * return true if removals took place */
    bool removePending();

    /// @brief determine id of new vehicle from calibrator state
    std::string getNewVehicleID();

protected:
    /// @brief the edge on which this calibrator lies
    MSEdge* const myEdge;
    /// @brief the lane on which this calibrator lies (nullptr if the whole edge is covered at once)
    MSLane* const myLane;
    /// @brief the junction on which this calibrator lies (nullptr if is edge or lane specific)
    MSJunction* const myNode;
    /// @brief the position on the edge where this calibrator lies
    const double myPos;
    /// @brief the route probe to retrieve routes from
    const MSRouteProbe* const myProbe;
    /// @brief dummy parent to retrieve vType filter
    MSMeanData_Net myMeanDataParent;
    /// @brief data collector for the calibrator
    std::vector<MSMeanData_Net::MSLaneMeanDataValues*> myLaneMeanData;
    /// @brief accumlated data for the whole edge
    MSMeanData_Net::MSLaneMeanDataValues myEdgeMeanData;

    /// @brief List of adaptation intervals
    std::vector<AspiredState> myIntervals;
    /// @brief Iterator pointing to the current interval
    std::vector<AspiredState>::const_iterator myCurrentStateInterval;

    std::vector<VehicleRemover*> myVehicleRemovers;

    /** @brief set of vehicle ids to remove
     * @note: we avoid keeping vehicle points because someone else might
     * invalidate it before look at it again (i.e. another calibrator)
     */
    std::set<std::string> myToRemove;

    /// @brief The device for xml statistics
    OutputDevice* myOutput;

    /// @brief The frequeny with which to check for calibration
    SUMOTime myFrequency;
    /// @brief The number of vehicles that were removed in the current interval
    int myRemoved;
    /// @brief The number of vehicles that were inserted in the current interval
    int myInserted;
    /// @brief The number of vehicles that were removed when clearin a jam
    int myClearedInJam;
    /// @brief The information whether the speed adaption has been reset
    bool mySpeedIsDefault;
    /// @brief The information whether speed was adapted in the current interval
    bool myDidSpeedAdaption;
    /// @brief The information whether init was called
    bool myDidInit;
    /// @brief The default (maximum) speed on the segment
    double myDefaultSpeed;
    /// @brief The default (maximum) speed on the segment
    bool myHaveWarnedAboutClearingJam;

    /// @brief whether the calibrator was active when last checking
    bool myAmActive;

    /// @brief relative speed threshold for detecting and clearing invalid jam
    double myInvalidJamThreshold;

    /// @brief whether the calibrator needs to undo the calibration after the edge / junction has been left
    bool myAmLocal;

    /// @brief whether the calibrator has registered an invalid jam in the last execution step
    bool myHaveInvalidJam;

    /* @brief objects which need to live longer than the MSCalibrator
     * instance which created them */
    static std::vector<MSMoveReminder*> myLeftoverReminders;
    static std::vector<SUMOVehicleParameter*> myLeftoverVehicleParameters;
    static std::map<std::string, MSCalibrator*> myInstances;

};
