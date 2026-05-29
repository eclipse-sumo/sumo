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
/// @file    Batch.h
/// @date    2026
///
// libsumo extension that performs batched in-engine extraction of per-step
// vehicle / agent / edge / traffic-light data into packed little-endian byte
// columns (one std::string per typed-array field).  Two consumers:
//
//   1. The optional eCAL/Protobuf publisher (HAVE_ECAL build) that streams the
//      same data to the sumo-webgui browser frontend.  See publishSimStep.
//   2. Embedded C++ clients (e.g. the Qt6 GUI in sumo-webgui/qt_cpp) that read
//      the Batch::buffers() directly and avoid the SWIG / per-id round-trip
//      cost on large scenarios.
//
// The per-section fillVehicles / fillAgents / fillEdges / fillTLS methods are
// always compiled; they do not depend on eCAL or Protobuf.  Only the
// publishSimStep / init / close / getStats / available group is gated on
// HAVE_ECAL — those methods are stubs in builds without eCAL.
/****************************************************************************/
#pragma once

#include <cstdint>
#include <string>
#include <vector>


// ===========================================================================
// class definitions
// ===========================================================================
namespace libsumo {

#ifndef SWIG
// The BatchBuffers struct and the per-section fill* / beginStep / drainNewTypes
// / buffers() API are for embedded C++ consumers (e.g. the Qt6 GUI in
// sumo-webgui/qt_cpp).  SWIG cannot wrap the std::string columns as Python
// properties and would fail at module-init, so we hide this section from it.
// Python clients access libsumo::Batch only via the eCAL publisher entry
// points (available / init / close / publishSimStep / getStats) below.

/// Packed little-endian byte columns produced by Batch::fill*.  All bytes
/// fields use the same wire layout as the sumo.SimStepBin protobuf message
/// consumed by the deck.gl frontend, so embedded C++ clients can either parse
/// them directly (cheap reinterpret_cast for the typed-array fields) or hand
/// them to a SimStepBin via std::string::swap (cost: one swap per field).
///
/// All std::strings in this struct are cleared (but their heap capacity is
/// retained) at the start of each Batch::beginStep call, so after warmup the
/// per-step fill cycle performs no heap allocation in steady state.
struct BatchBuffers {
    // ---- vehicle section (filled by fillVehicles) ----
    std::string veh_positions;       // f64[3*N]    x,y,z
    std::string veh_angles;          // f32[N]      navi-degrees (clockwise from north)
    std::string veh_speeds;          // f32[N]      m/s
    std::string veh_ids;             // utf8[N]     null-terminated
    std::string veh_type_indices;    // u32[N]      index into the type registry
    std::string veh_attr_vals;       // f32[Kv*N]   per-attribute column, attr-major
    uint32_t    veh_count = 0;
    uint32_t    veh_attr_count = 0;

    // ---- agent section (persons + containers, filled by fillAgents) ----
    std::string agent_positions;     // f64[3*M]
    std::string agent_angles;        // f32[M]
    std::string agent_ids;           // utf8[M]
    std::string agent_type_indices;  // u32[M]
    uint32_t    agent_count = 0;

    // ---- edge section (filled by fillEdges) ----
    std::string edge_indices;        // u32[E]      external-edge index per included edge
    std::string edge_attr_vals;      // f32[Ke*E]   per-attribute column, attr-major
    uint32_t    edge_count = 0;
    uint32_t    edge_attr_count = 0;

    // ---- traffic-light section (filled by fillTLS) ----
    std::string tls_ids;             // utf8[T]
    std::string tls_states;          // utf8[T]     RYG strings
    uint32_t    tls_count = 0;
};
#endif // !SWIG

class Batch {
public:
#ifndef SWIG
    /// Reset per-step buffers (preserves heap capacity).  Call once before the
    /// fill* methods.  Idempotent; safe to call again to discard a partial
    /// fill cycle.
    static void beginStep();

    /// Iterate over visible vehicles, append their position / angle / speed /
    /// id / type / per-attribute columns to the buffers and populate the
    /// internal active_edges set used by fillEdges in non-full mode.
    /// Returns the number of vehicles emitted.
    static uint32_t fillVehicles(const std::vector<std::string>& attrs,
                                 bool geoReferenced);

    /// Iterate over visible persons and containers, append their state to the
    /// agent_* buffers.  Returns the number of agents emitted.
    static uint32_t fillAgents(bool geoReferenced);

    /// Iterate over edges, append index + per-attribute columns to edge_*.
    /// When @p fullSnapshot is false, only edges currently carrying a visible
    /// vehicle (as populated by fillVehicles) are emitted.  Returns the number
    /// of edges emitted.
    static uint32_t fillEdges(const std::vector<std::string>& attrs,
                              bool fullSnapshot);

    /// Iterate over active TLS logics, append id + current RYG state to
    /// tls_ids / tls_states.  Returns the number of TLS emitted.
    static uint32_t fillTLS();

    /// True iff new vehicle/agent types were registered since the last drain.
    /// Callers that publish type dictionaries should check this before each
    /// send so a freshly-seen type is delivered before the simstep that
    /// references it.  Always clears the dirty flag (so a subsequent call
    /// without intervening fill* invocations returns false).
    static bool drainNewTypes();

    /// Read-only access to the per-step buffers (vehicles / agents / edges /
    /// TLS).  Pointers/contents remain valid until the next beginStep call.
    static const BatchBuffers& buffers();

    /// Number of distinct vehicle / agent types registered so far.
    static uint32_t typeCount();

    /// Vehicle/agent type id for @p idx (as observed by fillVehicles /
    /// fillAgents).  Returns an empty string for out-of-range indices.
    /// Used by embedded C++ clients to map BatchBuffers::veh_type_indices /
    /// agent_type_indices back to a libsumo::VehicleType::getColor(id) call.
    static std::string typeId(uint32_t idx);
#endif // !SWIG

    // ---- optional eCAL publisher (HAVE_ECAL build only) -----------------
    // These methods are always declared so callers compile against the same
    // header regardless of build configuration.  In a build without eCAL,
    // available() returns false and the other methods are no-ops.

    /// True iff this build of libsumo includes eCAL/Protobuf support.
    static bool available();

    /// Create the eCAL publishers for the SimStepBin and VehicleTypeDict
    /// topics.  Caller must have already initialised eCAL in the host process.
    /// Safe to call multiple times; re-initialises the publishers.
    static void init(const std::string& simstepTopic,
                     const std::string& typedictTopic);

    /// Build a SimStepBin via beginStep + fillVehicles + fillAgents +
    /// fillEdges + fillTLS, then publish it on the simstep topic.  A
    /// VehicleTypeDict is republished first whenever new types were seen.
    /// Returns the number of visible vehicles (saves the caller a separate
    /// getIDCount() round-trip).
    static unsigned int publishSimStep(
        const std::vector<std::string>& vehAttrs,
        const std::vector<std::string>& edgeAttrs,
        bool fullEdgeSnapshot,
        int seq);

    /// Release publishers and reset internal type-registry state.
    static void close();

    /// Accumulated phase timings (microseconds) since the last reset:
    /// calls, veh_us, agent_us, edge_us, tls_us, typedict_us, serialize_us,
    /// send_us.  When @p reset is true, all counters are zeroed.
    static std::string getStats(bool reset);
};

} // namespace libsumo
