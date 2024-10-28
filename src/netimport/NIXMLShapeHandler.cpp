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
/// @file    NIXMLShapeHandler.cpp
/// @author  Jakob Erdmann
/// @date    Sat, 28 Jul 2018
///
// Importer for static public transport information
/****************************************************************************/
#include <config.h>

#include <utils/geom/Position.h>
#include <utils/common/MsgHandler.h>
#include <utils/geom/GeoConvHelper.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBEdge.h>

#include "NIXMLShapeHandler.h"

// ===========================================================================
// method definitions
// ===========================================================================

NIXMLShapeHandler::NIXMLShapeHandler(ShapeContainer& sc, const NBEdgeCont& ec) :
    ShapeHandler("polgyon - file", sc, GeoConvHelper::getNumLoaded() == 0 ? nullptr : & GeoConvHelper::getLoaded()),
    myEdgeCont(ec)
{}

Position
NIXMLShapeHandler::getLanePos(const std::string& poiID, const std::string& laneID, double lanePos, bool friendlyPos, double lanePosLat) {
    std::string edgeID;
    int laneIndex;
    NBHelpers::interpretLaneID(laneID, edgeID, laneIndex);
    NBEdge* edge = myEdgeCont.retrieve(edgeID);
    if (edge == 0 || laneIndex < 0 || edge->getNumLanes() <= laneIndex) {
        WRITE_ERRORF(TL("Lane '%' to place poi '%' on is not known."), laneID, poiID);
        return Position::INVALID;
    }
    if (lanePos < 0) {
        lanePos = edge->getLength() + lanePos;
    }
    if ((lanePos < 0) && friendlyPos) {
        lanePos = 0;
    }
    if ((lanePos > edge->getLength()) && friendlyPos) {
        lanePos = edge->getLength();
    }
    if (lanePos < 0 || lanePos > edge->getLength()) {
        WRITE_WARNINGF(TL("lane position % for poi '%' is not valid."), toString(lanePos), poiID);
    }
    return edge->getLanes()[laneIndex].shape.positionAtOffset(lanePos, -lanePosLat);
}


/****************************************************************************/
