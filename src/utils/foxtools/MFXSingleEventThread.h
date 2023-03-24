/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2004-2023 German Aerospace Center (DLR) and others.
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
/// @file    MFXSingleEventThread.h
/// @author  unknown_author
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    2004-03-19
///
//
/****************************************************************************/

#pragma once
#include <config.h>

#include "fxheader.h"
#include <FXThread.h>
#include "fxexdefs.h"

class MFXInterThreadEventClient;

class MFXSingleEventThread : public FXObject, public FXThread {
    /// @brief FOX declaration
    FXDECLARE(MFXSingleEventThread)

public:
    enum {
        ID_THREAD_EVENT
    };

    /// @brief constructor
    MFXSingleEventThread(FXApp* a, MFXInterThreadEventClient* client);

    virtual ~MFXSingleEventThread();

    void signal();

    void signal(FXuint seltype);

    virtual FXint run() {
        return 0;
    }

    static void sleep(long ms);

    long onThreadSignal(FXObject*, FXSelector, void*);

    long onThreadEvent(FXObject*, FXSelector, void*);

protected:
    /// @brief FOX need this
    MFXSingleEventThread() { }

    /// @brief pointer to APP
    FXApp* myApp = nullptr;

    /// @brief thread client
    MFXInterThreadEventClient* myClient = nullptr;

private:
    /// @brief event
    FXEX::MFXThreadEventHandle event;

    /// @brief invalidate copy constructor
    MFXSingleEventThread(const MFXSingleEventThread&) = delete;

    /// @brief invalidate assignment constructor
    MFXSingleEventThread& operator=(const MFXSingleEventThread&) = delete;
};
