/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    GNERouteDistribution.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2022
///
// Route distribution used in netedit
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEDistribution.h"


// ===========================================================================
// class definitions
// ===========================================================================

class GNERouteDistribution : public GNEDistribution {

public:
    /// @brief default constructor
    GNERouteDistribution(GNENet* net);

    /// @brief parameter constructor
    GNERouteDistribution(GNENet* net, const std::string& ID);

    /// @brief destructor
    ~GNERouteDistribution();

    /**@brief write demand element element into a xml file
     * @param[in] device device in which write parameters of demand element element
     */
    void writeDemandElement(OutputDevice& device) const;

private:
    /// @brief Invalidated copy constructor.
    GNERouteDistribution(GNERouteDistribution*) = delete;

    /// @brief Invalidated assignment operator
    GNERouteDistribution* operator=(GNERouteDistribution*) = delete;
};
