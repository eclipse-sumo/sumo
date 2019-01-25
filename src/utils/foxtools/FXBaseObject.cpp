/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2003-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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


/* =========================================================================
 * included modules
 * ======================================================================= */
#include <config.h>

#include <fxver.h>
#define NOMINMAX
#include <xincs.h>
#undef NOMINMAX
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
    data = nullptr;
    target = tgt;
    message = sel;
    flags = 0;
    app = FXApp::instance();
    if (app == nullptr) {
        fxerror("%s: Cannot create object without FXApp object\n", getClassName());
    }
}

// ctor
FXBaseObject::FXBaseObject(FXApp* a, FXObject* tgt, FXSelector sel) : FXObject() {
    data = nullptr;
    target = tgt;
    message = sel;
    flags = 0;
    app = a;
    if (app == nullptr) {
        app = FXApp::instance();
    }
    if (app == nullptr) {
        fxerror("%s: Cannot create object without FXApp object\n", getClassName());
    }
}

// free up all resources
FXBaseObject::~FXBaseObject() {
    if (data != nullptr && data != (void*) - 1) {
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
        sender->handle(this, FXSEL(SEL_UPDATE, FXWindow::ID_ENABLE), nullptr);
    } else {
        sender->handle(this, FXSEL(SEL_UPDATE, FXWindow::ID_DISABLE), nullptr);
    }
    return 1;
}

}

