/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2002-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUIEvent_Screenshot.h
/// @author  Michael Behrisch
/// @date    2015-08-25
/// @version $Id$
///
// Event send when a screenshot should be made
/****************************************************************************/
#ifndef GUIEvent_Screenshot_h
#define GUIEvent_Screenshot_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <iostream>
#include <utils/gui/events/GUIEvent.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUISUMOAbstractView;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class  GUIEvent_Screenshot
 *
 * Throw to GUIApplicationWindow from GUIRunThread to trigger a screenshot
 */
class GUIEvent_Screenshot : public GUIEvent {
public:
    /// constructor
    GUIEvent_Screenshot(GUISUMOAbstractView* view,
                        const std::string& file)
        : GUIEvent(EVENT_SCREENSHOT),
          myView(view), myFile(file) {
    }

    /// destructor
    ~GUIEvent_Screenshot() { }

public:
    /// @brief the view to save
    GUISUMOAbstractView* const myView;

    /// @brief the name of the file to save to
    const std::string myFile;

private:
    /// @brief Invalidated assignment operator
    GUIEvent_Screenshot& operator=(const GUIEvent_Screenshot& s);
};


#endif

/****************************************************************************/

