#ifndef QSUMOEvent_h
#define QSUMOEvent_h
//---------------------------------------------------------------------------//
//                        QSUMOEvent.h -
//  Definition of an own event class
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
// Revision 1.3  2003/06/18 11:04:53  dkrajzew
// new error processing adapted
//
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

#include <qevent.h>
#include "GUIEvents.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * QSUMOEvent
 * As it is not possible to pass user events directly into QEvent, this
 * class wraps it.
 */
class QSUMOEvent : public QEvent {
public:
    /// returns the event type
    GUIEvent getOwnType() const { return myType; }

protected:
    /// constructor
    QSUMOEvent(GUIEvent ownType)
        : QEvent(QEvent::User), myType(ownType) { }

    /// destructor
    virtual ~QSUMOEvent() { }

protected:
    /// the type of the event
    GUIEvent myType;

};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "QSUMOEvent.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

