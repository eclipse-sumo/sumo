/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
/// @file    MEPartition.h
/// @author  Josh
/// @date    2026-11-26
///
// Load edge groups for mesoscopic simulation
/****************************************************************************/
#pragma once

#include <map>
#include <string>

class MSEdge;

namespace MEPartition {

using EdgeGroups = std::map<const MSEdge*, std::string>;

/// Load edge memberships after the network has been loaded.
/// Each value is the TAZ id. Edges absent from the file have no entry.
EdgeGroups load(const std::string& file);

}
