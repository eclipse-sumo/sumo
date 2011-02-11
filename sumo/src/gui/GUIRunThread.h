/****************************************************************************/
/// @file    GUIRunThread.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The thread that runs the simulation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIRunThread_h
#define GUIRunThread_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <iostream>
#include <fx.h>
#include <fx.h>
#include <utils/foxtools/FXSingleEventThread.h>
#include <utils/foxtools/FXRealSpinDial.h>
#include <utils/foxtools/FXThreadEvent.h>
#include <utils/foxtools/MFXEventQue.h>
#include <utils/foxtools/MFXMutex.h>
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUINet;
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
class GUIRunThread : public FXSingleEventThread {
public:
    /// constructor
    GUIRunThread(MFXInterThreadEventClient *mw,
                 FXRealSpinDial &simDelay, MFXEventQue &eq, FXEX::FXThreadEvent &ev);

    /// destructor
    virtual ~GUIRunThread();

    /// initialises the thread with the new simulation
    virtual void init(GUINet *net, SUMOTime start, SUMOTime end);

    /// starts the execution
    virtual FXint run();

    /** called when the user presses the "resume"-button,
        this method resumes the execution after a break */
    void resume();

    /** called when the user presses the "single step"-button,
    this method allows the thread to perform a single simulation step */
    void singleStep();

    /** starts the simulation (execution of one step after another) */
    virtual void begin();

    /** halts the simulation execution */
    void stop();

    /** returns the information whether a simulation has been loaded */
    bool simulationAvailable() const;

    virtual bool simulationIsStartable() const;
    virtual bool simulationIsStopable() const;
    virtual bool simulationIsStepable() const;

    /** deletes the existing simulation */
    virtual void deleteSim();

    /** returns the loaded network */
    GUINet &getNet() const;

    /** halts the thread before it shall be deleted */
    void prepareDestruction();

    /// Retrieves messages from the loading module
    void retrieveMessage(const MsgHandler::MsgType type, const std::string &msg);

protected:
    void makeStep() throw();

protected:
    /// the loaded simulation network
    GUINet *myNet;

    /// the times the simulation starts and ends with
    SUMOTime mySimStartTime, mySimEndTime;

    /// information whether the simulation is halting (is not being executed)
    bool myHalting;

    /** information whether the thread shall be stopped
    (if not, the thread stays in an endless loop) */
    bool myQuit;

    /** information whether a simulation step is being performed
    (otherwise the thread may be waiting or the simulation is maybe not
    performed at all) */
    bool mySimulationInProgress;

    bool myOk;

    /** information whether the thread is running in single step mode */
    bool mySingle;

    /** @brief The instances of message retriever encapsulations
        Needed to be deleted from the handler later on */
    OutputDevice *myErrorRetriever, *myMessageRetriever, *myWarningRetriever;

    FXRealSpinDial &mySimDelay;

    MFXEventQue &myEventQue;

    FXEX::FXThreadEvent &myEventThrow;

    MFXMutex mySimulationLock;

};


#endif

/****************************************************************************/

