/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    GUIEvent_CloseView.h
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
 * GUIEvent_CloseView
 * Throw from GUIRunThread to GUIApplicationWindow and then further to all
 * displays after a step has been performed
 */
class GUIEvent_CloseView : public GUIEvent {
public:
    /// constructor
    GUIEvent_CloseView(const std::string& caption)
        : GUIEvent(GUIEventType::CLOSE_VIEW), myCaption(caption) {
    }

    /// destructor
    ~GUIEvent_CloseView() { }

    const std::string& getCaption() const {
        return myCaption;
    }

protected:

    /// The message
    std::string myCaption;
};
