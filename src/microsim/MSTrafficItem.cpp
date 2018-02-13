/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSTransportable.cpp
/// @author  Melanie Weber
/// @author  Andreas Kendziorra
/// @author  Michael Behrisch
/// @date    Thu, 12 Jun 2014
/// @version $Id$
///
// The common superclass for modelling transportable objects like persons and containers
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <MSTrafficItem.h>

/* -------------------------------------------------------------------------
* static member definitions
* ----------------------------------------------------------------------- */
// hash function
std::hash<std::string> MSTrafficItem::hash = std::hash<std::string>();


// ===========================================================================
// method definitions
// ===========================================================================
MSTrafficItem::MSTrafficItem(MSTrafficItemType type, const std::string& id, std::shared_ptr<MSTrafficItemCharacteristics> data) :
            type(type),
            id_hash(hash(id)),
            data(data),
            remainingIntegrationTime(0.),
            integrationDemand(0.),
            latentDemand(0.)
{}
/****************************************************************************/
