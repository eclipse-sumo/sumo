#ifndef QSimulationLoadedEvent_h
#define QSimulationLoadedEvent_h
//---------------------------------------------------------------------------//
//                        QSimulationLoadedEvent.h -
//  Event send when the simulation has been loaded by GUILadThread
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
// Revision 1.3  2003/02/07 10:34:15  dkrajzew
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
#include <qevent.h>
#include <iostream>
#include <microsim/MSNet.h>
#include "QSUMOEvent.h"
#include "GUIEvents.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUINet;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * QSimulationLoadedEvent
 * Throw to GUIApplicationWindow from GUILoadThread after a simulation has
 * been loaded or the loading process failed
 */
class QSimulationLoadedEvent : public QSUMOEvent {
public:
    /// the loaded net
    GUINet          *_net;

    /// the build raw-output stream
    std::ostream    *_craw;

    /// the time the simulation shall start with
    MSNet::Time     _begin;

    /// the time the simulation shall end with
    MSNet::Time     _end;

    /// the name of the loaded file
    std::string     _file;

public:
    /// constructor
    QSimulationLoadedEvent(GUINet *net, std::ostream *craw,
        MSNet::Time startTime, MSNet::Time endTime,
        const std::string &file)
        : QSUMOEvent(EVENT_SIMULATION_LOADED),
        _net(net), _craw(craw), _begin(startTime), _end(endTime),
        _file(file) { }

    /// destructor
    ~QSimulationLoadedEvent() { }
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "QSimulationLoadedEvent.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

