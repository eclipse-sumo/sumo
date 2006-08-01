#ifndef MSE3COLLECTOR_H
#define MSE3COLLECTOR_H

///
/// @file    MSE3Collector.h
/// @author  Christian Roessel <christian.roessel@dlr.de>
/// @date    Started Tue Dec 02 2003 22:17 CET
/// @version
///
/// @brief
///
///

/* Copyright (C) 2003, 2004 by German Aerospace Center (http://www.dlr.de) */

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <microsim/MSMoveReminder.h>
#include <microsim/output/MSDetectorFileOutput.h>
#include <string>
#include <cassert>
#include <vector>
#include <limits>
#include <microsim/output/MSDetectorContainerWrapperBase.h>
#include <microsim/output/MSDetectorContainerWrapper.h>
#include <microsim/output/MSDetectorCounterContainerWrapper.h>
#include <microsim/output/MSDetectorHaltingContainerWrapper.h>
#include <microsim/output/MSDetectorOccupancyCorrection.h>
#include <microsim/output/MSCrossSection.h>
#include <microsim/output/MSLDDetectorInterface.h>
#include <microsim/output/MSDetectorTypedefs.h>
#include <microsim/MSVehicleQuitReminded.h>
#include <utils/common/ToString.h>
#include <utils/iodevices/XMLDevice.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/// Collection of E3-detectors. E3-detectors are defined by a set of
/// in-cross-sections and out-cross-sections. Vehicles, that pass an
/// in- and out-cross-section are detected when they pass the
/// out-cross-section. Vehicles passing the out-cross-section without
/// having passed the in-cross-section are not detected. You can add
/// detectors via addDetector() out of E3::DetType to work on the
/// detection area. You get a sampled value via getAggregate(). As
/// MSE3Collector inherits from MSDetectorFileOutput there is the
/// possibility to get file output by calling
/// MSDetector2File::addDetectorAndInterval().
///
class MSE3Collector : public MSDetectorFileOutput,
                      public MSVehicleQuitReminded
{
public:
    /**
    *
    */
    class MSE3EntryReminder : public MSMoveReminder
    {
    public:
        MSE3EntryReminder(const std::string &id,
            const MSCrossSection &crossSection, MSE3Collector& collector);

        bool isStillActive(MSVehicle& veh, SUMOReal , SUMOReal newPos, SUMOReal);

        void dismissByLaneChange( MSVehicle& veh );

        bool isActivatedByEmitOrLaneChange( MSVehicle& veh );

    private:
        MSE3Collector& collectorM;
        SUMOReal posM;

    };



    class MSE3LeaveReminder : public MSMoveReminder
    {
    public:
        MSE3LeaveReminder(const std::string &id,
            const MSCrossSection &crossSection, MSE3Collector& collector);

        bool isStillActive(MSVehicle& veh, SUMOReal , SUMOReal newPos, SUMOReal);

        void dismissByLaneChange( MSVehicle& veh );

        bool isActivatedByEmitOrLaneChange( MSVehicle& veh );

    private:
        MSE3Collector& collectorM;
        SUMOReal posM;

    };

    ///
    /// Collection of all possible E3-detectors. Names should be
    /// self-explanatory. All E3-detectors are LD-detectors (leave-data).
    ///
    enum DetType {
        MEAN_TRAVELTIME = 0,
        MEAN_NUMBER_OF_HALTINGS_PER_VEHICLE,
        NUMBER_OF_VEHICLES,
        ALL        ///< Use this to generate all possible
                                ///detectors in
                                ///MSE3Collector::addDetector().
    };

    /// Increment operator that allows us to iterate over the
    /// E3::DetType detectors.
    ///
    /// @param det A detector out of E3::DetType (an integral type).
    ///
    /// @return The detector that follows det.
    ///
    friend MSE3Collector::DetType& operator++( MSE3Collector::DetType& det );

    ///
    /// Collection of different "vehicle" containers used by MSE3Collector.
    ///
    enum Containers {
        VEHICLES = 0,
        HALTINGS,
        TRAVELTIME
    };

    /// Increment operator that allows us to iterate over the
    /// E3::Containers detectors.
    ///
    /// @param cont A container out of E3::Containers (an integral type).
    ///
    /// @return The container that follows cont.

    friend MSE3Collector::Containers& operator++( MSE3Collector::Containers& cont );


    typedef LD::MSDetectorInterface LDDetector;
    typedef std::vector< LDDetector* > DetectorCont;
    typedef DetectorCont::iterator DetContIter;

    typedef std::vector< MSDetectorContainerWrapperBase* > ContainerCont;
    typedef ContainerCont::iterator ContainerContIter;
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
    /// @param deleteDataAfterSeconds Delete the data that is collected every
    /// timestep so that data is always present for at least
    /// deleteDataAfterSeconds.
    ///
    MSE3Collector(const std::string &id,
        const CrossSectionVector &entries, const CrossSectionVector &exits,
        MSUnit::Seconds haltingTimeThreshold, MSUnit::MetersPerSecond haltingSpeedThreshold,
        SUMOTime deleteDataAfterSeconds);

    /// Dtor. Deletes the created detectors.
    virtual ~MSE3Collector( void );

    /// Call if a vehicle touches an entry-cross-section. Usually
    /// called by LD::MSMoveReminder. Inserts vehicle into internal
    /// containers.
    ///
    /// @param veh The entering vehicle.
    ///
    void enter( MSVehicle& veh );

    /// Call if a vehicle passes a leave-cross-section. Usually called
    /// by LD::MSMoveReminder. Removed vehicle from internal
    /// containers.
    ///
    /// @param veh The leaving vehicle.
    ///
    void leave( MSVehicle& veh );

    /// Add, i.e. create the requested detector or all detectors out
    /// of E3::DetType.
    ///
    /// @param type One detetcor out of E3::DetType. ALL will create
    /// all detectors.
    /// @param detId Optional id of the newly created detector(s).
    ///
    void addDetector( DetType type, std::string detId = "" );

    /// Checks if the requested detector is present.
    ///
    /// @param type The detector out of E3::DetType you are interested in.
    ///
    /// @return True if the detector exists, false otherwise.
    ///
    bool hasDetector( DetType type );

    /// Get the aggregated value of a detector. If the detector
    /// doesn't exist, create it and return -1 for this first call.
    ///
    /// @param type The detector of E3::DetType you are interested in.
    /// @param lastNSeconds Length of the aggregation intervall
    /// (now-lastNSeconds, now].
    ///
    /// @return If the detector exists, return it's aggregated value,
    /// else return -1.
    ///
    SUMOReal getAggregate( DetType type, MSUnit::Seconds lastNSeconds );

    /// Get the detectors unique id.
    ///
    /// @return Th detectors unique id.
    ///
    const std::string& getID() const;

    /// Remove vehicles that entered the detector but reached their
    /// destination before passing the leave-cross-section from
    /// internal containers.
    ///
    /// @param veh The vehicle to remove.
    ///
    void removeOnTripEnd( MSVehicle *veh );

    /// Get a header for file output which shall contain some
    /// explanation of the output generated by getXMLOutput.
    ///
    /// @return The member xmlHeaderM.
    ///
    void writeXMLHeader( XMLDevice &dev ) const;

    /// Get the XML-formatted output of the concrete detector.  Calls
    /// resetQueueLengthAheadOfTrafficLights() if the detector
    /// E2::QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_VEHICLES exists.
    ///
    /// @param lastNTimesteps Generate data out of the interval
    /// (now-lastNTimesteps, now].
    ///
    /// @return XML-output of all existing concrete detectors. Except
    /// APPROACHING_VEHICLES_STATES.
    ///
    void writeXMLOutput( XMLDevice &dev, SUMOTime startTime, SUMOTime stopTime );

    /// Get an opening XML-tag containing information about the detector.
    ///
    /// @return String describing the detetctor-collection.
    ///
    void writeXMLDetectorInfoStart( XMLDevice &dev ) const;

    /// Get an closing XML-tag corresponding to the opening tag from
    /// getXMLDetectorInfoStart().
    ///
    /// @return The member infoEndM.
    ///
    void writeXMLDetectorInfoEnd( XMLDevice &dev ) const;

    /// Get the clean-up interval length.
    ///
    /// @return Interval-length in steps.
    ///
    SUMOTime getDataCleanUpSteps( void ) const;

    /// @}

protected:

    /// Get a pointer to the requested detector out of E3::DetType.
    ///
    /// @param type The detector you are interested in.
    ///
    /// @return If the detector exists, a pointer to the valid object,
    /// 0 otherwise.
    ///
    LDDetector* getDetector( DetType type ) const;

    std::string idM;            ///< The detector's unique id.

    CrossSectionVector entriesM; ///< Container of detector entries.
    CrossSectionVector exitsM; ///< Container of detector exits.

    EntryReminders entryRemindersM; ///< Container of entryReminders.
    LeaveReminders leaveRemindersM; ///< Container of leaveReminders.

    MSUnit::Steps haltingTimeThresholdM; ///< Time-theshold to
                                         ///determine if a vehicle is
                                         ///halting.
    MSUnit::CellsPerStep haltingSpeedThresholdM; ///< Speed-theshold
                                                 ///to determine if a
                                                 ///vehicle is
                                                 ///halting.
    SUMOTime deleteDataAfterSecondsM; ///< Data removal interval.

    DetectorCont detectorsM;    ///< Container of E3-detectors.

    ContainerCont containersM;  ///< Container of helper-containers.

    static std::string xmlHeaderM; ///< Header for the XML-output.

    static std::string infoEndM; ///< Closing detector tag.

    /// Create a "vehicle"-container out of E3::Containers. They may
    /// be shared among several detectors. The created container is
    /// put into the member containersM. A container holds pointers to
    /// all "vehicles" currently on the detector.
    ///
    /// @param type The container you are interested in.
    ///
    void createContainer( Containers type );

    /// Create a detector out of E3::DetType. The created detector is
    /// put into member detectorsM. On creation, the detector gets one
    /// of the "vehicle"-containers out of E3::Containers.
    ///
    /// @param type The detector you are interested in.
    /// @param detId Detector-id, need not be unique.
    ///
    void createDetector( DetType type, std::string detId );

    /// Get aggregated XML-output for all detectors in a container.
    ///
    /// @param container A container holding detectors.
    /// @param lastNTimesteps The length of the aggregation interval.
    ///
    /// @return A XML-formatted string.
    ///
    template< class Cont >
    void writeXMLOutput( XMLDevice &dev, Cont& container,
                        SUMOTime startTime, SUMOTime stopTime) {
        MSUnit::Seconds lastNSeconds = (MSUnit::Seconds) stopTime-startTime+1;
        for ( typename Cont::iterator it = container.begin(); it != container.end(); ++it ) {
            if ( *it == 0 ) {
                continue;
            }
            dev.writeString((*it)->getName()).writeString("=\"").writeString(
                toString( (*it)->getAggregate( lastNSeconds ))).writeString(
                "\" ");
        }
    }

    /// Deletes the elements of a container.
    ///
    /// @param cont The container whose elements shall be deleted.
    ///
    template< class Cont >
    void deleteContainer( Cont& cont ) {
        for ( typename Cont::iterator it = cont.begin(); it != cont.end(); ++it ) {
            if ( *it != 0 ) {
                delete *it;
            }
        }
    }

};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

#endif // MSE3COLLECTOR_H
