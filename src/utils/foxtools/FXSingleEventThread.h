/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2004-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    FXSingleEventThread.h
/// @author  unknown_author
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    2004-03-19
/// @version $Id$
///
//
/****************************************************************************/

#ifndef FXSingleEventThread_h
#define FXSingleEventThread_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <fx.h>
#include <FXThread.h>
#include "fxexdefs.h"

class MFXInterThreadEventClient;

class FXSingleEventThread : public FXObject, public FXThread {
    FXDECLARE(FXSingleEventThread)

private:
    FXEX::FXThreadEventHandle event;

private:
    FXSingleEventThread(const FXSingleEventThread&);
    FXSingleEventThread& operator=(const FXSingleEventThread&);

public:
    enum {
        ID_THREAD_EVENT
    };

public:
    long onThreadSignal(FXObject*, FXSelector, void*);
    long onThreadEvent(FXObject*, FXSelector, void*);

public:
    FXSingleEventThread(FXApp* a, MFXInterThreadEventClient* client);

    void signal();

    void signal(FXuint seltype);

    virtual FXint run() {
        return 0;
    }

    virtual ~FXSingleEventThread();

    static void sleep(long ms);

private:
    FXApp* myApp;
    MFXInterThreadEventClient* myClient;

protected:
    FXSingleEventThread() { }
};


#endif
