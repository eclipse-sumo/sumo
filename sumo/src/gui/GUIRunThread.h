#ifndef GUIRunThread_h
#define GUIRunThread_h
//---------------------------------------------------------------------------//
//                        GUIRunThread.h -
//  The thread that runs the simulation
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.2  2003/02/07 10:34:14  dkrajzew
// files updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <iostream>
#include <qthread.h>
#include <microsim/MSNet.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUINet;
class GUIApplicationWindow;


/* =========================================================================
 * class definition
 * ======================================================================= */
/**
 * This thread executes the given simulation stepwise to allow parallel
 * visualisation.
 * The avoidance of collisions between the simulation execution and her
 * visualisation is done individually for every lane using mutexes
 */
class GUIRunThread : public QThread
{
public:
    /// constructor
    GUIRunThread(GUIApplicationWindow *mw, long sleepPeriod);

    /// destructor
    ~GUIRunThread();

    /// initialises the thread with the new simulation
    void init(GUINet *net, long start, long end, std::ostream *craw);

    /// starts the execution
    void run();

    /** called when the user presses the "resume"-button,
        this method resumes the execution after a break */
    void resume();

    /** called when the user presses the "single step"-button,
	this method allows the thread to perform a single simulation step */
    void singleStep();

    /** starts the simulation (execution of one step after another) */
    void begin();

    /** halts the simulation execution */
    void stop();

    /** returns the information whether a simulation has been loaded */
    bool simulationAvailable() const;

    /** deletes the existing simulation */
    void deleteSim();

    /** returns the simulation's current time step */
    MSNet::Time getCurrentTimeStep() const;

    /** returns the loaded network */
    GUINet &getNet() const;

    /** halts the thread before it shall be deleted */
    void prepareDestruction();

public slots:
    /** sets the number of milliseconds to wait between simulation steps */
    void setSimulationDelay(int value);

private:
    /// the parent application window
    GUIApplicationWindow    *_parent;

    /// the loaded simulation network
    GUINet                  *_net;

    /// the times the simulation starts and ends with
    MSNet::Time             _simStartTime, _simEndTime;

    /// the raw-output stream
    std::ostream            *_craw;

    /// information whether the simulation is halting (is not being executed)
    bool                    _halting;

    /// the curent simulation step
    MSNet::Time             _step;

    /** information whether the thread shall be stopped
	(if not, the thread stays in an endless loop) */
    bool                    _quit;

    /** information whether a simulation step is being performed
	(otherwise the thread may be waiting or the simulation is maybe not
	performed at all) */
    bool                    _simulationInProgress;

    /// the duration of a break between the simulation steps in ms */
    long                    _sleepPeriod;

    /** information whether the thread is running in single step mode */
    bool                    _single;
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "GUIRunThread.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

