/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDataHandler.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// Builds data objects for netedit
/****************************************************************************/
#ifndef GNEDataHandler_h
#define GNEDataHandler_h

// ===========================================================================
// included modules
// ===========================================================================

#include <config.h>

#include <utils/xml/SUMOSAXAttributes.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/SUMOXMLDefinitions.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GNEViewNet;
class GNEEdge;
class GNETAZ;
class GNEDataElement;
class GNEVehicle;
class GNEPerson;
class GNEUndoList;

// ===========================================================================
// class definitions
// ===========================================================================

/// @class GNEDataHandler
/// @brief Builds trigger objects for GNENet (busStops, chargingStations, detectors, etc..)
class GNEDataHandler : public SUMOSAXHandler {
public:

    /// @brief Constructor
    GNEDataHandler(const std::string& file, GNEViewNet* viewNet, bool undoDataElements = true);

    /// @brief Destructor
    ~GNEDataHandler();

protected:
    //

private:
    /// @brief pointer to View's Net
    GNEViewNet* myViewNet;

    /// @brief flag used for parsing values
    bool myAbort;

    /// @brief flag to check if created data elements must be undo and redo
    bool myUndoDataElements;
};


#endif
