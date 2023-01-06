/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2003-2023 German Aerospace Center (DLR) and others.
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
/// @file    MFXBaseObject.cpp
/// @author  Mathew Robertson
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    2004-03-19
///
//
/****************************************************************************/


/* =========================================================================
 * included modules
 * ======================================================================= */
#include <config.h>

#define NOMINMAX
#undef NOMINMAX
#include "fxheader.h"
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
#include "MFXBaseObject.h"

using namespace FXEX;
namespace FXEX {

FXDEFMAP(MFXBaseObject) MFXBaseObjectMap[] = {
    FXMAPFUNC(SEL_COMMAND, FXWindow::ID_ENABLE, MFXBaseObject::onCmdEnable),
    FXMAPFUNC(SEL_COMMAND, FXWindow::ID_DISABLE, MFXBaseObject::onCmdDisable),
    FXMAPFUNC(SEL_UPDATE, FXWindow::ID_DISABLE, MFXBaseObject::onUpdate),
};
FXIMPLEMENT(MFXBaseObject, FXObject, MFXBaseObjectMap, ARRAYNUMBER(MFXBaseObjectMap))

// ctor
MFXBaseObject::MFXBaseObject(FXObject* tgt, FXSelector sel) : FXObject() {
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
MFXBaseObject::MFXBaseObject(FXApp* a, FXObject* tgt, FXSelector sel) : FXObject() {
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
MFXBaseObject::~MFXBaseObject() {
    if (data != nullptr && data != (void*) - 1) {
        fxerror("%s::~%s - user data is not NULL prior to destruction\n", getClassName(), getClassName());
    }
    app = (FXApp*) - 1;
    target = (FXObject*) - 1;
}

// save object to stream
void MFXBaseObject::save(FXStream& store) const {
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
void MFXBaseObject::load(FXStream& store) {
    FXObject::load(store);
    store >> app;
    store >> target;
    store >> message;
    store >> flags;
    store >> options;
    store >> datalen;
    store.load((FXuchar*)data, (unsigned long)datalen);
}

// this allows MFXBaseObject derived classes to be singletons
FXApp* MFXBaseObject::getApp() {
    if (app) {
        return app;
    }
    return FXApp::instance();
}

// set the readonly flag
void MFXBaseObject::setReadonly(FXbool mode) {
    if (mode) {
        flags |= FLAG_READONLY;
    } else {
        flags &= ~FLAG_READONLY;
    }
}

// handle enable event
long MFXBaseObject::onCmdEnable(FXObject*, FXSelector, void*) {
    enable();
    return 1;
}

// handle disable event
long MFXBaseObject::onCmdDisable(FXObject*, FXSelector, void*) {
    disable();
    return 1;
}

// handle update event
long MFXBaseObject::onUpdate(FXObject* sender, FXSelector, void*) {
    if (flags & FLAG_ENABLED) {
        sender->handle(this, FXSEL(SEL_UPDATE, FXWindow::ID_ENABLE), nullptr);
    } else {
        sender->handle(this, FXSEL(SEL_UPDATE, FXWindow::ID_DISABLE), nullptr);
    }
    return 1;
}

}

