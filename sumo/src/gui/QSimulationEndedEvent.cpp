//---------------------------------------------------------------------------//
//                        QSimulationEndedEvent.cpp -
//  Event send when the the simulation is over
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Thu, 19 Jun 2003
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
// Revision 1.1  2003/06/19 10:56:03  dkrajzew
// user information about simulation ending added; the gui may shutdown on end and be started with a simulation now;
//
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "QSUMOEvent.h"
#include "GUIEvents.h"
#include "QSimulationEndedEvent.h"


/* =========================================================================
 * method definitions
 * ======================================================================= */
QSimulationEndedEvent::QSimulationEndedEvent(EndReason reason, size_t step)
    : QSUMOEvent(EVENT_SIMULATION_ENDED), myReason(reason), myStep(step)
{
}


QSimulationEndedEvent::~QSimulationEndedEvent()
{
}


size_t
QSimulationEndedEvent::getTimeStep() const
{
    return myStep;
}


QSimulationEndedEvent::EndReason
QSimulationEndedEvent::getReason() const
{
    return myReason;
}

