/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2007-2024 German Aerospace Center (DLR) and others.
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
/// @file    MSRouterDefs.h
/// @author  Michael Behrisch
/// @date    2023-12-21
///
// Type definitions for routers in the microsim context
/****************************************************************************/
#pragma once
#include <config.h>

#include <memory>
#include <utils/router/SUMOAbstractRouter.h>
#include <utils/router/RouterProvider.h>


// ===========================================================================
// class declarations and typedefs
// ===========================================================================
class MSEdge;
class MSLane;
class MSJunction;
class MSRoute;
class SUMOVehicle;

typedef std::vector<MSEdge*> MSEdgeVector;
typedef std::shared_ptr<const MSRoute> ConstMSRoutePtr;
typedef RouterProvider<MSEdge, MSLane, MSJunction, SUMOVehicle> MSRouterProvider;
typedef SUMOAbstractRouter<MSEdge, SUMOVehicle> MSVehicleRouter;
typedef IntermodalRouter<MSEdge, MSLane, MSJunction, SUMOVehicle> MSTransportableRouter;
typedef PedestrianRouter<MSEdge, MSLane, MSJunction, SUMOVehicle> MSPedestrianRouter;
