/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    GUITestSystem.h
/// @author  Pablo Alvarez Lopez
/// @date    May 2025
///
// A global holder of test system
/****************************************************************************/
#pragma once
#include <config.h>

#include "GUITestSystem.h"

// ===========================================================================
// global variable declarations
// ===========================================================================

extern GUITestSystem gTestSystem;

// ===========================================================================
// macro declarations
// ===========================================================================

// we use this macro for check test signals
//#define TEST_SIGNALS

#ifndef TEST_SIGNALS
    #define FXIMPLEMENT_TESTING(classname, baseclassname, mapping, nmappings) \
        FX::FXObject* classname::manufacture(){  \
            return new classname;  \
        } \
        const FX::FXMetaClass classname::metaClass(#classname, classname::manufacture, &baseclassname::metaClass, mapping, nmappings, sizeof(classname::FXMapEntry)); \
        long classname::handle(FX::FXObject* sender,FX::FXSelector sel,void* ptr) { \
            const FXMapEntry* me = (const FXMapEntry*)metaClass.search(sel); \
            int result; \
            if (me) { \
                result = (this->*me->func)(sender, sel, ptr); \
            } else { \
                result = baseclassname::handle(sender, sel, ptr); \
            } \
            gTestSystem.nextTest(sender, sel); \
            return result;\
        }

    #define FXIMPLEMENT_TESTING_ABSTRACT(classname, baseclassname, mapping, nmappings) \
        const FX::FXMetaClass classname::metaClass(#classname, NULL, &baseclassname::metaClass, mapping, nmappings, sizeof(classname::FXMapEntry)); \
        long classname::handle(FX::FXObject* sender, FX::FXSelector sel, void* ptr) { \
        const FXMapEntry* me = (const FXMapEntry*)metaClass.search(sel); \
        int result; \
        if (me) { \
            result = (this->*me->func)(sender, sel, ptr); \
        } else { \
            result = baseclassname::handle(sender, sel, ptr); \
        } \
        return result;\
        }

#else
    #define FXIMPLEMENT_TESTING(classname, baseclassname, mapping, nmappings) \
        FX::FXObject* classname::manufacture(){  \
            return new classname;  \
        } \
        const FX::FXMetaClass classname::metaClass(#classname, classname::manufacture, &baseclassname::metaClass, mapping, nmappings, sizeof(classname::FXMapEntry)); \
        long classname::handle(FX::FXObject* sender,FX::FXSelector sel,void* ptr) { \
            gTestSystem.writeSignalInfo(sender, sel); \
            const FXMapEntry* me = (const FXMapEntry*)metaClass.search(sel); \
            int result; \
            if (me) { \
                result = (this->*me->func)(sender, sel, ptr); \
            } else { \
                result = baseclassname::handle(sender, sel, ptr); \
            } \
            gTestSystem.nextTest(sender, sel); \
            return result;\
        }

    #define FXIMPLEMENT_TESTING_ABSTRACT(classname, baseclassname, mapping, nmappings) \
        FX::FXObject* classname::manufacture(){  \
            return new classname;  \
        } \
        long classname::handle(FX::FXObject* sender, FX::FXSelector sel, void* ptr) { \
        const FXMapEntry* me=(const FXMapEntry*)metaClass.search(sel); \
        return me ? (this->* me->func)(sender,sel,ptr) : baseclassname::handle(sender,sel,ptr); \
        }
#endif
