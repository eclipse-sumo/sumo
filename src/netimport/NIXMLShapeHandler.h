/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NIXMLShapeHandler.h
/// @author  Jakob Erdmann
/// @date    Sat, 28 Jul 2018
/// @version $Id$
///
// Importer for static public transport information
/****************************************************************************/
#ifndef NIXMLShapeHandler_h
#define NIXMLShapeHandler_h

#include <config.h>

#include <string>
#include <utils/shapes/ShapeHandler.h>
// ===========================================================================
// class declarations
// ===========================================================================
class NBEdgeCont;
class Position;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class NBShapeHandler
 * @brief The XML-Handler for shapes loading network loading
 *
 * This subclasses ShapeHandler with MSLane specific function
 */
class NIXMLShapeHandler : public ShapeHandler {
public:
    NIXMLShapeHandler(ShapeContainer& sc, const NBEdgeCont& ec);

    /// @brief Destructor
    virtual ~NIXMLShapeHandler() {}

    Position getLanePos(const std::string& poiID, const std::string& laneID, double lanePos, double lanePosLat);

    virtual bool addLanePosParams() {
        return true;
    }

    const NBEdgeCont& myEdgeCont;
};
#endif

/****************************************************************************/
