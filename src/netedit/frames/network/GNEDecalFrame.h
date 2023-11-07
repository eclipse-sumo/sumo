/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    GNEDecalFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2023
///
// The Widget for add and edit decals
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/frames/GNEFrame.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
* @class GNEDecalFrame
* The Widget for setting decals
*/
class GNEDecalFrame : public GNEFrame {

public:
    /**@brief Constructor
    * @brief viewParent GNEViewParent in which this GNEFrame is placed
    * @brief viewNet viewNet that uses this GNEFrame
    */
    GNEDecalFrame(GNEViewParent* viewParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNEDecalFrame();

protected:

private:

};
