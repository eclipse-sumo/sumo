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
/// @file    GNEAdditionalListed.h
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2025
///
// A abstract class for representation of additional listed elements
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/elements/GNEAttributeCarrier.h>
#include <netedit/elements/GNEContour.h>
#include <netedit/elements/GNEHierarchicalElement.h>
#include <netedit/elements/GNEPathElement.h>
#include <netedit/elements/moving/GNEMoveElement.h>
#include <utils/gui/div/GUIGeometry.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/images/GUITextureSubSys.h>

// ===========================================================================
// class definitions
// ===========================================================================

class GNEAdditionalListed {

public:
    /// @brief default constructor
    GNEAdditionalListed();

protected:


private:

    /// @brief Invalidated copy constructor.
    GNEAdditionalListed(const GNEAdditionalListed&) = delete;

    /// @brief Invalidated assignment operator.
    GNEAdditionalListed& operator=(const GNEAdditionalListed&) = delete;
};
