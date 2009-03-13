/****************************************************************************/
/// @file    MSRouteProbe.h
/// @author  Michael Behrisch
/// @date    Thu, 04.12.2008
/// @version $Id$
///
// Writes route distributions at a certain edge
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSRouteProbe_h
#define MSRouteProbe_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <microsim/output/MSDetectorFileOutput.h>
#include <microsim/MSMoveReminder.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/common/Named.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSEdge;
class MSRoute;
class MSVehicle;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSRouteProbe
 * @brief Writes routes of vehicles passing a certain edge
 *
 * This device allows to create route distributions of all vehicles
 *  running over a certain edge.
 *
 * A frequency can be specified to generate the output in certain intervals,
 *  (e.g. every 900 seconds) and is used via the detector control by
 *  calling the appropriate methods derived from MSDetectorFileOutput.
 *
 * @see MSDetectorFileOutput
 * @see Named
 */

class MSRouteProbe : public MSDetectorFileOutput, public Named {
public:
    /**
     * @class EntryReminder
     * @brief A place on the road net (at a certain lane and position on it) where the E3-area begins
     */
    class EntryReminder : public MSMoveReminder {
    public:
        /** @brief Constructor
         *
         * @param[in] lane The lane the entry belongs to
         * @param[in] collector The detector the entry belongs to
         */
        EntryReminder(MSLane * const lane, MSRouteProbe& collector) throw();


        /// @name Methods inherited from MSMoveReminder
        /// @{

        /** @brief Checks whether the vehicle enters
         *
         * As soon as the reported vehicle enters the detector area (position>myPosition)
         *  the entering time is computed and both are added to the parent detector using
         *  "enter".
         *
         * @param[in] veh The vehicle in question.
         * @param[in] oldPos Position before the move-micro-timestep.
         * @param[in] newPos Position after the move-micro-timestep.
         * @param[in] newSpeed Unused here.
         * @return False, if vehicle passed the detector entierly, else true.
         * @see MSMoveReminder
         * @see MSMoveReminder::isStillActive
         * @see MSE3Collector::enter
         */
        bool isStillActive(MSVehicle& veh, SUMOReal , SUMOReal newPos, SUMOReal) throw();


        /** @brief Nothing is done, here
         *
         * @param[in] veh The leaving vehicle.
         * @see MSMoveReminder::dismissByLaneChange
         */
        void dismissByLaneChange(MSVehicle& veh) throw();


        /** @brief Returns whether the vehicle shall be aware of this entry
         *
         * Returns true if the vehicle is in front of the entry, so that it
         *  may enter it in later steps.
         *
         * @param[in] veh The vehicle that enters the lane
         * @param[in] isEmit true means emit, false: lane change
         * @see MSMoveReminder::isActivatedByEmitOrLaneChange
         * @return False, if vehicle passed the entry, else true.
         */
        bool isActivatedByEmitOrLaneChange(MSVehicle& veh, bool isEmit) throw();
        /// @}


    private:
        /// @brief The parent collector
        MSRouteProbe& myCollector;

    private:
        /// @brief Invalidated copy constructor.
        EntryReminder(const EntryReminder&);

        /// @brief Invalidated assignment operator.
        EntryReminder& operator=(const EntryReminder&);

    };


    /** @brief Constructor
     *
     * @param[in] id The id of the route probe
     * @param[in] edge The edge where the distribution shall be estimated
     * @param[in] begin The start of the first reporting interval
     */
    MSRouteProbe(const std::string &id,
                 const MSEdge *edge,
                 SUMOTime begin) throw();


    /// @brief Destructor
    virtual ~MSRouteProbe() throw();



    /// @name Methods inherited from MSDetectorFileOutput.
    /// @{

    /** @brief Writes values into the given stream
     *
     * This method writes the distribution of routes collected
     *  in the last interval.
     * As a side effect the distribution is added to the global
     *  route distribution container.
     *
     * @param[in] dev The output device to write the data into
     * @param[in] startTime First time step the data were gathered
     * @param[in] stopTime Last time step the data were gathered
     * @see MSDetectorFileOutput::writeXMLOutput
     * @exception IOError If an error on writing occures (!!! not yet implemented)
     */
    void writeXMLOutput(OutputDevice &dev,
                        SUMOTime startTime, SUMOTime stopTime) throw(IOError);


    /** @brief Opens the XML-output using "detector" as root element
     *
     * @param[in] dev The output device to write the root into
     * @see MSDetectorFileOutput::writeXMLDetectorProlog
     * @todo What happens with the additional information if several detectors use the same output?
     * @exception IOError If an error on writing occures (!!! not yet implemented)
     */
    void writeXMLDetectorProlog(OutputDevice &dev) const throw(IOError);
    /// @}


    /** adds the route to the current distribution */
    void addRoute(const MSRoute &route) const;

private:
    /// @brief The current distribution of routes (probability->route)
    RandomDistributor<const MSRoute*> *myCurrentRouteDistribution;

    /// @brief The move reminder notifying about new vehicles
    EntryReminder *myEntryReminder;


private:
    /// @brief Invalidated copy constructor.
    MSRouteProbe(const MSRouteProbe&);

    /// @brief Invalidated assignment operator.
    MSRouteProbe& operator=(const MSRouteProbe&);


};

#endif

/****************************************************************************/

