/****************************************************************************/
/// @file    NBPtStop.cpp
/// @author  Gregor Laemmel
/// @date    Tue, 20 Mar 2017
/// @version $Id: NBPtStop.cpp 23150 2017-02-27 12:08:30Z behrisch $
///
// The representation of a single pt stop
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#include "NBPTStop.h"
NBPTStop::NBPTStop(std::string ptStopId, Position position, std::string edgeId, double length):
myPTStopId(ptStopId),
myPosition(position),
myEdgeId(edgeId),
myPTStopLength(length)
{

}

std::string
NBPTStop::getID() const
{
    return myPTStopId;
}
