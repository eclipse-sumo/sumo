/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2002-2022 German Aerospace Center (DLR) and others.
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
/// @file    MSRailSignalConstraint.h
/// @author  Jakob Erdmann
/// @date    August 2020
///
// A constraint on rail signal switching
/****************************************************************************/
#pragma once
#include <config.h>

#include <microsim/MSMoveReminder.h>

// ===========================================================================
// class declarations
// ===========================================================================
class MSRailSignal;
class SUMOSAXAttributes;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSRailSignalConstraint
 * @brief A base class for constraints
 */
class MSRailSignalConstraint {
public:
    /** @brief Constructor
     */
    MSRailSignalConstraint() {};

    /// @brief Destructor
    virtual ~MSRailSignalConstraint() {};

    /// @brief whether the constraint has been met
    virtual bool cleared() const = 0;

    virtual std::string getDescription() const {
        return "RailSignalConstraint";
    }

    virtual void write(OutputDevice& out, SumoXMLTag tag, const std::string& tripId) const = 0;

    /// @brief clean up state
    static void cleanup();

    /** @brief Saves the current constraint states into the given stream */
    static void saveState(OutputDevice& out);

    /** @brief Clear all constraint states before quick-loading state */
    static void clearState();

    /** @brief Remove all constraints before quick-loading state */
    static void clearAll();

protected:
    static std::string getVehID(const std::string& tripID);
};


class MSRailSignalConstraint_Predecessor : public MSRailSignalConstraint {
public:
    /** @brief Constructor
     */
    MSRailSignalConstraint_Predecessor(const MSRailSignal* signal, const std::string& tripId, int limit);

    /// @brief Destructor
    ~MSRailSignalConstraint_Predecessor() {};

    void write(OutputDevice& out, SumoXMLTag tag, const std::string& tripId) const;

    /// @brief clean up state
    static void cleanup();

    /** @brief Saves the current constraint states into the given stream */
    static void saveState(OutputDevice& out);

    /** @brief loads the constraint state from the given attrs */
    static void loadState(const SUMOSAXAttributes& attrs);

    /** @brief Clear all constraint states before quick-loading state */
    static void clearState();

    bool cleared() const;

    std::string getDescription() const;

    class PassedTracker : public MSMoveReminder {
    public:
        PassedTracker(MSLane* lane);

        /// @name inherited from MSMoveReminder
        //@{
        /// @brief tracks vehicles that passed this link (entered the next lane)
        bool notifyEnter(SUMOTrafficObject& veh, MSMoveReminder::Notification reason, const MSLane* enteredLane = 0);
        //@}

        void raiseLimit(int limit);

        bool hasPassed(const std::string& tripId, int limit) const;

        /** @brief Clear all passed states before quick-loading state */
        void clearState();

        /** @brief Saves the current passed states into the given stream */
        void saveState(OutputDevice& out);

        /** @brief loads the current passed states into the given stream */
        void loadState(int index, const std::vector<std::string>& tripIDs);

        /// @brief passed tripIds
        std::vector<std::string> myPassed;

        /// @brief index of the last passed object
        int myLastIndex;
    };

    /// @brief the tracker object for this constraint
    std::vector<PassedTracker*> myTrackers;

    /// @brief id of the predecessor that must already have passed
    const std::string myTripId;

    /// @brief the number of passed vehicles within which tripId must have occured
    const int myLimit;

    /// @brief store the foe signal (for TraCI access)
    const MSRailSignal* myFoeSignal;


    static std::map<const MSLane*, PassedTracker*> myTrackerLookup;

private:
    /// invalidated assignment operator
    MSRailSignalConstraint_Predecessor& operator=(const MSRailSignalConstraint_Predecessor& s) = delete;
};


