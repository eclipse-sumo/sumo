/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDialog_About.h
/// @author  Jakob Erdmann
/// @date    Feb 2011
/// @version $Id$
///
// The "About" - dialog for NETEDIT, (adapted from GUIDialog_AboutSUMO)
/****************************************************************************/
#ifndef GNEDialog_About_h
#define GNEDialog_About_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <fx.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEDialog_About
 * @brief The application's "About" - dialog
 */
class GNEDialog_About : public FXDialogBox {
public:
    /**@brief Constructor
     * @param[in] parent The parent window
     */
    GNEDialog_About(FXWindow* parent);

    /// @brief Destructor
    ~GNEDialog_About();

    /// @brief Creates the widget
    void create();

private:
    /// @brief Font for the widget
    FXFont* myHeadlineFont;
};


#endif

/****************************************************************************/

