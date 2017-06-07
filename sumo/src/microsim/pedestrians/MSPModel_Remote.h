/****************************************************************************/
/// @file    MSPModel_Remote.h
/// @author  Gregor Laemmel
/// @date    Mon, 13 Jan 2014
/// @version $Id: MSPModel_Remote.h 23150 2017-02-27 12:08:30Z behrisch $
///
// The pedestrian following model for remote controlled pedestrian movement
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2014-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/

#ifndef SUMO_MSPMODEL_REMOTE_H
#define SUMO_MSPMODEL_REMOTE_H


#include <utils/options/OptionsCont.h>
#include <microsim/MSNet.h>
#include "MSPModel.h"
class MSPModel_Remote : public MSPModel{


public:
    MSPModel_Remote(const OptionsCont& oc, MSNet* net);
};


#endif //SUMO_MSPMODEL_REMOTE_H
