/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
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
/// @file    GUIRunThread.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
///
// The thread that runs the simulation
/****************************************************************************/
#pragma once
#include <config.h>

#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <utils/foxtools/fxheader.h>
#include <utils/foxtools/MFXSingleEventThread.h>
#include <utils/foxtools/MFXThreadEvent.h>
#include <utils/foxtools/MFXSynchQue.h>
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUINet;
class GUIEvent;
class OutputDevice;

// ===========================================================================
// class definition
// ===========================================================================
/**
 * @class GUIRunThread
 * This thread executes the given simulation stepwise to allow parallel
 * visualisation.
 * The avoidance of collisions between the simulation execution and its
 * visualisation is done individually for every lane using mutexes
 */
class GUIRunThread : public MFXSingleEventThread {

public:
    /// @brief constructor
    GUIRunThread(FXApp* app, MFXInterThreadEventClient* mw,
                 double& simDelay, MFXSynchQue<GUIEvent*>& eq,
                 FXEX::MFXThreadEvent& ev);

    /// @brief destructor
    virtual ~GUIRunThread();

    /// @brief initialises the thread with the new simulation
    virtual bool init(GUINet* net, SUMOTime start, SUMOTime end);

    /// @brief starts the execution
    virtual FXint run();

    /// @brief called when the user presses the "resume"-button
    /// @note this method resumes the execution after a break
    void resume();

    /// @brief called when the user presses the "single step"-button
    /// @note this method allows the thread to perform a single simulation step
    void singleStep();

    /// @brief starts the simulation (execution of one step after another)
    virtual void begin();

    /// @brief halts the simulation execution
    void stop();

    /// @brief returns the information whether a network has been loaded
    bool networkAvailable() const;

    /// @brief check if simulation is startable
    virtual bool simulationIsStartable() const;

    /// @brief check if simulation is stopableo
    virtual bool simulationIsStopable() const;

    /// @brief check if simulation is stepable
    virtual bool simulationIsStepable() const;

    /// @brief deletes the existing simulation
    virtual void deleteSim();

    /// @brief returns the loaded network
    GUINet& getNet() const;

    /// @brief halts the thread before it shall be deleted
    void prepareDestruction();

    /// @brief Retrieves messages from the loading module
    void retrieveMessage(const MsgHandler::MsgType type, const std::string& msg);

    /// @brief get simulation begin time
    SUMOTime getSimBegin() {
        return mySimStartTime;
    }

    /// @brief get simulation end time
    SUMOTime getSimEndTime() const {
        return mySimEndTime;
    }

    /// @brief get list of breakpoints
    std::vector<SUMOTime>& getBreakpoints() {
        return myBreakpoints;
    }

    /// @brief get breakpoint lock
    FXMutex& getBreakpointLock() {
        return myBreakpointLock;
    }

    /// @brief enable lib SUMO
    void enableLibsumo() {
        myAmLibsumo = true;
    }

    /// @brief try simulation step
    void tryStep();

protected:
    /// @brief make simulation step
    void makeStep();

    /// @brief wait for snapshots
    void waitForSnapshots(const SUMOTime snapshotTime);

protected:
    /// @brief the loaded simulation network
    GUINet* myNet;

    /// @brief the times the simulation starts and ends with
    SUMOTime mySimStartTime, mySimEndTime;

    /// @brief information whether the simulation is halting (is not being executed)
    bool myHalting;

    /// @brief information whether the thread shall be stopped
    /// @note if not, the thread stays in an endless loop)
    bool myQuit;

    /// @brief information whether a simulation step is being performed
    /// @note otherwise the thread may be waiting or the simulation is maybe not performed at all
    bool mySimulationInProgress;

    /// @brief flag to check if all is ok
    bool myOk;

    /// @brief information whether the thread is running in single step mode
    bool mySingle;

    /// @brief whether the simulation already ended
    bool myHaveSignaledEnd;

    /// @brief @brief The instances of message retriever encapsulations
    /// @note Needed to be deleted from the handler later on
    OutputDevice* myErrorRetriever, *myMessageRetriever, *myWarningRetriever;

    /// @brief simulation delay
    double& mySimDelay;

    /// @brief event queue
    MFXSynchQue<GUIEvent*>& myEventQue;

    /// @brief thrower events
    FXEX::MFXThreadEvent& myEventThrow;

    /// @brief mutex for lock simulation
    FXMutex mySimulationLock;

    /// @brief @brief List of breakpoints
    std::vector<SUMOTime> myBreakpoints;

    /// @brief @brief Lock for modifying the list of breakpoints
    FXMutex myBreakpointLock;

    /// @brief end of the last simulation step
    long myLastEndMillis;

    /// @brief last time the simulation took a microsecond break for the fox event loop to catch up (#9028)
    long myLastBreakMillis;

    /// @brief whether we are running in libsumo
    bool myAmLibsumo;
};
