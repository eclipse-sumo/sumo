#ifndef GUIAbstractRunThread_h
#define GUIAbstractRunThread_h
//---------------------------------------------------------------------------//
//                        GUIAbstractRunThread.h -
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
// Revision 1.2  2005/02/01 10:10:48  dkrajzew
// got rid of MSNet::Time
//
// Revision 1.1  2004/11/23 10:38:32  dkrajzew
// debugging
//
// Revision 1.1  2004/10/22 12:50:57  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.6  2004/07/02 08:28:50  dkrajzew
// some changes needed to derive the threading classes more easily added
//
// Revision 1.5  2004/04/02 11:10:20  dkrajzew
// simulation-wide output files are now handled by MSNet directly
//
// Revision 1.4  2004/03/19 12:54:08  dkrajzew
// porting to FOX
//
// Revision 1.3  2003/11/26 10:57:14  dkrajzew
// messages from the simulation are now also passed to the message handler
//
// Revision 1.2  2003/02/07 10:34:14  dkrajzew
// files updated
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <iostream>
#include <fx.h>
#include <fx.h>
#include <utils/foxtools/FXSingleEventThread.h>
#include <utils/foxtools/FXRealSpinDial.h>
#include <utils/foxtools/FXMutex.h>
#include <utils/foxtools/FXThreadEvent.h>
#include <utils/foxtools/MFXEventQue.h>
#include <utils/common/SUMOTime.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUINet;
class GUIApplicationWindow;
class MsgRetriever;


/* =========================================================================
 * class definition
 * ======================================================================= */
/**
 * @class GUIAbstractRunThread
 * This thread executes the given simulation stepwise to allow parallel
 * visualisation.
 * The avoidance of collisions between the simulation execution and her
 * visualisation is done individually for every lane using mutexes
 */
class GUIAbstractRunThread : public FXSingleEventThread
{
public:
    /// constructor
    GUIAbstractRunThread(GUIApplicationWindow *mw,
        FXRealSpinDial &simDelay, MFXEventQue &eq, FXEX::FXThreadEvent &ev);

    /// destructor
    virtual ~GUIAbstractRunThread();

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

    /** returns the simulation's current time step */
    SUMOTime getCurrentTimeStep() const;

    /** halts the thread before it shall be deleted */
    void prepareDestruction();

    /// Retrieves messages from the loading module
    void retrieveMessage(const std::string &msg);

    /// Retrieves warnings from the loading module
    void retrieveWarning(const std::string &msg);

    /// Retrieves error from the loading module
    void retrieveError(const std::string &msg);

protected:
    void makeStep();

protected:
    /// the times the simulation starts and ends with
    SUMOTime             _simStartTime, _simEndTime;

    /// information whether the simulation is halting (is not being executed)
    bool                    _halting;

    /// the curent simulation step
    SUMOTime             _step;

    /** information whether the thread shall be stopped
    (if not, the thread stays in an endless loop) */
    bool                    _quit;

    /** information whether a simulation step is being performed
    (otherwise the thread may be waiting or the simulation is maybe not
    performed at all) */
    bool                    _simulationInProgress;

    bool _ok;

    /** information whether the thread is running in single step mode */
    bool                    _single;

    /** @brief The instances of message retriever encapsulations
        Needed to be deleted from the handler later on */
    MsgRetriever *myErrorRetriever, *myMessageRetriever, *myWarningRetreiver;

    FXRealSpinDial &mySimDelay;

    MFXEventQue &myEventQue;

    FXEX::FXThreadEvent &myEventThrow;

    FXEX::FXMutex mySimulationLock;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

