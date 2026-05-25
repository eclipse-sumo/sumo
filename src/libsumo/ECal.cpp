/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2012-2026 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    ECal.cpp
/// @date    2026
///
// Implementation — see ECal.h for the public contract.
/****************************************************************************/
#include <config.h>

#include "ECal.h"

#ifdef HAVE_ECAL

#include <cstdint>
#include <cstring>
#include <chrono>
#include <functional>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <ecal/ecal.h>
#include <ecal/pubsub/publisher.h>

#include <microsim/MSEdge.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSLane.h>
#include <microsim/MSNet.h>
#include <microsim/MSBaseVehicle.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSVehicleType.h>
#include <microsim/transportables/MSTransportable.h>
#include <microsim/transportables/MSTransportableControl.h>
#include <microsim/transportables/MSStage.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/emissions/PollutantsInterface.h>
#include <utils/geom/GeomHelper.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/geom/Position.h>

#include "sumo_ecal.pb.h"


namespace libsumo {

namespace {

// ---- typed-array append helpers ---------------------------------------------------------------
// Protobuf "bytes" fields hold the packed little-endian representation of typed numeric arrays.
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
        // accumulated waiting is microscopic-only; meso falls back to current waiting seconds.
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

// Class byte used by the Python publisher: {0=vehicle, 1=person, 2=container}.
constexpr uint8_t CLASS_VEHICLE   = 0;
constexpr uint8_t CLASS_PERSON    = 1;
constexpr uint8_t CLASS_CONTAINER = 2;

// ---- persistent state (one instance per process; reset by close()) ----------------------------
struct State {
    std::unique_ptr<eCAL::CPublisher> pubSimstep;
    std::unique_ptr<eCAL::CPublisher> pubTypedict;

    // stable insertion-order vehicle/agent type registry; shared across vehicles, persons,
    // containers so the frontend VehicleTypeDict mirrors the Python implementation exactly.
    std::unordered_map<std::string, uint32_t> typeIndex;
    sumo::VehicleTypeDict typeDict;
    bool typeDictDirty = false;
    uint32_t typeCount = 0;

    // typed-array scratch buffers carried over per VehicleTypeDict rebuild
    std::string typeIdBlock;
    std::string typeLengths;
    std::string typeWidths;
    std::string typeShapes;
    std::string typeClasses;

    // resolved attribute getters; recomputed when the requested attribute list changes
    std::vector<std::string> lastVehAttrs;
    std::vector<std::string> lastEdgeAttrs;
    std::vector<VehAttrFn>  vehFns;
    std::vector<EdgeAttrFn> edgeFns;

    // per-step scratch hoisted out of publishSimStep so that:
    //  * sb itself preserves the capacity of its internal bytes fields across Clear()
    //  * sb.mutable_<field>() is appended to directly (no separate std::string scratch)
    //  * vehAttrCols/edgeAttrCols are reused across steps (one std::string per attribute
    //    column, sized to peak observed vehicle/edge count)
    //  * outBuf reuses its serialization capacity
    // After warmup, publishSimStep performs zero heap allocations for these buffers.
    sumo::SimStepBin sb;
    std::string outBuf;
    std::vector<std::string> vehAttrCols;
    std::vector<std::string> edgeAttrCols;
    std::unordered_set<const MSEdge*> activeEdges;

    // phase timers (nanoseconds, accumulated across publishSimStep calls).
    uint64_t calls       = 0;
    uint64_t vehNs       = 0;
    uint64_t agentNs     = 0;
    uint64_t edgeNs      = 0;
    uint64_t serializeNs = 0;
    uint64_t sendNs      = 0;
    uint64_t typedictNs  = 0;
};

State& state() {
    static State s;
    return s;
}

// Register a vehicle/agent type if not yet seen; flag the type dict for republish.
uint32_t registerType(State& s, const MSVehicleType& vt, uint8_t classByte) {
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

void publishTypeDictIfDirty(State& s) {
    if (!s.typeDictDirty || !s.pubTypedict) {
        return;
    }
    s.typeDict.Clear();
    s.typeDict.set_type_count(s.typeCount);
    s.typeDict.set_type_id_block(s.typeIdBlock);
    s.typeDict.set_type_lengths(s.typeLengths);
    s.typeDict.set_type_widths(s.typeWidths);
    s.typeDict.set_type_shapes(s.typeShapes);
    s.typeDict.set_type_classes(s.typeClasses);
    std::string buf;
    s.typeDict.SerializeToString(&buf);
    s.pubTypedict->Send(buf);
    s.typeDictDirty = false;
}

// (re)resolve the attribute getter caches when the requested attribute list changed
void refreshAttrFns(State& s,
                    const std::vector<std::string>& vehAttrs,
                    const std::vector<std::string>& edgeAttrs) {
    if (vehAttrs != s.lastVehAttrs) {
        s.vehFns.clear();
        for (const auto& n : vehAttrs) s.vehFns.push_back(resolveVehAttr(n));
        s.lastVehAttrs = vehAttrs;
        // resize the column scratch to match; existing strings keep their capacity.
        s.vehAttrCols.resize(vehAttrs.size());
    }
    if (edgeAttrs != s.lastEdgeAttrs) {
        s.edgeFns.clear();
        for (const auto& n : edgeAttrs) s.edgeFns.push_back(resolveEdgeAttr(n));
        s.lastEdgeAttrs = edgeAttrs;
        s.edgeAttrCols.resize(edgeAttrs.size());
    }
}

// Vehicle::isVisible — mirrors libsumo/Vehicle.cpp:82.
inline bool isVehicleVisible(const SUMOVehicle* v) {
    return v->isOnRoad() || v->isParking() || v->wasRemoteControlled();
}

// Append every visible transportable in @p tc (persons or containers) to the agent
// section of the SimStepBin held in @p s.  Returns the number of transportables
// emitted so the caller can sum the counts of persons + containers.
uint32_t appendTransportables(State& s, MSTransportableControl* tc, uint8_t classByte) {
    if (tc == nullptr) return 0;
    const GeoConvHelper& gch = GeoConvHelper::getFinal();
    const bool geoReferenced = gch.usingGeoProjection();
    std::string* agentPositions = s.sb.mutable_agent_positions();
    std::string* agentAngles    = s.sb.mutable_agent_angles();
    std::string* agentIds       = s.sb.mutable_agent_ids();
    std::string* agentTypeIdx   = s.sb.mutable_agent_type_indices();
    uint32_t count = 0;
    for (auto it = tc->loadedBegin(); it != tc->loadedEnd(); ++it) {
        const MSTransportable* t = it->second;
        if (t->getCurrentStageType() == MSStageType::WAITING_FOR_DEPART) continue;
        Position p = t->getPosition();
        if (geoReferenced) {
            gch.cartesian2geo(p);
        }
        appendLE<double>(*agentPositions, p.x());
        appendLE<double>(*agentPositions, p.y());
        appendLE<double>(*agentPositions, p.z());
        appendLE<float>(*agentAngles, static_cast<float>(GeomHelper::naviDegree(t->getAngle())));
        appendCString(*agentIds, t->getID());
        const uint32_t tIdx = registerType(s, t->getVehicleType(), classByte);
        appendLE<uint32_t>(*agentTypeIdx, tIdx);
        ++count;
    }
    return count;
}

} // namespace


// ===================================================================================
// public API
// ===================================================================================

bool ECal::available() { return true; }

void ECal::init(const std::string& simstepTopic, const std::string& typedictTopic) {
    State& s = state();
    eCAL::SDataTypeInformation dtiSim;
    dtiSim.encoding = "proto";
    dtiSim.name     = "sumo.SimStepBin";
    s.pubSimstep    = std::make_unique<eCAL::CPublisher>(simstepTopic, dtiSim);

    eCAL::SDataTypeInformation dtiVtd;
    dtiVtd.encoding = "proto";
    dtiVtd.name     = "sumo.VehicleTypeDict";
    s.pubTypedict   = std::make_unique<eCAL::CPublisher>(typedictTopic, dtiVtd);

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

void ECal::close() {
    State& s = state();
    s.pubSimstep.reset();
    s.pubTypedict.reset();
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

unsigned int ECal::publishSimStep(const std::vector<std::string>& vehAttrs,
                                  const std::vector<std::string>& edgeAttrs,
                                  bool fullEdgeSnapshot,
                                  int seq) {
    State& s = state();
    if (!s.pubSimstep) return 0;

    refreshAttrFns(s, vehAttrs, edgeAttrs);

    // GeoConvHelper knows from the parsed network whether a projection is in
    // use; detect once per step so we don't pay a per-vehicle lookup.
    const bool geoReferenced = GeoConvHelper::getFinal().usingGeoProjection();

    // Reset per-step scratch.  sb.Clear() preserves the capacity of its bytes fields
    // so mutable_*() returns the same backing std::string buffer as the previous step
    // (just resized to 0).  outBuf and the attribute column scratches keep their
    // capacity across clear() too.  Net result: zero heap allocations per step in
    // steady state.
    sumo::SimStepBin& sb = s.sb;
    sb.Clear();
    s.outBuf.clear();
    for (auto& c : s.vehAttrCols)  c.clear();
    for (auto& c : s.edgeAttrCols) c.clear();
    s.activeEdges.clear();

    sb.set_edge_full_snapshot(fullEdgeSnapshot);
    sb.set_time_ms(static_cast<int64_t>(MSNet::getInstance()->getCurrentTimeStep()));
    sb.set_seq_num(static_cast<uint32_t>(seq));

    const size_t Kv = s.vehFns.size();
    const size_t Ke = s.edgeFns.size();

    using clk = std::chrono::steady_clock;
    auto t0 = clk::now();

    // ------ vehicle section --------------------------------------------------------------------
    std::string* vehPositions = sb.mutable_veh_positions();
    std::string* vehAngles    = sb.mutable_veh_angles();
    std::string* vehSpeeds    = sb.mutable_veh_speeds();
    std::string* vehIds       = sb.mutable_vehicle_ids();
    std::string* vehTypeIdx   = sb.mutable_veh_type_indices();
    uint32_t vehCount = 0;

    MSVehicleControl& vc = MSNet::getInstance()->getVehicleControl();
    for (auto it = vc.loadedVehBegin(); it != vc.loadedVehEnd(); ++it) {
        const SUMOVehicle* sv = it->second;
        if (!isVehicleVisible(sv)) continue;
        const MSBaseVehicle* v = dynamic_cast<const MSBaseVehicle*>(sv);
        if (v == nullptr) continue;  // non-MSBaseVehicle implementation (shouldn't happen)

        Position p = v->getPosition();
        if (geoReferenced) {
            GeoConvHelper::getFinal().cartesian2geo(p);
        }
        appendLE<double>(*vehPositions, p.x());
        appendLE<double>(*vehPositions, p.y());
        appendLE<double>(*vehPositions, p.z());
        appendLE<float>(*vehAngles, static_cast<float>(GeomHelper::naviDegree(v->getAngle())));
        appendLE<float>(*vehSpeeds, static_cast<float>(v->getSpeed()));
        for (size_t k = 0; k < Kv; ++k) {
            appendLE<float>(s.vehAttrCols[k], static_cast<float>(s.vehFns[k](v)));
        }
        appendCString(*vehIds, v->getID());
        const uint32_t tIdx = registerType(s, v->getVehicleType(), CLASS_VEHICLE);
        appendLE<uint32_t>(*vehTypeIdx, tIdx);

        const MSEdge* edge = v->getEdge();
        if (edge != nullptr) s.activeEdges.insert(edge);
        ++vehCount;
    }

    // Concatenate attribute columns into mutable_veh_attr_vals().
    std::string* vehAttrVals = sb.mutable_veh_attr_vals();
    for (const auto& c : s.vehAttrCols) vehAttrVals->append(c);

    sb.set_veh_count(vehCount);
    sb.set_veh_attr_count(static_cast<uint32_t>(Kv));

    auto t1 = clk::now();

    // ------ agent section (persons + containers) -----------------------------------------------
    MSNet* net = MSNet::getInstance();
    uint32_t agentCount = 0;
    if (net->hasPersons())    agentCount += appendTransportables(s, &net->getPersonControl(),    CLASS_PERSON);
    if (net->hasContainers()) agentCount += appendTransportables(s, &net->getContainerControl(), CLASS_CONTAINER);

    sb.set_agent_count(agentCount);

    auto t2 = clk::now();

    // ------ edge section -----------------------------------------------------------------------
    std::string* edgeIndices = sb.mutable_edge_indices();
    uint32_t edgeOutCount = 0;

    if (Ke > 0) {
        const MSEdgeVector& edges = MSNet::getInstance()->getEdgeControl().getEdges();
        // External edges only (matches sumolib.net.readNet(withInternal=False) ordering used
        // by the Python NetworkGeometry builder). Position in this filtered enumeration is the
        // index that the frontend stores in NetworkGeometry.edge_ids.
        uint32_t extIdx = 0;
        for (const MSEdge* e : edges) {
            const std::string& eid = e->getID();
            if (!eid.empty() && eid[0] == ':') continue;  // skip internal
            const bool include = fullEdgeSnapshot || s.activeEdges.count(e) > 0;
            if (include) {
                appendLE<uint32_t>(*edgeIndices, extIdx);
                for (size_t k = 0; k < Ke; ++k) {
                    appendLE<float>(s.edgeAttrCols[k], static_cast<float>(s.edgeFns[k](e)));
                }
                ++edgeOutCount;
            }
            ++extIdx;
        }
    }

    std::string* edgeAttrVals = sb.mutable_edge_attr_vals();
    for (const auto& c : s.edgeAttrCols) edgeAttrVals->append(c);

    sb.set_edge_count(edgeOutCount);
    sb.set_edge_attr_count(static_cast<uint32_t>(Ke));

    auto t3 = clk::now();

    // ------ publish ----------------------------------------------------------------------------
    publishTypeDictIfDirty(s);  // BEFORE simstep so frontend has new types before it sees indices

    auto t4 = clk::now();

    sb.SerializeToString(&s.outBuf);

    auto t5 = clk::now();

    s.pubSimstep->Send(s.outBuf);

    auto t6 = clk::now();

    using ns = std::chrono::nanoseconds;
    s.calls       += 1;
    s.vehNs       += std::chrono::duration_cast<ns>(t1 - t0).count();
    s.agentNs     += std::chrono::duration_cast<ns>(t2 - t1).count();
    s.edgeNs      += std::chrono::duration_cast<ns>(t3 - t2).count();
    s.typedictNs  += std::chrono::duration_cast<ns>(t4 - t3).count();
    s.serializeNs += std::chrono::duration_cast<ns>(t5 - t4).count();
    s.sendNs      += std::chrono::duration_cast<ns>(t6 - t5).count();
    return vehCount;
}

std::string ECal::getStats(bool reset) {
    State& s = state();
    std::ostringstream os;
    os << "calls=" << s.calls
       << " veh_us="       << (s.vehNs       / 1000)
       << " agent_us="     << (s.agentNs     / 1000)
       << " edge_us="      << (s.edgeNs      / 1000)
       << " typedict_us="  << (s.typedictNs  / 1000)
       << " serialize_us=" << (s.serializeNs / 1000)
       << " send_us="      << (s.sendNs      / 1000);
    if (reset) {
        s.calls = s.vehNs = s.agentNs = s.edgeNs = s.serializeNs = s.sendNs = s.typedictNs = 0;
    }
    return os.str();
}

} // namespace libsumo

#else  // !HAVE_ECAL

namespace libsumo {

bool ECal::available() { return false; }
void ECal::init(const std::string&, const std::string&) {}
void ECal::close() {}
unsigned int ECal::publishSimStep(const std::vector<std::string>&,
                                  const std::vector<std::string>&,
                                  bool, int) { return 0; }
std::string ECal::getStats(bool) { return std::string(); }

} // namespace libsumo

#endif // HAVE_ECAL
