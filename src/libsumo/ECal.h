/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2012-2026 German Aerospace Center (DLR) and others.
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
/// @file    ECal.h
/// @date    2026
///
// Optional libsumo extension: publishes per-step vehicle / person / edge
// snapshots as packed protobuf messages over eCAL.  Replaces the hot Python
// per-vehicle / per-edge extraction loop in the sumo-webgui publisher.
//
// The class is always compiled.  When the build does not have eCAL/Protobuf
// available (HAVE_ECAL is undefined), every method is a no-op that returns
// false from available() so callers can fall back to a Python implementation.
/****************************************************************************/
#pragma once
#include <config.h>

#include <string>
#include <vector>


// ===========================================================================
// class definitions
// ===========================================================================
namespace libsumo {

class ECal {
public:
    /// True iff this build of libsumo includes eCAL/Protobuf support.
    static bool available();

    /// Create eCAL publishers.  Must be called after the Python side already
    /// called eCAL initialise() (C++ inherits the same process-wide eCAL
    /// state via the shared libecal_core.so).  Safe to call multiple times;
    /// re-initialises publishers on each call.
    static void init(const std::string& simstepTopic,
                     const std::string& typedictTopic);

    /// Iterate over visible vehicles + persons + containers, pack into one
    /// SimStepBin protobuf message and publish it on the simstep topic.
    /// When @p fullEdgeSnapshot is true, the edge section contains every
    /// non-internal edge; otherwise only edges that currently carry at least
    /// one visible vehicle are included.
    /// A VehicleTypeDict is republished whenever a new vehicle/agent type is
    /// observed.
    static void publishSimStep(
        const std::vector<std::string>& vehAttrs,
        const std::vector<std::string>& edgeAttrs,
        bool fullEdgeSnapshot,
        int seq);

    /// Release publishers and reset internal type registry state.
    static void close();
};

} // namespace libsumo
