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
/// @file    GUIGlobalTestSystem.h
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

extern GUITestSystem* gTestSystem;

// ===========================================================================
// macro declarations
// ===========================================================================

// we use this macro for debug signals
// #define DEBUG_SIGNALS

#ifndef DEBUG_SIGNALS
    ///@brief reimplementation of FXIMPLEMENT used in sumo
    #define FXIMPLEMENT_SUMO(classname, baseclassname, mapping, nmappings) \
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
            if (gTestSystem) { \
                gTestSystem->nextTest(sender, sel); \
            } \
            return result;\
        }

    ///@brief reimplementation of FXIMPLEMENT_ABSTRACT used in sumo
    #define FXIMPLEMENT_SUMO_ABSTRACT(classname, baseclassname, mapping, nmappings) \
        const FX::FXMetaClass classname::metaClass(#classname, NULL, &baseclassname::metaClass, mapping, nmappings, sizeof(classname::FXMapEntry)); \
        long classname::handle(FX::FXObject* sender, FX::FXSelector sel, void* ptr) { \
            const FXMapEntry* me = (const FXMapEntry*)metaClass.search(sel); \
            int result; \
            if (me) { \
                result = (this->*me->func)(sender, sel, ptr); \
            } else { \
                result = baseclassname::handle(sender, sel, ptr); \
            } \
            if (gTestSystem) { \
                gTestSystem->nextTest(sender, sel); \
            } \
            return result;\
        }

    ///@brief reimplementation of FXIMPLEMENT used in netedit
    #define FXIMPLEMENT_NETEDIT(classname, baseclassname, mapping, nmappings) \
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
            if (gTestSystem) { \
                gTestSystem->nextTest(sender, sel); \
            } \
            return result;\
        }

    // @brief reimplementation of FXIMPLEMENT_ABSTRACT used in netedit
    #define FXIMPLEMENT_NETEDIT_ABSTRACT(classname, baseclassname, mapping, nmappings) \
        const FX::FXMetaClass classname::metaClass(#classname, NULL, &baseclassname::metaClass, mapping, nmappings, sizeof(classname::FXMapEntry)); \
        long classname::handle(FX::FXObject* sender, FX::FXSelector sel, void* ptr) { \
            const FXMapEntry* me = (const FXMapEntry*)metaClass.search(sel); \
            int result; \
            if (me) { \
                result = (this->*me->func)(sender, sel, ptr); \
            } else { \
                result = baseclassname::handle(sender, sel, ptr); \
            } \
            if (gTestSystem) { \
                gTestSystem->nextTest(sender, sel); \
            } \
            return result;\
        }
#else
    ///@brief reimplementation of FXIMPLEMENT used in sumo that write signal info
    #define FXIMPLEMENT_SUMO(classname, baseclassname, mapping, nmappings) \
        FX::FXObject* classname::manufacture(){  \
            return new classname;  \
        } \
        const FX::FXMetaClass classname::metaClass(#classname, classname::manufacture, &baseclassname::metaClass, mapping, nmappings, sizeof(classname::FXMapEntry)); \
        long classname::handle(FX::FXObject* sender,FX::FXSelector sel,void* ptr) { \
            if (gTestSystem) { \
                gTestSystem->writeSignalInfo(sender, sel); \
            } \
            const FXMapEntry* me = (const FXMapEntry*)metaClass.search(sel); \
            int result; \
            if (me) { \
                result = (this->*me->func)(sender, sel, ptr); \
            } else { \
                result = baseclassname::handle(sender, sel, ptr); \
            } \
            if (gTestSystem) { \
                gTestSystem->nextTest(sender, sel); \
            } \
            return result;\
        }

    ///@brief reimplementation of FXIMPLEMENT_ABSTRACT used in sumo that write signal info
    #define FXIMPLEMENT_SUMO_ABSTRACT(classname, baseclassname, mapping, nmappings) \
        const FX::FXMetaClass classname::metaClass(#classname, NULL, &baseclassname::metaClass, mapping, nmappings, sizeof(classname::FXMapEntry)); \
        long classname::handle(FX::FXObject* sender, FX::FXSelector sel, void* ptr) { \
            if (gTestSystem) { \
                gTestSystem->writeSignalInfo(sender, sel); \
            } \
            const FXMapEntry* me = (const FXMapEntry*)metaClass.search(sel); \
            int result; \
            if (me) { \
                result = (this->*me->func)(sender, sel, ptr); \
            } else { \
                result = baseclassname::handle(sender, sel, ptr); \
            } \
            if (gTestSystem) { \
                gTestSystem->nextTest(sender, sel); \
            } \
            return result;\
        }

    ///@brief reimplementation of FXIMPLEMENT used in netedit that write signal info
    #define FXIMPLEMENT_NETEDIT(classname, baseclassname, mapping, nmappings) \
        FX::FXObject* classname::manufacture(){  \
            return new classname;  \
        } \
        const FX::FXMetaClass classname::metaClass(#classname, classname::manufacture, &baseclassname::metaClass, mapping, nmappings, sizeof(classname::FXMapEntry)); \
        long classname::handle(FX::FXObject* sender,FX::FXSelector sel,void* ptr) { \
            if (gTestSystem) { \
                gTestSystem->writeSignalInfo(sender, sel); \
            } \
            const FXMapEntry* me = (const FXMapEntry*)metaClass.search(sel); \
            int result; \
            if (me) { \
                result = (this->*me->func)(sender, sel, ptr); \
            } else { \
                result = baseclassname::handle(sender, sel, ptr); \
            } \
            if (gTestSystem) { \
                gTestSystem->nextTest(sender, sel); \
            } \
            return result;\
        }

    ///@brief reimplementation of FXIMPLEMENT_ABSTRACT used in netedit that write signal info
    #define FXIMPLEMENT_NETEDIT_ABSTRACT(classname, baseclassname, mapping, nmappings) \
        const FX::FXMetaClass classname::metaClass(#classname, NULL, &baseclassname::metaClass, mapping, nmappings, sizeof(classname::FXMapEntry)); \
        long classname::handle(FX::FXObject* sender, FX::FXSelector sel, void* ptr) { \
            if (gTestSystem) { \
                gTestSystem->writeSignalInfo(sender, sel); \
            } \
            const FXMapEntry* me = (const FXMapEntry*)metaClass.search(sel); \
            int result; \
            if (me) { \
                result = (this->*me->func)(sender, sel, ptr); \
            } else { \
                result = baseclassname::handle(sender, sel, ptr); \
            } \
            if (gTestSystem) { \
                gTestSystem->nextTest(sender, sel); \
            } \
            return result;\
        }
#endif
