/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUIDialog_AboutSUMO.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Fri, 29.04.2005
/// @version $Id$
///
// The application's "About" - dialog
/****************************************************************************/
#ifndef GUIDialog_AboutSUMO_h
#define GUIDialog_AboutSUMO_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <fx.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIDialog_AboutSUMO
 * @brief The application's "About" - dialog
 */
class GUIDialog_AboutSUMO : public FXDialogBox {
public:
    /** @brief Constructor
     * @param[in] parent The parent window
     */
    GUIDialog_AboutSUMO(FXWindow* parent);

    /// @brief Destructor
    ~GUIDialog_AboutSUMO();

    /// @brief Creates the widget
    void create();

private:
    /// @brief Font for the widget
    FXFont* myHeadlineFont;
};


#endif

/****************************************************************************/

