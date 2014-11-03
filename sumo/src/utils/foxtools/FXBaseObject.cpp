/****************************************************************************/
/// @file    FXBaseObject.cpp
/// @author  Mathew Robertson
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    2004-03-19
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2003-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fxver.h>
#include <xincs.h>
#include <fxdefs.h>
#include <fx.h>
/*
#include <FXString.h>
#include <FXHash.h>
#include <FXStream.h>
#include <FXSize.h>
#include <FXPoint.h>
#include <FXRectangle.h>
#include <FXRegistry.h>
#include <FXMutex.h>
#include <FXApp.h>
#include <FXWindow.h>
*/
using namespace FX;
#include "FXBaseObject.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS
using namespace FXEX;
namespace FXEX {

FXDEFMAP(FXBaseObject) FXBaseObjectMap[] = {
    FXMAPFUNC(SEL_COMMAND, FXWindow::ID_ENABLE, FXBaseObject::onCmdEnable),
    FXMAPFUNC(SEL_COMMAND, FXWindow::ID_DISABLE, FXBaseObject::onCmdDisable),
    FXMAPFUNC(SEL_UPDATE, FXWindow::ID_DISABLE, FXBaseObject::onUpdate),
};
FXIMPLEMENT(FXBaseObject, FXObject, FXBaseObjectMap, ARRAYNUMBER(FXBaseObjectMap))

// ctor
FXBaseObject::FXBaseObject(FXObject* tgt, FXSelector sel) : FXObject() {
    data = NULL;
    target = tgt;
    message = sel;
    flags = 0;
    app = FXApp::instance();
    if (app == NULL) {
        fxerror("%s: Cannot create object without FXApp object\n", getClassName());
    }
}

// ctor
FXBaseObject::FXBaseObject(FXApp* a, FXObject* tgt, FXSelector sel) : FXObject() {
    data = NULL;
    target = tgt;
    message = sel;
    flags = 0;
    app = a;
    if (app == NULL) {
        app = FXApp::instance();
    }
    if (app == NULL) {
        fxerror("%s: Cannot create object without FXApp object\n", getClassName());
    }
}

// free up all resources
FXBaseObject::~FXBaseObject() {
    if (data != NULL && data != (void*) - 1) {
        fxerror("%s::~%s - user data is not NULL prior to destruction\n", getClassName(), getClassName());
    }
    app = (FXApp*) - 1;
    target = (FXObject*) - 1;
}

// save object to stream
void FXBaseObject::save(FXStream& store) const {
    FXObject::save(store);
    store << app;
    store << target;
    store << message;
    store << flags;
    store << options;
    store << datalen;
    store.save((FXuchar*)data, (unsigned long)datalen);
}

// load object from stream
void FXBaseObject::load(FXStream& store) {
    FXObject::load(store);
    store >> app;
    store >> target;
    store >> message;
    store >> flags;
    store >> options;
    store >> datalen;
    store.load((FXuchar*)data, (unsigned long)datalen);
}

// this allows FXBaseObject derived classes to be singletons
FXApp* FXBaseObject::getApp() {
    if (app) {
        return app;
    }
    return FXApp::instance();
}

// set the readonly flag
void FXBaseObject::setReadonly(FXbool mode) {
    if (mode) {
        flags |= FLAG_READONLY;
    } else {
        flags &= ~FLAG_READONLY;
    }
}

// handle enable event
long FXBaseObject::onCmdEnable(FXObject*, FXSelector, void*) {
    enable();
    return 1;
}

// handle disable event
long FXBaseObject::onCmdDisable(FXObject*, FXSelector, void*) {
    disable();
    return 1;
}

// handle update event
long FXBaseObject::onUpdate(FXObject* sender, FXSelector, void*) {
    if (flags & FLAG_ENABLED) {
        sender->handle(this, FXSEL(SEL_UPDATE, FXWindow::ID_ENABLE), NULL);
    } else {
        sender->handle(this, FXSEL(SEL_UPDATE, FXWindow::ID_DISABLE), NULL);
    }
    return 1;
}

}

