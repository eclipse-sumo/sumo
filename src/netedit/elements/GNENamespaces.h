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
/// @file    GNENamespaces.h
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2023
///
// Class for unify namespaces in netedit
/****************************************************************************/
#pragma once
#include <config.h>


// ===========================================================================
// included modules
// ===========================================================================

#include <utils/xml/SUMOSAXAttributes.h>

// ===========================================================================
// class definitions
// ===========================================================================

class GNENamespaces {

public:
    /// @brief default constructor
    GNENamespaces();

    /// @brief destructor
    ~GNENamespaces();

    /// @brief busStops namespace
    const std::vector<SumoXMLTag> busStops;

    /// @brief lane area detectors namespace
    const std::vector<SumoXMLTag> laneAreaDetectors;

    /// @brief calibrators namespace
    const std::vector<SumoXMLTag> calibrators;

    /// @brief polygon namespace
    const std::vector<SumoXMLTag> polygons;

    /// @brief POIs namespace
    const std::vector<SumoXMLTag> POIs;

    /// @brief type namespace
    const std::vector<SumoXMLTag> types;

    /// @brief route namespace
    const std::vector<SumoXMLTag> routes;

    /// @brief vehicles namespace
    const std::vector<SumoXMLTag> vehicles;

    /// @brief persons namespace
    const std::vector<SumoXMLTag> persons;

    /// @brief containers namespace
    const std::vector<SumoXMLTag> containers;

    /// @brief vehicle stops namespace
    const std::vector<SumoXMLTag> stops;
};

/****************************************************************************/
