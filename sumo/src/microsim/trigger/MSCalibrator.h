/****************************************************************************/
/// @file    MSCalibrator.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @date    Tue, May 2005
/// @version $Id$
///
// Calibrates the flow on an edge by removing an inserting vehicles
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
#ifndef MSCalibrator_h
#define MSCalibrator_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <utils/common/Command.h>
#include <microsim/MSRouteHandler.h>
#include <microsim/output/MSMeanData_Net.h>
#include <microsim/trigger/MSTrigger.h>


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
class MSCalibrator : public MSTrigger, public MSRouteHandler, public Command {
public:
    /** constructor */
    MSCalibrator(const std::string& id,
                 const MSEdge* const edge, const SUMOReal pos,
                 const std::string& aXMLFilename,
                 const std::string& outputFilename,
                 const SUMOTime freq, const SUMOReal length,
                 const MSRouteProbe* probe, const bool addLaneMeanData = true);

    /** destructor */
    virtual ~MSCalibrator();


    /** the implementation of the MSTrigger / Command interface.
        Calibrating takes place here. */
    virtual SUMOTime execute(SUMOTime currentTime);

    /// @brief cleanup remaining data structures
    static void cleanup();



protected:
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
        VehicleRemover(MSLane* lane, int laneIndex, MSCalibrator* parent) :
            MSMoveReminder(parent->getID(), lane, true), myLaneIndex(laneIndex), myParent(parent) {}

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
        virtual bool notifyEnter(SUMOVehicle& veh, Notification reason);

        void disable() {
            myParent = 0;
        }

    private:
        int myLaneIndex;
        MSCalibrator* myParent;
    };
    friend class VehicleRemover;

    // @return whether the current state is active (GUI)
    bool isActive() const {
        return myAmActive;
    }

protected:

    struct AspiredState {
        AspiredState() : begin(-1), end(-1), q(-1.), v(-1.), vehicleParameter(0) {}
        SUMOTime begin;
        SUMOTime end;
        SUMOReal q;
        SUMOReal v;
        SUMOVehicleParameter* vehicleParameter;
    };

    void writeXMLOutput();

    bool isCurrentStateActive(SUMOTime time);

    bool tryEmit(MSLane* lane, MSVehicle* vehicle);

    void init();

    inline virtual int passed() const {
        // calibrator measures at start of segment
        // vehicles drive to the end of an edge by default so they count as passed
        // but vaporized vehicles do not count
        // if the calibrator is located on a short edge, the vehicles are
        // vaporized on the next edge so we cannot rely on myEdgeMeanData.nVehVaporized
        return myEdgeMeanData.nVehEntered + myEdgeMeanData.nVehDeparted - myClearedInJam - myRemoved;
    }

    /// @brief number of vehicles expected to pass this interval
    int totalWished() const;

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

    /** @brief try to schedule the givne vehicle for removal. return true if it
     * isn't already scheduled */
    bool scheduleRemoval(MSVehicle* veh) {
        return myToRemove.insert(veh->getID()).second;
    };


    /** @brief remove any vehicles which are scheduled for removal.
     * return true if removals took place */
    bool removePending();

protected:
    /// @brief the edge on which this calibrator lies
    const MSEdge* const myEdge;
    /// @brief the position on the edge where this calibrator lies
    const SUMOReal myPos;
    /// @brief the route probe to retrieve routes from
    const MSRouteProbe* const myProbe;
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
    unsigned int myRemoved;
    /// @brief The number of vehicles that were inserted in the current interval
    unsigned int myInserted;
    /// @brief The number of vehicles that were removed when clearin a jam
    unsigned int myClearedInJam;
    /// @brief The information whether the speed adaption has been reset
    bool mySpeedIsDefault;
    /// @brief The information whether speed was adapted in the current interval
    bool myDidSpeedAdaption;
    /// @brief The information whether init was called
    bool myDidInit;
    /// @brief The default (maximum) speed on the segment
    SUMOReal myDefaultSpeed;
    /// @brief The default (maximum) speed on the segment
    bool myHaveWarnedAboutClearingJam;

    /// @brief whether the calibrator was active when last checking
    bool myAmActive;

    /* @brief objects which need to live longer than the MSCalibrator
     * instance which created them */
    static std::vector<MSMoveReminder*> LeftoverReminders;
    static std::vector<SUMOVehicleParameter*> LeftoverVehicleParameters;

};

#endif

/****************************************************************************/
