/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GUIEvent_Message.h
/// @author  Daniel Krajzewicz
/// @date    Wed 18 Jun 2003
///
// Event send when a message (message, warning, error) has to besubmitted
/****************************************************************************/
#pragma once
#include <config.h>

#include "GUIEvent.h"
#include <string>
#include <utils/common/MsgHandler.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * GUIEvent_Message
 * Throw from GUIRunThread to GUIApplicationWindow and then further to all
 * displays after a step has been performed
 */
class GUIEvent_Message : public GUIEvent {
public:
    /// constructor
    GUIEvent_Message(const std::string& msg)
        : GUIEvent(GUIEventType::STATUS_OCCURRED), myMsg(msg) {
    }

    GUIEvent_Message(GUIEventType type, const std::string& msg):
        GUIEvent(type), myMsg(msg) {}

    /// constructor
    GUIEvent_Message(MsgHandler::MsgType type, const std::string& msg)
        : GUIEvent(GUIEventType::MESSAGE_OCCURRED), myMsg(msg) {
        switch (type) {
            case MsgHandler::MsgType::MT_MESSAGE:
                myType = GUIEventType::MESSAGE_OCCURRED;
                break;
            case MsgHandler::MsgType::MT_WARNING:
                myType = GUIEventType::WARNING_OCCURRED;
                break;
            case MsgHandler::MsgType::MT_ERROR:
                myType = GUIEventType::ERROR_OCCURRED;
                break;
            case MsgHandler::MsgType::MT_DEBUG:
                myType = GUIEventType::DEBUG_OCCURRED;
                break;
            case MsgHandler::MsgType::MT_GLDEBUG:
                myType = GUIEventType::GLDEBUG_OCCURRED;
                break;
            default:
                throw 1;
        }
    }

    /// destructor
    ~GUIEvent_Message() { }

    /// Returns the message
    const std::string& getMsg() const {
        return myMsg;
    }

protected:

    /// The message
    std::string myMsg;

};
