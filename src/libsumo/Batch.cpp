/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2012-2026 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    Batch.cpp
/// @date    2026
///
// Implementation — see Batch.h for the public contract.
//
// The file is split into two compilation regions:
//
//   * Always-compiled: per-section fillVehicles / fillAgents / fillEdges /
//     fillTLS, the type registry, and the shared helpers.  These depend only
//     on microsim internals and produce/consume BatchBuffers.
//   * HAVE_ECAL only: init / close / publishSimStep / getStats / available —
//     the thin wrapper that swaps BatchBuffers into a sumo::SimStepBin proto,
//     serialises it, and sends it over an eCAL publisher.
//
// Outside HAVE_ECAL the eCAL wrapper methods are stubs that return false /
// zero — the fill* methods remain fully functional for embedded C++ clients.
/****************************************************************************/
#include <config.h>

#include "Batch.h"

#include <cstdint>
#include <cstring>
#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <microsim/MSEdge.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSLane.h>
#include <microsim/MSNet.h>
#include <microsim/MSBaseVehicle.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSVehicleType.h>
#include <microsim/traffic_lights/MSTLLogicControl.h>
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include <microsim/traffic_lights/MSPhaseDefinition.h>
#include <microsim/transportables/MSTransportable.h>
#include <microsim/transportables/MSTransportableControl.h>
#include <microsim/transportables/MSStage.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/emissions/PollutantsInterface.h>
#include <utils/geom/GeomHelper.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/geom/Position.h>


namespace libsumo {

namespace {

// ---- typed-array append helpers ---------------------------------------------------------------
// Bytes fields hold the packed little-endian representation of typed numeric arrays.
// Linux x86_64 is little-endian, matching the wire layout consumed by the frontend.
template <typename T>
inline void appendLE(std::string& dst, T value) {
    dst.append(reinterpret_cast<const char*>(&value), sizeof(T));
}

inline void appendCString(std::string& dst, const std::string& s) {
    dst.append(s);
    dst.push_back('\0');
}

// ---- attribute getter dispatch (resolved once per (attribute-list, kind) key) -----------------
// Vehicle getters take MSBaseVehicle* so both micro (MSVehicle) and meso (MEVehicle)
// vehicles dispatch through the same path; all methods used here are either declared
// on SUMOTrafficObject/SUMOVehicle (virtual) or on MSBaseVehicle itself.
using VehAttrFn  = std::function<double(const MSBaseVehicle*)>;
using EdgeAttrFn = std::function<double(const MSEdge*)>;

VehAttrFn resolveVehAttr(const std::string& name) {
    if (name == "waiting_time")            return [](const MSBaseVehicle* v) { return STEPS2TIME(v->getWaitingTime()); };
    if (name == "accumulated_waiting_time")return [](const MSBaseVehicle* v) {
        if (auto* mv = dynamic_cast<const MSVehicle*>(v)) return mv->getAccumulatedWaitingSeconds();
        return STEPS2TIME(v->getWaitingTime());
    };
    if (name == "co2_emission")            return [](const MSBaseVehicle* v) { return v->getEmissions<PollutantsInterface::CO2>(); };
    if (name == "co_emission")             return [](const MSBaseVehicle* v) { return v->getEmissions<PollutantsInterface::CO>(); };
    if (name == "hc_emission")             return [](const MSBaseVehicle* v) { return v->getEmissions<PollutantsInterface::HC>(); };
    if (name == "nox_emission")            return [](const MSBaseVehicle* v) { return v->getEmissions<PollutantsInterface::NO_X>(); };
    if (name == "pmx_emission")            return [](const MSBaseVehicle* v) { return v->getEmissions<PollutantsInterface::PM_X>(); };
    if (name == "fuel_consumption")        return [](const MSBaseVehicle* v) { return v->getEmissions<PollutantsInterface::FUEL>(); };
    if (name == "electricity_consumption") return [](const MSBaseVehicle* v) { return v->getEmissions<PollutantsInterface::ELEC>(); };
    if (name == "noise_emission")          return [](const MSBaseVehicle* v) { return v->getHarmonoise_NoiseEmissions(); };
    return [](const MSBaseVehicle*) { return 0.0; };
}

EdgeAttrFn resolveEdgeAttr(const std::string& name) {
    auto sumLanes = [](const MSEdge* e, auto&& f) {
        double sum = 0.0;
        for (MSLane* lane : e->getLanes()) sum += f(lane);
        return sum;
    };
    if (name == "speed")              return [](const MSEdge* e) { return e->getMeanSpeed(); };
    if (name == "occupancy")          return [](const MSEdge* e) { return e->getOccupancy(); };
    if (name == "vehicle_count")      return [](const MSEdge* e) { return (double)e->getVehicleNumber(); };
    if (name == "waiting_time")       return [](const MSEdge* e) { return e->getWaitingSeconds(); };
    if (name == "travel_time")        return [](const MSEdge* e) { return e->getCurrentTravelTime(); };
    if (name == "co2_emission")       return [sumLanes](const MSEdge* e) {
        return sumLanes(e, [](MSLane* l) { return l->getEmissions<PollutantsInterface::CO2>(); });
    };
    if (name == "fuel_consumption")   return [sumLanes](const MSEdge* e) {
        return sumLanes(e, [](MSLane* l) { return l->getEmissions<PollutantsInterface::FUEL>(); });
    };
    return [](const MSEdge*) { return 0.0; };
}

// Class byte used by the wire format: {0=vehicle, 1=person, 2=container}.
constexpr uint8_t CLASS_VEHICLE   = 0;
constexpr uint8_t CLASS_PERSON    = 1;
constexpr uint8_t CLASS_CONTAINER = 2;

// ---- persistent extraction state (one instance per process; reset by close()) -----------------
// Holds the per-step output buffers, the type-registry scratch, the resolved attribute getter
// caches, and the active-edges set shared between fillVehicles and fillEdges.
struct ExtractState {
    BatchBuffers buf;

    // Per-attribute column scratch.  Concatenated into buf.veh_attr_vals /
    // buf.edge_attr_vals at the end of the corresponding fill.  Kept as
    // separate strings so each column's capacity is sized to the peak observed
    // vehicle/edge count and preserved across steps.
    std::vector<std::string> vehAttrCols;
    std::vector<std::string> edgeAttrCols;

    // Active edges populated by fillVehicles (each visited vehicle's current edge)
    // and consumed by fillEdges when fullSnapshot is false.
    std::unordered_set<const MSEdge*> activeEdges;

    // Resolved attribute getters; recomputed when the requested attribute list changes.
    std::vector<std::string> lastVehAttrs;
    std::vector<std::string> lastEdgeAttrs;
    std::vector<VehAttrFn>   vehFns;
    std::vector<EdgeAttrFn>  edgeFns;

    // Stable insertion-order vehicle/agent type registry; shared across vehicles, persons
    // and containers so the frontend VehicleTypeDict mirrors the Python publisher exactly.
    std::unordered_map<std::string, uint32_t> typeIndex;
    uint32_t typeCount = 0;
    bool     typeDictDirty = false;

    // Typed-array scratch carried over per VehicleTypeDict rebuild; read by the eCAL
    // publish wrapper when typeDictDirty is true.
    std::string typeIdBlock;
    std::string typeLengths;
    std::string typeWidths;
    std::string typeShapes;
    std::string typeClasses;

    // Phase timers (nanoseconds, accumulated across publishSimStep calls; read by getStats).
    uint64_t calls       = 0;
    uint64_t vehNs       = 0;
    uint64_t agentNs     = 0;
    uint64_t edgeNs      = 0;
    uint64_t tlsNs       = 0;
    uint64_t serializeNs = 0;
    uint64_t sendNs      = 0;
    uint64_t typedictNs  = 0;
};

ExtractState& state() {
    static ExtractState s;
    return s;
}

// Register a vehicle/agent type if not yet seen; flag the type dict for republish.
uint32_t registerType(ExtractState& s, const MSVehicleType& vt, uint8_t classByte) {
    const std::string& tid = vt.getID();
    auto it = s.typeIndex.find(tid);
    if (it != s.typeIndex.end()) {
        return it->second;
    }
    const uint32_t idx = s.typeCount++;
    s.typeIndex.emplace(tid, idx);
    appendCString(s.typeIdBlock, tid);
    appendLE<float>(s.typeLengths, static_cast<float>(vt.getLength()));
    appendLE<float>(s.typeWidths,  static_cast<float>(vt.getWidth()));
    appendCString(s.typeShapes, getVehicleShapeName(vt.getGuiShape()));
    s.typeClasses.push_back(static_cast<char>(classByte));
    s.typeDictDirty = true;
    return idx;
}

// (re)resolve the attribute getter caches when the requested attribute list changed
void refreshVehAttrFns(ExtractState& s, const std::vector<std::string>& vehAttrs) {
    if (vehAttrs == s.lastVehAttrs) return;
    s.vehFns.clear();
    for (const auto& n : vehAttrs) s.vehFns.push_back(resolveVehAttr(n));
    s.lastVehAttrs = vehAttrs;
    s.vehAttrCols.resize(vehAttrs.size());
}

void refreshEdgeAttrFns(ExtractState& s, const std::vector<std::string>& edgeAttrs) {
    if (edgeAttrs == s.lastEdgeAttrs) return;
    s.edgeFns.clear();
    for (const auto& n : edgeAttrs) s.edgeFns.push_back(resolveEdgeAttr(n));
    s.lastEdgeAttrs = edgeAttrs;
    s.edgeAttrCols.resize(edgeAttrs.size());
}

// Vehicle::isVisible — mirrors libsumo/Vehicle.cpp:82.
inline bool isVehicleVisible(const SUMOVehicle* v) {
    return v->isOnRoad() || v->isParking() || v->wasRemoteControlled();
}

// Append every visible transportable in @p tc (persons or containers) to the
// agent section of the BatchBuffers held in @p s.  Returns the number emitted.
uint32_t appendTransportables(ExtractState& s, MSTransportableControl* tc, uint8_t classByte, bool geoReferenced) {
    if (tc == nullptr) return 0;
    const GeoConvHelper& gch = GeoConvHelper::getFinal();
    uint32_t count = 0;
    for (auto it = tc->loadedBegin(); it != tc->loadedEnd(); ++it) {
        const MSTransportable* t = it->second;
        if (t->getCurrentStageType() == MSStageType::WAITING_FOR_DEPART) continue;
        Position p = t->getPosition();
        if (geoReferenced) {
            gch.cartesian2geo(p);
        }
        appendLE<double>(s.buf.agent_positions, p.x());
        appendLE<double>(s.buf.agent_positions, p.y());
        appendLE<double>(s.buf.agent_positions, p.z());
        appendLE<float>(s.buf.agent_angles, static_cast<float>(GeomHelper::naviDegree(t->getAngle())));
        appendCString(s.buf.agent_ids, t->getID());
        const uint32_t tIdx = registerType(s, t->getVehicleType(), classByte);
        appendLE<uint32_t>(s.buf.agent_type_indices, tIdx);
        ++count;
    }
    return count;
}

} // namespace


// ===================================================================================
// public API — always-available section
// ===================================================================================

void Batch::beginStep() {
    ExtractState& s = state();
    BatchBuffers& b = s.buf;
    b.veh_positions.clear();
    b.veh_angles.clear();
    b.veh_speeds.clear();
    b.veh_ids.clear();
    b.veh_type_indices.clear();
    b.veh_attr_vals.clear();
    b.veh_count = 0;
    b.veh_attr_count = 0;
    b.agent_positions.clear();
    b.agent_angles.clear();
    b.agent_ids.clear();
    b.agent_type_indices.clear();
    b.agent_count = 0;
    b.edge_indices.clear();
    b.edge_attr_vals.clear();
    b.edge_count = 0;
    b.edge_attr_count = 0;
    b.tls_ids.clear();
    b.tls_states.clear();
    b.tls_count = 0;
    for (auto& c : s.vehAttrCols)  c.clear();
    for (auto& c : s.edgeAttrCols) c.clear();
    s.activeEdges.clear();
}

uint32_t Batch::fillVehicles(const std::vector<std::string>& attrs, bool geoReferenced) {
    ExtractState& s = state();
    refreshVehAttrFns(s, attrs);
    const size_t Kv = s.vehFns.size();

    uint32_t vehCount = 0;
    MSVehicleControl& vc = MSNet::getInstance()->getVehicleControl();
    for (auto it = vc.loadedVehBegin(); it != vc.loadedVehEnd(); ++it) {
        const SUMOVehicle* sv = it->second;
        if (!isVehicleVisible(sv)) continue;
        const MSBaseVehicle* v = dynamic_cast<const MSBaseVehicle*>(sv);
        if (v == nullptr) continue;

        Position p = v->getPosition();
        if (geoReferenced) {
            GeoConvHelper::getFinal().cartesian2geo(p);
        }
        appendLE<double>(s.buf.veh_positions, p.x());
        appendLE<double>(s.buf.veh_positions, p.y());
        appendLE<double>(s.buf.veh_positions, p.z());
        appendLE<float>(s.buf.veh_angles, static_cast<float>(GeomHelper::naviDegree(v->getAngle())));
        appendLE<float>(s.buf.veh_speeds, static_cast<float>(v->getSpeed()));
        for (size_t k = 0; k < Kv; ++k) {
            appendLE<float>(s.vehAttrCols[k], static_cast<float>(s.vehFns[k](v)));
        }
        appendCString(s.buf.veh_ids, v->getID());
        const uint32_t tIdx = registerType(s, v->getVehicleType(), CLASS_VEHICLE);
        appendLE<uint32_t>(s.buf.veh_type_indices, tIdx);

        const MSEdge* edge = v->getEdge();
        if (edge != nullptr) s.activeEdges.insert(edge);
        ++vehCount;
    }

    // Concatenate attribute columns into veh_attr_vals.
    for (const auto& c : s.vehAttrCols) s.buf.veh_attr_vals.append(c);
    s.buf.veh_count = vehCount;
    s.buf.veh_attr_count = static_cast<uint32_t>(Kv);
    return vehCount;
}

uint32_t Batch::fillAgents(bool geoReferenced) {
    ExtractState& s = state();
    MSNet* net = MSNet::getInstance();
    uint32_t agentCount = 0;
    if (net->hasPersons())    agentCount += appendTransportables(s, &net->getPersonControl(),    CLASS_PERSON,    geoReferenced);
    if (net->hasContainers()) agentCount += appendTransportables(s, &net->getContainerControl(), CLASS_CONTAINER, geoReferenced);
    s.buf.agent_count = agentCount;
    return agentCount;
}

uint32_t Batch::fillEdges(const std::vector<std::string>& attrs, bool fullSnapshot) {
    ExtractState& s = state();
    refreshEdgeAttrFns(s, attrs);
    const size_t Ke = s.edgeFns.size();

    uint32_t edgeOutCount = 0;
    if (Ke > 0) {
        const MSEdgeVector& edges = MSNet::getInstance()->getEdgeControl().getEdges();
        // sumolib.net.readNet(withInternal=True) enumerates internal + crossing +
        // walkingarea + normal edges in MSEdge loading order, so position in this
        // loop is the index the frontend stores.  We skip internal edges from the
        // value stream but the index counter still advances across them so the
        // emitted indices align with the frontend's edge_ids array.
        uint32_t extIdx = 0;
        for (const MSEdge* e : edges) {
            const std::string& eid = e->getID();
            const bool isInternal = !eid.empty() && eid[0] == ':';
            if (!isInternal) {
                const bool include = fullSnapshot || s.activeEdges.count(e) > 0;
                if (include) {
                    appendLE<uint32_t>(s.buf.edge_indices, extIdx);
                    for (size_t k = 0; k < Ke; ++k) {
                        appendLE<float>(s.edgeAttrCols[k], static_cast<float>(s.edgeFns[k](e)));
                    }
                    ++edgeOutCount;
                }
            }
            ++extIdx;
        }
    }
    for (const auto& c : s.edgeAttrCols) s.buf.edge_attr_vals.append(c);
    s.buf.edge_count = edgeOutCount;
    s.buf.edge_attr_count = static_cast<uint32_t>(Ke);
    return edgeOutCount;
}

uint32_t Batch::fillTLS() {
    ExtractState& s = state();
    uint32_t tlsCount = 0;
    MSTLLogicControl& tlc = MSNet::getInstance()->getTLSControl();
    const std::vector<std::string> ids = tlc.getAllTLIds();
    for (const std::string& id : ids) {
        const MSTrafficLightLogic* logic = tlc.getActive(id);
        if (logic == nullptr) continue;
        appendCString(s.buf.tls_ids, id);
        appendCString(s.buf.tls_states, logic->getCurrentPhaseDef().getState());
        ++tlsCount;
    }
    s.buf.tls_count = tlsCount;
    return tlsCount;
}

bool Batch::drainNewTypes() {
    ExtractState& s = state();
    const bool dirty = s.typeDictDirty;
    s.typeDictDirty = false;
    return dirty;
}

const BatchBuffers& Batch::buffers() {
    return state().buf;
}

} // namespace libsumo


// ===================================================================================
// optional eCAL publisher (HAVE_ECAL build only)
// ===================================================================================
#ifdef HAVE_ECAL

#include <chrono>
#include <memory>
#include <sstream>

#include <ecal/ecal.h>
#include <ecal/pubsub/publisher.h>

#include "sumo_ecal.pb.h"

namespace libsumo {

namespace {

struct EcalState {
    std::unique_ptr<eCAL::CPublisher> pubSimstep;
    std::unique_ptr<eCAL::CPublisher> pubTypedict;

    // Per-step protobuf scratch.  Cleared at the start of every publish to
    // preserve the capacity of its bytes fields.  After warmup, the swap-with-
    // BatchBuffers technique below performs zero heap allocations.
    sumo::SimStepBin sb;
    sumo::VehicleTypeDict typeDict;
    std::string outBuf;
};

EcalState& ecalState() {
    static EcalState es;
    return es;
}

// Build and publish a fresh VehicleTypeDict from the current type registry.
void publishTypeDictNow() {
    EcalState& es = ecalState();
    if (!es.pubTypedict) return;
    ExtractState& s = state();
    sumo::VehicleTypeDict& td = es.typeDict;
    td.Clear();
    td.set_type_count(s.typeCount);
    td.set_type_id_block(s.typeIdBlock);
    td.set_type_lengths(s.typeLengths);
    td.set_type_widths(s.typeWidths);
    td.set_type_shapes(s.typeShapes);
    td.set_type_classes(s.typeClasses);
    std::string buf;
    td.SerializeToString(&buf);
    es.pubTypedict->Send(buf);
}

} // namespace

bool Batch::available() { return true; }

void Batch::init(const std::string& simstepTopic, const std::string& typedictTopic) {
    EcalState& es = ecalState();
    eCAL::SDataTypeInformation dtiSim;
    dtiSim.encoding = "proto";
    dtiSim.name     = "sumo.SimStepBin";
    es.pubSimstep   = std::make_unique<eCAL::CPublisher>(simstepTopic, dtiSim);

    eCAL::SDataTypeInformation dtiVtd;
    dtiVtd.encoding = "proto";
    dtiVtd.name     = "sumo.VehicleTypeDict";
    es.pubTypedict  = std::make_unique<eCAL::CPublisher>(typedictTopic, dtiVtd);

    ExtractState& s = state();
    s.typeIndex.clear();
    s.typeIdBlock.clear();
    s.typeLengths.clear();
    s.typeWidths.clear();
    s.typeShapes.clear();
    s.typeClasses.clear();
    s.typeCount = 0;
    s.typeDictDirty = false;
    s.lastVehAttrs.clear();
    s.lastEdgeAttrs.clear();
    s.vehFns.clear();
    s.edgeFns.clear();
}

void Batch::close() {
    EcalState& es = ecalState();
    es.pubSimstep.reset();
    es.pubTypedict.reset();
    ExtractState& s = state();
    s.typeIndex.clear();
    s.typeIdBlock.clear();
    s.typeLengths.clear();
    s.typeWidths.clear();
    s.typeShapes.clear();
    s.typeClasses.clear();
    s.typeCount = 0;
    s.typeDictDirty = false;
    s.lastVehAttrs.clear();
    s.lastEdgeAttrs.clear();
    s.vehFns.clear();
    s.edgeFns.clear();
}

unsigned int Batch::publishSimStep(const std::vector<std::string>& vehAttrs,
                                   const std::vector<std::string>& edgeAttrs,
                                   bool fullEdgeSnapshot,
                                   int seq) {
    EcalState& es = ecalState();
    if (!es.pubSimstep) return 0;
    ExtractState& s = state();

    const bool geoReferenced = GeoConvHelper::getFinal().usingGeoProjection();

    using clk = std::chrono::steady_clock;

    Batch::beginStep();

    auto t0 = clk::now();
    const uint32_t vehCount = Batch::fillVehicles(vehAttrs, geoReferenced);
    auto t1 = clk::now();
    Batch::fillAgents(geoReferenced);
    auto t2 = clk::now();
    Batch::fillEdges(edgeAttrs, fullEdgeSnapshot);
    auto t3 = clk::now();
    Batch::fillTLS();
    auto t4 = clk::now();

    // Republish the type dict BEFORE the simstep so the frontend has new types
    // before it sees indices that reference them.
    if (Batch::drainNewTypes()) {
        publishTypeDictNow();
    }
    auto t5 = clk::now();

    // ---- pack the protobuf SimStepBin -----------------------------------------------
    // Swap our BatchBuffers strings into the proto's mutable_*() backing strings:
    // O(1) per field, zero heap allocation in steady state.  After Send() we swap
    // back so the BatchBuffers regain their capacity for the next step.
    BatchBuffers& b = s.buf;
    sumo::SimStepBin& sb = es.sb;
    sb.Clear();
    sb.set_edge_full_snapshot(fullEdgeSnapshot);
    sb.set_time_ms(static_cast<int64_t>(MSNet::getInstance()->getCurrentTimeStep()));
    sb.set_seq_num(static_cast<uint32_t>(seq));
    sb.set_veh_count(b.veh_count);
    sb.set_veh_attr_count(b.veh_attr_count);
    sb.set_agent_count(b.agent_count);
    sb.set_edge_count(b.edge_count);
    sb.set_edge_attr_count(b.edge_attr_count);
    sb.set_tls_count(b.tls_count);

    sb.mutable_veh_positions()->swap(b.veh_positions);
    sb.mutable_veh_angles()->swap(b.veh_angles);
    sb.mutable_veh_speeds()->swap(b.veh_speeds);
    sb.mutable_vehicle_ids()->swap(b.veh_ids);
    sb.mutable_veh_type_indices()->swap(b.veh_type_indices);
    sb.mutable_veh_attr_vals()->swap(b.veh_attr_vals);
    sb.mutable_agent_positions()->swap(b.agent_positions);
    sb.mutable_agent_angles()->swap(b.agent_angles);
    sb.mutable_agent_ids()->swap(b.agent_ids);
    sb.mutable_agent_type_indices()->swap(b.agent_type_indices);
    sb.mutable_edge_indices()->swap(b.edge_indices);
    sb.mutable_edge_attr_vals()->swap(b.edge_attr_vals);
    sb.mutable_tls_ids()->swap(b.tls_ids);
    sb.mutable_tls_states()->swap(b.tls_states);

    sb.SerializeToString(&es.outBuf);
    auto t6 = clk::now();

    es.pubSimstep->Send(es.outBuf);
    auto t7 = clk::now();

    // Swap the buffers back so BatchBuffers regains its capacity.  After Clear()
    // at the next publish call, sb's bytes fields will also retain capacity.
    sb.mutable_veh_positions()->swap(b.veh_positions);
    sb.mutable_veh_angles()->swap(b.veh_angles);
    sb.mutable_veh_speeds()->swap(b.veh_speeds);
    sb.mutable_vehicle_ids()->swap(b.veh_ids);
    sb.mutable_veh_type_indices()->swap(b.veh_type_indices);
    sb.mutable_veh_attr_vals()->swap(b.veh_attr_vals);
    sb.mutable_agent_positions()->swap(b.agent_positions);
    sb.mutable_agent_angles()->swap(b.agent_angles);
    sb.mutable_agent_ids()->swap(b.agent_ids);
    sb.mutable_agent_type_indices()->swap(b.agent_type_indices);
    sb.mutable_edge_indices()->swap(b.edge_indices);
    sb.mutable_edge_attr_vals()->swap(b.edge_attr_vals);
    sb.mutable_tls_ids()->swap(b.tls_ids);
    sb.mutable_tls_states()->swap(b.tls_states);

    using ns = std::chrono::nanoseconds;
    s.calls       += 1;
    s.vehNs       += std::chrono::duration_cast<ns>(t1 - t0).count();
    s.agentNs     += std::chrono::duration_cast<ns>(t2 - t1).count();
    s.edgeNs      += std::chrono::duration_cast<ns>(t3 - t2).count();
    s.tlsNs       += std::chrono::duration_cast<ns>(t4 - t3).count();
    s.typedictNs  += std::chrono::duration_cast<ns>(t5 - t4).count();
    s.serializeNs += std::chrono::duration_cast<ns>(t6 - t5).count();
    s.sendNs      += std::chrono::duration_cast<ns>(t7 - t6).count();
    return vehCount;
}

std::string Batch::getStats(bool reset) {
    ExtractState& s = state();
    std::ostringstream os;
    os << "calls=" << s.calls
       << " veh_us="       << (s.vehNs       / 1000)
       << " agent_us="     << (s.agentNs     / 1000)
       << " edge_us="      << (s.edgeNs      / 1000)
       << " tls_us="       << (s.tlsNs       / 1000)
       << " typedict_us="  << (s.typedictNs  / 1000)
       << " serialize_us=" << (s.serializeNs / 1000)
       << " send_us="      << (s.sendNs      / 1000);
    if (reset) {
        s.calls = s.vehNs = s.agentNs = s.edgeNs = s.tlsNs
                = s.serializeNs = s.sendNs = s.typedictNs = 0;
    }
    return os.str();
}

} // namespace libsumo

#else  // !HAVE_ECAL

namespace libsumo {

bool Batch::available() { return false; }
void Batch::init(const std::string&, const std::string&) {}
void Batch::close() {}
unsigned int Batch::publishSimStep(const std::vector<std::string>&,
                                   const std::vector<std::string>&,
                                   bool, int) { return 0; }
std::string Batch::getStats(bool) { return std::string(); }

} // namespace libsumo

#endif // HAVE_ECAL
