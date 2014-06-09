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
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2004-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/

#ifndef FXSingleEventThread_h
#define FXSingleEventThread_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

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
