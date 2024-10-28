/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2004-2024 German Aerospace Center (DLR) and others.
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
/// @file    MFXThreadEvent.h
/// @author  Mathew Robertson
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    2004-03-19
///
//
/****************************************************************************/
#pragma once
#include <config.h>

#include "fxexdefs.h"
#include "MFXBaseObject.h"

namespace FXEX {

/**
 * :Description
 *
 *  Interthread communication object
 *
 *------------------------------------------------------------------
 *
 * Usage:
 *
 *  GUI_thread.h:
 *  ============
 *
 *  class MyGUI::FXWhatEver
 *  {
 *    // constructor
 *    MyGUI(...);
 *
 *    // message IDs
 *    enum {
 *    ID_THREAD_EVENT = FXWhatEver::ID_LAST,
 *    ID_LAST };
 *
 *    // message handler
 *    long onThreadEvent(FXObject*, FXSelector, void*);
 *
 *    // thread event object
 *   MFXThreadEvent m_threadEvent;
 *  };
 *
 *  GUI_thread.cpp:
 *  ==============
 *
 *  // message map
 *  FXDEFMAP(MyGUI, FXWhatEver) = {
 *    FXMAPFUNC(SEL_THREAD_EVENT, MyGUI::ID_THREAD_EVENT, MyGUI::onThreadEvent)
 *  };
 *
 *  // constructor
 *  MyGUI::MyGUI(...)
 *  {
 *    m_threadEvent.setTarget(this),
 *    m_threadEvent.setSelector(ID_THREAD_EVENT);
 *  }
 *
 *  // message handler
 *  long onThreadEvent(FXObject*, FXSelector, void*)
 *  {
 *    do something with the GUI
 *  }
 *
 *  Worker_thread.cpp:
 *  =================
 *
 *  int threadFunction(...)
 *  {
 *    MFXThreadEvent* pThreadEvent = (MFXThreadEvent*)(ptr);
 *
 *    while (not_finished) {
 *      // work hard
 *      ...
 *
 *      // wake up GUI
 *      if (something_happened_and_the_GUI_needs_to_know_it) {
 *        pThreadEvent.signal();
 *      }
 *    }
 *
 *    ...
 *  }
 *
 */
class /*FXAPI */MFXThreadEvent : public MFXBaseObject {
    FXDECLARE(MFXThreadEvent)

private:
    MFXThreadEventHandle event;

protected:
    MFXThreadEvent(const MFXThreadEvent&);
    MFXThreadEvent& operator=(const MFXThreadEvent&);

public:
    enum {
        ID_THREAD_EVENT = MFXBaseObject::ID_LAST,
        ID_LAST
    };

public:
    long onThreadSignal(FXObject*, FXSelector, void*);
    long onThreadEvent(FXObject*, FXSelector, void*);

public:
    /// Construct an object capable of signaling the main FOX event loop
    MFXThreadEvent(FXObject* tgt = NULL, FXSelector sel = 0);

    /**
     * Signal the event - using the SEL_THREAD FXSelector type
     *
     * This is meant to be called from the worker thread - it sends a mesage to
     * the target, which is in another thread.
     */
    void signal();

    /**
     * Signal the event - using the specified FXSelector
     *
     * This is meant to be called from the worker thread - it sends a mesage to
     * the target, which is in another thread.
     */
    void signal(FXuint seltype);

    /// destructor
    virtual ~MFXThreadEvent();
};

} // namespace FXEX
