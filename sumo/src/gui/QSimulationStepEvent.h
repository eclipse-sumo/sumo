#ifndef QSimulationStepEvent_h
#define QSimulationStepEvent_h
//---------------------------------------------------------------------------//
//                        QSimulationStepEvent.h -
//  Event send when the a simulation step has been performed by GUIRunThread
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
// Revision 1.2  2003/02/07 10:34:15  dkrajzew
// files updated
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


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * QSimulationStepEvent
 * Throw from GUIRunThread to GUIApplicationWindow and then further to all
 * displays after a step has been performed
 */
class QSimulationStepEvent : public QSUMOEvent {
public:
    /// constructor
    QSimulationStepEvent()
        : QSUMOEvent(EVENT_SIMULATION_STEP) { }

    /// destructor
    ~QSimulationStepEvent() { }
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "QSimulationStepEvent.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

