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
// Revision 1.7  2003/06/05 06:26:16  dkrajzew
// first tries to build under linux: warnings removed; Makefiles added
//
// Revision 1.6  2003/05/20 09:23:54  dkrajzew
// some statistics added; some debugging done
//
// Revision 1.5  2003/04/04 08:37:50  dkrajzew
// view centering now applies net size; closing problems debugged; comments added; tootip button added
//
// Revision 1.4  2003/02/07 10:34:14  dkrajzew
// files updated
//
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
#include <helpers/SingletonDictionary.h>
#include <microsim/MSLaneState.h>
#include <qthread.h>
#include <guisim/GUINet.h>
#include "QSimulationStepEvent.h"
#include "GUIApplicationWindow.h"
#include "GUIRunThread.h"


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUIRunThread::GUIRunThread(GUIApplicationWindow *parent, long sleepPeriod)
    : _parent(parent),
    _net(0), _craw(0), _quit(false), _simulationInProgress(false),
    _sleepPeriod(sleepPeriod)
{
}


GUIRunThread::~GUIRunThread()
{
    // the thread shall stop, wait for it
    _quit = true;
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
    SingletonDictionary< std::string, MSLaneState* >::create();
    _net->initialiseSimulation(_craw, start, end);
}


void
GUIRunThread::run()
{
    // perform an endless loop
    while(!_quit) {
	    // sleep when no net is available
        if(_net==0) {
            msleep(500);
        }
	    // if the simulation shall be perfomed, do it
        if(!_halting&&_net!=0) {
	        // simulation is being perfomed
            _simulationInProgress = true;
	        // execute a single step
            _net->simulationStep(_craw, _simStartTime, _step);
	        // inform parent that a step has been performed
            QThread::postEvent( _parent, new QSimulationStepEvent() );
	        // increase step counter
            _step++;
	        // stop the simulation when the last step has been reached
            if(_step==_simEndTime) {
                _halting = true;
            }
	        // stop the execution when only a single step should have
	        //  been performed
            if(_single) {
                _halting = true;
            }
	        // simulation step is over
            _simulationInProgress = false;
	        // sleep, but only when the simulation is continuing
        }
        msleep(_sleepPeriod);
    }
    // delete a maybe existing simulation at the end
    deleteSim();
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
    while(_simulationInProgress);
    delete _net;
    _net = 0;
    delete _craw;
    _craw = 0;
}


GUINet &
GUIRunThread::getNet() const
{
    return *_net;
}


void
GUIRunThread::setSimulationDelay(int value)
{
    _sleepPeriod = value;
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

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUIRunThread.icc"
//#endif

// Local Variables:
// mode:C++
// End:




