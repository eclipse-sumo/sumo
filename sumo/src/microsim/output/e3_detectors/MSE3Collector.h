/****************************************************************************/
/// @file    MSE3Collector.h
/// @author  Christian Roessel
/// @date    Tue Dec 02 2003 22:17 CET
/// @version $Id$
///
// A detector of vehicles passing an area between entry/exit points
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
#ifndef MSE3Collector_h
#define MSE3Collector_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <microsim/MSMoveReminder.h>
#include <microsim/output/MSDetectorFileOutput.h>
#include <string>
#include <cassert>
#include <vector>
#include <limits>
#include <microsim/output/MSCrossSection.h>
#include <microsim/MSVehicleQuitReminded.h>
#include <microsim/MSUpdateEachTimestep.h>
#include <utils/common/ToString.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSE3Collector
 * @brief A detector of vehicles passing an area between entry/exit points
 * 
 * E3-detectors are defined by a set of in-cross-sections and out-cross-sections. 
 * Vehicles, that pass an in- and out-cross-section are detected when they pass the
 * out-cross-section. Vehicles passing the out-cross-section without having 
 * passed the in-cross-section are not detected. You can add detectors via 
 * addDetector() out of E3::DetType to work on the detection area. 
 * You get a sampled value via getAggregate(). As MSE3Collector inherits from 
 * MSDetectorFileOutput there is the possibility to get file output by calling
 * MSDetector2File::addDetectorAndInterval().
 */
class MSE3Collector : public MSDetectorFileOutput,
            public MSVehicleQuitReminded,
            public MSUpdateEachTimestep< MSE3Collector >
{
public:
    /**
     * @class MSE3EntryReminder
     * @brief A place on the road net where the E3-area begins
     */
    class MSE3EntryReminder : public MSMoveReminder
    {
    public:
        /// Constructor
        MSE3EntryReminder(
            const MSCrossSection &crossSection, MSE3Collector& collector);

        //{ methods from MSMoveReminder
        bool isStillActive(MSVehicle& veh, SUMOReal , SUMOReal newPos, SUMOReal);

        void dismissByLaneChange(MSVehicle& veh);

        bool isActivatedByEmitOrLaneChange(MSVehicle& veh);
        //}

    private:
        /// The parent collector
        MSE3Collector& collectorM;

        /// The position on the lane
        SUMOReal posM;

    };


    /**
     * @class MSE3LeaveReminder
     * @brief A place on the road net where the E3-area ends
     */
    class MSE3LeaveReminder : public MSMoveReminder
    {
    public:
        /// Constructor
        MSE3LeaveReminder(
            const MSCrossSection &crossSection, MSE3Collector& collector);

        //{ methods from MSMoveReminder
        bool isStillActive(MSVehicle& veh, SUMOReal , SUMOReal newPos, SUMOReal);

        void dismissByLaneChange(MSVehicle& veh);

        bool isActivatedByEmitOrLaneChange(MSVehicle& veh);
        //}

    private:
        /// The parent collector
        MSE3Collector& collectorM;

        /// The position on the lane
        SUMOReal posM;

    };

    /**
     * @enum Value
     * @brief An enumeration of measures the e3-collector collects
     */
    enum Value {
        /// Mean travel time [s]
        MEAN_TRAVELTIME = 0,
        /// Mean number of haltings per vehicles [#]
        MEAN_NUMBER_OF_HALTINGS_PER_VEHICLE,
        /// The number of vehicles [#}
        NUMBER_OF_VEHICLES,
        /// Mean speed [m/s]
        MEAN_SPEED
    };

    typedef std::vector<MSE3EntryReminder*> EntryReminders;
    typedef std::vector<MSE3LeaveReminder*> LeaveReminders;

    /// Ctor. Sets reminder objects on entry- and leave-lanes and
    /// inserts itself to an E3Dictionary for global access to all
    /// MSE3Collector objects.
    ///
    /// @param id The detector's unique id.
    /// @param entries Entry-cross-sections.
    /// @param exits Leavey-cross-sections.
    /// @param haltingTimeThreshold A vehicle must at least drive that time
    /// at a speed lesser than haltingSpeedThreshold to be a "halting" vehicle.
    /// @param haltingSpeedThreshold A vehicle must not drive a greater speed
    /// for at more than haltingTimeThreshold  to be a "halting" vehicle.
    ///
    MSE3Collector(const std::string &id,
                  const CrossSectionVector &entries, const CrossSectionVector &exits,
                  MSUnit::Seconds haltingTimeThreshold, MSUnit::MetersPerSecond haltingSpeedThreshold);

    /// Dtor. Deletes the created detectors.
    virtual ~MSE3Collector(void);

    /** @brief Call if a vehicle touches an entry-cross-section. 
     *
     * Inserts vehicle into internal containers.*/
    void enter(MSVehicle& veh, SUMOReal entryTimestep);

    /** @brief Call if a vehicle passes a leave-cross-section. 
     *
     * Removes vehicle from internal containers. */
    void leave(MSVehicle& veh, SUMOReal leaveTimestep);

    /// Get the detectors unique id.
    const std::string& getID() const;

    /// Remove vehicles that entered the detector but reached their
    /// destination before passing the leave-cross-section from
    /// internal containers.
    void removeOnTripEnd(MSVehicle *veh);

    /// Writes the header into the given device
    void writeXMLHeader(OutputDevice &dev) const;

    /// Writes the current output into the device
    void writeXMLOutput(OutputDevice &dev, SUMOTime startTime, SUMOTime stopTime);

    /// Get an opening XML-tag containing information about the detector.
    void writeXMLDetectorInfoStart(OutputDevice &dev) const;

    /// Get an closing XML-tag corresponding to the opening tag from
    /// getXMLDetectorInfoStart().
    ///
    /// @return The member infoEndM.
    ///
    void writeXMLDetectorInfoEnd(OutputDevice &dev) const;

    /// @}


        bool updateEachTimestep(void);

        SUMOReal getValue(Value which) const;

protected:
    std::string idM;            ///< The detector's unique id.

    CrossSectionVector entriesM; ///< Container of detector entries.
    CrossSectionVector exitsM; ///< Container of detector exits.

    EntryReminders entryRemindersM; ///< Container of entryReminders.
    LeaveReminders leaveRemindersM; ///< Container of leaveReminders.

    // Time-theshold to determine if a vehicle is halting.
    //MSUnit::Steps haltingTimeThresholdM; 
    // !dk! kept for later use

    /// Speed-theshold to determine if a vehicle is halting.
    MSUnit::MetersPerSecond haltingSpeedThresholdM;

    static std::string xmlHeaderM; ///< Header for the XML-output.

    static std::string infoEndM; ///< Closing detector tag.

    struct E3Values {
        SUMOReal entryTime;
        SUMOReal leaveTime;
        SUMOReal speedSum;
        size_t haltings;
    };

    std::map<MSVehicle*, E3Values> myEnteredContainer;
    std::map<MSVehicle*, E3Values> myLeftContainer;

};


#endif

/****************************************************************************/

