#ifndef QMessageEvent_h
#define QMessageEvent_h
//---------------------------------------------------------------------------//
//                        QMessageEvent.h -
//  Event send when a message (message, warning, error) has to besubmitted
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Wed 18 Jun 2003
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
// Revision 1.1  2003/06/18 11:04:53  dkrajzew
// new error processing adapted
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
#include <string>
#include <utils/common/MsgHandler.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * QMessageEvent
 * Throw from GUIRunThread to GUIApplicationWindow and then further to all
 * displays after a step has been performed
 */
class QMessageEvent : public QSUMOEvent {
public:
    /// constructor
    QMessageEvent(MsgHandler::MsgType type, const std::string &msg)
        : QSUMOEvent(EVENT_MESSAGE_OCCURED), myMsg(msg)
    {
        switch(type) {
        case MsgHandler::MT_MESSAGE:
            myType = EVENT_MESSAGE_OCCURED;
            break;
        case MsgHandler::MT_WARNING:
            myType = EVENT_WARNING_OCCURED;
            break;
        case MsgHandler::MT_ERROR:
            myType = EVENT_ERROR_OCCURED;
            break;
        default:
            throw 1;
        }
    }

    /// destructor
    ~QMessageEvent() { }

    /// Returns the message
    const std::string &getMsg() const {
        return myMsg;
    }

protected:
    /// The message
    std::string myMsg;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "QMessageEvent.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

