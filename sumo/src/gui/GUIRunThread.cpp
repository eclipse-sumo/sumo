//---------------------------------------------------------------------------//
//                        GUIRunThread.cpp -
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
namespace
{
    const char rcsid[] =
        "$Id$";
}
// $Log$
// Revision 1.22  2004/03/19 12:54:08  dkrajzew
// porting to FOX
//
// Revision 1.21  2004/02/05 16:29:30  dkrajzew
// memory leaks removed
//
// Revision 1.20  2003/12/11 06:21:15  dkrajzew
// implemented MSVehicleControl as the instance responsible for vehicles
//
// Revision 1.19  2003/12/09 11:22:13  dkrajzew
// errors during simulation are now caught properly
//
// Revision 1.18  2003/12/04 13:24:45  dkrajzew
// error handliung improved
//
// Revision 1.17  2003/11/26 10:57:14  dkrajzew
// messages from the simulation are now also passed to the message handler
//
// Revision 1.16  2003/11/20 13:17:33  dkrajzew
// further work on aggregated views
//
// Revision 1.15  2003/11/12 14:06:33  dkrajzew
// visualisation of tl-logics added
//
// Revision 1.14  2003/11/11 08:42:14  dkrajzew
// synchronisation problems of parameter tracker updates patched
//
// Revision 1.13  2003/10/30 08:57:53  dkrajzew
// first implementation of aggregated views using E2-detectors
//
// Revision 1.12  2003/08/20 11:58:04  dkrajzew
// cleaned up a bit
//
// Revision 1.11  2003/07/22 14:56:46  dkrajzew
// changes due to new detector handling
//
// Revision 1.10  2003/06/19 10:56:03  dkrajzew
// user information about simulation ending added; the gui may shutdown on
//  end and be started with a simulation now;
//
// Revision 1.9  2003/06/06 11:12:38  dkrajzew
// deletion of singletons changed/added
//
// Revision 1.8  2003/06/06 10:33:47  dkrajzew
// changes due to moving the popup-menus into a subfolder
//
// Revision 1.7  2003/06/05 06:26:16  dkrajzew
// first tries to build under linux: warnings removed; Makefiles added
//
// Revision 1.6  2003/05/20 09:23:54  dkrajzew
// some statistics added; some debugging done
//
// Revision 1.5  2003/04/04 08:37:50  dkrajzew
// view centering now applies net size; closing problems debugged;
//  comments added; tootip button added
//
// Revision 1.4  2003/02/07 10:34:14  dkrajzew
// files updated
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <cassert>
#include <string>
#include <iostream>

#include <utils/common/MsgRetrievingFunction.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <guisim/GUINet.h>
#include <microsim/MSVehicleControl.h>
#include <helpers/SingletonDictionary.h>
#include "GUIEvent_Message.h"
#include "GUIEvent_SimulationStep.h"
#include "GUIEvent_SimulationEnded.h"
#include "GUIApplicationWindow.h"
#include "GUIRunThread.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace FXEX;
using namespace std;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUIRunThread::GUIRunThread(GUIApplicationWindow *parent,
                           FXRealSpinDial &simDelay, MFXEventQue &eq,
                           FXEX::FXThreadEvent &ev)
    : FXSingleEventThread(parent->getApp(), parent), _parent(parent),
    _net(0), _craw(0), _quit(false), _simulationInProgress(false),
    mySimDelay(simDelay), myEventQue(eq), myEventThrow(ev)
{
    myErrorRetriever = new MsgRetrievingFunction<GUIRunThread>(this,
        &GUIRunThread::retrieveError);
    myMessageRetriever = new MsgRetrievingFunction<GUIRunThread>(this,
        &GUIRunThread::retrieveMessage);
    myWarningRetreiver = new MsgRetrievingFunction<GUIRunThread>(this,
        &GUIRunThread::retrieveWarning);
    // register message callbacks
    MsgHandler::getErrorInstance()->addRetriever(myErrorRetriever);
    MsgHandler::getWarningInstance()->addRetriever(myWarningRetreiver);
    MsgHandler::getMessageInstance()->addRetriever(myMessageRetriever);
}


GUIRunThread::~GUIRunThread()
{
    // the thread shall stop
    _quit = true;
    // remove message callbacks
    MsgHandler::getErrorInstance()->removeRetriever(myErrorRetriever);
    MsgHandler::getWarningInstance()->removeRetriever(myWarningRetreiver);
    MsgHandler::getMessageInstance()->removeRetriever(myMessageRetriever);
    delete myErrorRetriever;
    delete myMessageRetriever;
    delete myWarningRetreiver;
    // wait for the thread
    while(_simulationInProgress||_net!=0);
}


void
GUIRunThread::init(GUINet *net, long start, long end, std::ostream *craw)
{
    // delete a maybe existing simulation
    deleteSim();
    // assign new values
    _net = net;
    _craw = craw;
    _simStartTime = start;
    _simEndTime = end;
    _step = start;
    _net->initialiseSimulation(_craw);
}


FXint
GUIRunThread::run()
{
    GUIEvent *e = 0;
    // perform an endless loop
    while(!_quit) {
	    // sleep when no net is available
        if(_net==0) {
            sleep(500);
        }
	    // if the simulation shall be perfomed, do it
        if(!_halting&&_net!=0) {
	        // simulation is being perfomed
            _simulationInProgress = true;
	        // execute a single step
            try {
                mySimulationLock.lock();
                _net->simulationStep(_craw, _simStartTime, _step);
                _net->guiSimulationStep();
                mySimulationLock.unlock();

                // inform parent that a step has been performed
                e = new GUIEvent_SimulationStep();
                myEventQue.add(e);
                myEventThrow.signal();
	            // increase step counter
                _step++;
	            // stop the simulation when the last step has been reached
                if(_step==_simEndTime) {
                    e = new GUIEvent_SimulationEnded(
                        GUIEvent_SimulationEnded::ER_END_STEP_REACHED, _step);
                    myEventQue.add(e);
                    myEventThrow.signal();
                    _halting = true;
                }
    	        // stop the execution when only a single step should have
	            //  been performed
                if(_single) {
                    _halting = true;
                }
	            // simulation step is over
                _simulationInProgress = false;
                // check whether all vehicles loaded have left the simulation
                if(_net->getVehicleControl().haveAllVehiclesQuit()) {
                    _halting = true;
                    e = new GUIEvent_SimulationEnded(
                        GUIEvent_SimulationEnded::ER_NO_VEHICLES, _step-1);
                    myEventQue.add(e);
                    myEventThrow.signal();
                }
            } catch (ProcessError &exc) {
                _simulationInProgress = false;
                e = new GUIEvent_SimulationEnded(
                    GUIEvent_SimulationEnded::ER_ERROR_IN_SIM, _step);
                myEventQue.add(e);
                myEventThrow.signal();
                _halting = true;
            }
        }
        double val = mySimDelay.getValue();
        sleep((int) val);
    }
    // delete a maybe existing simulation at the end
    deleteSim();
    return 0;
}


void
GUIRunThread::resume()
{
    if(_step<_simEndTime) {
        _single = false;
        _halting = false;
    }
}


void
GUIRunThread::singleStep()
{
    _single = true;
    _halting = false;
}


void
GUIRunThread::begin()
{
    _step = _simStartTime;
    _net->preStartInit();
    _single = false;
    _halting = false;
}


void
GUIRunThread::stop()
{
    _single = false;
    _halting = true;
}


bool
GUIRunThread::simulationAvailable() const
{
    return _net!=0;
}


void
GUIRunThread::deleteSim()
{
    _halting = true;
    mySimulationLock.lock();
    if(_net!=0) {
        _net->closeSimulation(_craw);
    }
    while(_simulationInProgress);
    delete _net;
    _net = 0;
    delete _craw;
    _craw = 0;
    mySimulationLock.unlock();
}


GUINet &
GUIRunThread::getNet() const
{
    return *_net;
}


MSNet::Time
GUIRunThread::getCurrentTimeStep() const
{
    return _step;
}



void
GUIRunThread::prepareDestruction()
{
    _halting = true;
    _quit = true;
}


void
GUIRunThread::retrieveMessage(const std::string &msg)
{
    if(!_simulationInProgress) {
        return;
    }
    GUIEvent *e = new GUIEvent_Message(MsgHandler::MT_MESSAGE, msg);
    myEventQue.add(e);
    myEventThrow.signal();
}


void
GUIRunThread::retrieveWarning(const std::string &msg)
{
    if(!_simulationInProgress) {
        return;
    }
    GUIEvent *e = new GUIEvent_Message(MsgHandler::MT_WARNING, msg);
    myEventQue.add(e);
    myEventThrow.signal();
}


void
GUIRunThread::retrieveError(const std::string &msg)
{
    if(!_simulationInProgress) {
        return;
    }
    GUIEvent *e = new GUIEvent_Message(MsgHandler::MT_ERROR, msg);
    myEventQue.add(e);
    myEventThrow.signal();
}


bool
GUIRunThread::simulationIsStartable() const
{
    return _net!=0&&(_halting||_single);
}


bool
GUIRunThread::simulationIsStopable() const
{
    return _net!=0&&(!_halting);
}


bool
GUIRunThread::simulationIsStepable() const
{
    return _net!=0&&(_halting||_single);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:




