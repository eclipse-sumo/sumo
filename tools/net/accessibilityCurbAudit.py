#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2010-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    accessibilityCurbAudit.py
# @author  Horizon Europe MSCA STEP-UP Consortium
# @date    2026-11-20

"""
NetworkX + ``sumolib`` pedestrian accessibility router and ``.net.xml`` curb auditor.

Enforces universal-design physical barrier thresholds:
- Curb step height ``<= 0.02 m`` (``2 cm``) and longitudinal/ramp slope ``<= 8%`` (``0.08``)
  for wheelchair passability, flagging ``UNREACHABLE_ACCESSIBILITY_BARRIER`` when
  physical barriers sever accessible connectivity;
- Tactile paving guidance weighting for blind/low-vision pedestrians;
- Rest-bench fatigue relief weighting for older adults and manual wheelchair users;
- Pluggable night-time lighting / perceived-safety edge-cost hook.
"""

from __future__ import annotations

import dataclasses
import json
import math
import sys
import xml.etree.ElementTree as ET
from pathlib import Path
from typing import Any, Callable, Dict, List, Optional, Sequence, Set, Tuple

import networkx as nx
import sumolib
from sumolib.options import ArgumentParser


UNREACHABLE_ACCESSIBILITY_BARRIER: str = "UNREACHABLE_ACCESSIBILITY_BARRIER"
MAX_WHEELCHAIR_CURB_HEIGHT_M: float = 0.02
MAX_WHEELCHAIR_SLOPE: float = 0.08


@dataclasses.dataclass(frozen=True)
class AccessibilityRouteResult:
    status: str
    origin_lane: str
    dest_lane: str
    profile: str
    path_lanes: Tuple[str, ...]
    physical_distance_m: float
    generalized_cost: float
    baseline_distance_m: float
    detour_ratio: float


@dataclasses.dataclass(frozen=True)
class SidewalkAuditRecord:
    lane_id: str
    edge_id: str
    width_m: float
    length_m: float
    slope: float
    has_tactile_paving: bool
    has_rest_bench: bool
    illuminance_lux: float
    width_compliant: bool
    wheelchair_passable: bool


@dataclasses.dataclass(frozen=True)
class CrossingAuditRecord:
    crossing_id: str
    lane_id: str
    node_id: str
    width_m: float
    length_m: float
    priority_or_signalized: bool
    has_curb_ramp: bool
    curb_height_m: float
    slope: float
    has_tactile_paving: bool
    has_aps_beacon: bool
    wheelchair_accessible: bool


@dataclasses.dataclass(frozen=True)
class RefugeIslandRecord:
    walkingarea_id: str
    lane_id: str
    width_m: float
    length_m: float
    connected_crossings: Tuple[str, ...]
    depth_compliant: bool


@dataclasses.dataclass
class NetworkAccessibilityReport:
    net_file: str
    sidewalk_count: int
    sidewalk_compliance_share: float
    crossing_count: int
    accessible_crossing_count: int
    accessible_crossing_share: float
    tactile_crossing_count: int
    aps_crossing_count: int
    rest_bench_sidewalk_count: int
    refuge_island_count: int
    evaluated_od_pairs: int
    unreachable_od_pairs: int
    barrier_status: str
    mean_detour_ratio: float
    max_detour_ratio: float
    sidewalks: List[SidewalkAuditRecord]
    crossings: List[CrossingAuditRecord]
    refuge_islands: List[RefugeIslandRecord]

    def to_dict(self) -> Dict[str, Any]:
        return dataclasses.asdict(self)


def _parse_bool(val: Optional[str], default: bool) -> bool:
    if val is None:
        return default
    norm = val.strip().lower()
    if norm in ("1", "true", "yes", "y", "on"):
        return True
    if norm in ("0", "false", "no", "n", "off"):
        return False
    return default


def _parse_net_xml_metadata(
    net_file: str | Path,
) -> Tuple[Dict[str, Dict[str, str]], Dict[str, Dict[str, str]], Dict[str, Dict[str, str]]]:
    edge_params: Dict[str, Dict[str, str]] = {}
    lane_params: Dict[str, Dict[str, str]] = {}
    edge_attrs: Dict[str, Dict[str, str]] = {}

    root = ET.parse(str(net_file)).getroot()
    for edge_elem in root.findall("edge"):
        eid = edge_elem.attrib.get("id", "")
        edge_attrs[eid] = dict(edge_elem.attrib)
        ep = {
            p.attrib["key"]: p.attrib["value"]
            for p in edge_elem.findall("param")
            if "key" in p.attrib and "value" in p.attrib
        }
        edge_params[eid] = ep
        for lane_elem in edge_elem.findall("lane"):
            lid = lane_elem.attrib.get("id", "")
            lp = dict(ep)
            for p in lane_elem.findall("param"):
                if "key" in p.attrib and "value" in p.attrib:
                    lp[p.attrib["key"]] = p.attrib["value"]
            lane_params[lid] = lp
    return edge_params, lane_params, edge_attrs


class AccessibilityGraphRouter:
    """NetworkX-backed accessible pedestrian router over SUMO ``.net.xml`` graphs."""

    def __init__(
        self,
        net_file: str | Path,
        max_curb_height_m: float = MAX_WHEELCHAIR_CURB_HEIGHT_M,
        max_slope: float = MAX_WHEELCHAIR_SLOPE,
        min_wheelchair_width_m: float = 0.90,
        min_sidewalk_width_m: float = 1.50,
    ) -> None:
        self.net_file = str(net_file)
        self.max_curb_height_m = float(max_curb_height_m)
        self.max_slope = float(max_slope)
        self.min_wheelchair_width_m = float(min_wheelchair_width_m)
        self.min_sidewalk_width_m = float(min_sidewalk_width_m)

        self.net = sumolib.net.readNet(
            self.net_file, withInternal=True, withPedestrianConnections=True
        )
        self.edge_params, self.lane_params, self.edge_attrs = _parse_net_xml_metadata(
            self.net_file
        )
        self.graph = nx.Graph()
        self.sidewalks: List[SidewalkAuditRecord] = []
        self.crossings: List[CrossingAuditRecord] = []
        self.refuge_islands: List[RefugeIslandRecord] = []
        self.junction_sidewalks: Dict[str, List[str]] = {}
        self._build_graph()

    def _build_graph(self) -> None:
        for edge in self.net.getEdges():
            eid = edge.getID()
            func = edge.getFunction()
            ep = self.edge_params.get(eid, {})
            raw_attr = self.edge_attrs.get(eid, {})

            if func == "":
                for lane in edge.getLanes():
                    if not lane.allows("pedestrian"):
                        continue
                    lid = lane.getID()
                    lp = self.lane_params.get(lid, {})
                    w = float(lane.getWidth())
                    length = float(lane.getLength())
                    slope = abs(float(lp.get("maxSlope", lp.get("slope", "0.0"))))
                    curb_h = float(lp.get("curbHeight", "0.0"))
                    tactile = _parse_bool(lp.get("tactilePaving"), default=False)
                    bench = _parse_bool(lp.get("restBench"), default=False)
                    lux = float(lp.get("illuminanceLux", "20.0"))
                    passable = (
                        (w >= self.min_wheelchair_width_m)
                        and (slope <= self.max_slope)
                        and (curb_h <= self.max_curb_height_m)
                    )
                    self.sidewalks.append(
                        SidewalkAuditRecord(
                            lane_id=lid,
                            edge_id=eid,
                            width_m=w,
                            length_m=length,
                            slope=slope,
                            has_tactile_paving=tactile,
                            has_rest_bench=bench,
                            illuminance_lux=lux,
                            width_compliant=(w >= self.min_sidewalk_width_m),
                            wheelchair_passable=passable,
                        )
                    )
                    self.graph.add_node(
                        lid,
                        kind="sidewalk",
                        length_m=min(length, 4.0),
                        full_length_m=length,
                        wheelchair_ok=passable,
                        tactile=tactile,
                        rest_bench=bench,
                        illuminance_lux=lux,
                    )
                    self.junction_sidewalks.setdefault(
                        edge.getToNode().getID(), []
                    ).append(lid)
                    self.junction_sidewalks.setdefault(
                        edge.getFromNode().getID(), []
                    ).append(lid)

            elif func == "crossing":
                lane = edge.getLanes()[0]
                lid = lane.getID()
                lp = self.lane_params.get(lid, {})
                w = float(lane.getWidth())
                length = float(lane.getLength())
                node_id = (
                    eid[1:].rsplit("_c", 1)[0]
                    if eid.startswith(":") and "_c" in eid
                    else ""
                )
                node_obj = (
                    self.net.getNode(node_id)
                    if node_id and self.net.hasNode(node_id)
                    else None
                )
                is_tls = node_obj is not None and "traffic_light" in node_obj.getType()
                prio = _parse_bool(raw_attr.get("priority"), default=is_tls)
                curb_h = float(lp.get("curbHeight", "0.0"))
                slope = abs(float(lp.get("maxSlope", lp.get("slope", "0.0"))))
                has_ramp = _parse_bool(lp.get("curbRamp"), default=True) and (
                    curb_h <= self.max_curb_height_m
                )
                tactile = _parse_bool(lp.get("tactilePaving"), default=False)
                aps = _parse_bool(lp.get("apsBeacon"), default=False)
                lux = float(lp.get("illuminanceLux", "20.0"))
                whl_ok = (
                    has_ramp
                    and (slope <= self.max_slope)
                    and (w >= self.min_wheelchair_width_m)
                )

                self.crossings.append(
                    CrossingAuditRecord(
                        crossing_id=eid,
                        lane_id=lid,
                        node_id=node_id,
                        width_m=w,
                        length_m=length,
                        priority_or_signalized=bool(is_tls or prio),
                        has_curb_ramp=has_ramp,
                        curb_height_m=curb_h,
                        slope=slope,
                        has_tactile_paving=tactile,
                        has_aps_beacon=aps,
                        wheelchair_accessible=whl_ok,
                    )
                )
                self.graph.add_node(
                    lid,
                    kind="crossing",
                    length_m=length,
                    full_length_m=length,
                    wheelchair_ok=whl_ok,
                    tactile=tactile,
                    rest_bench=False,
                    illuminance_lux=lux,
                )

            elif func == "walkingarea":
                lane = edge.getLanes()[0]
                lid = lane.getID()
                lp = self.lane_params.get(lid, {})
                w = float(lane.getWidth())
                length = float(lane.getLength())
                slope = abs(float(lp.get("maxSlope", lp.get("slope", "0.0"))))
                curb_h = float(lp.get("curbHeight", "0.0"))
                lux = float(lp.get("illuminanceLux", "20.0"))
                whl_ok = (
                    (w >= self.min_wheelchair_width_m)
                    and (slope <= self.max_slope)
                    and (curb_h <= self.max_curb_height_m)
                )
                self.graph.add_node(
                    lid,
                    kind="walkingarea",
                    length_m=length,
                    full_length_m=length,
                    wheelchair_ok=whl_ok,
                    tactile=_parse_bool(lp.get("tactilePaving"), default=False),
                    rest_bench=_parse_bool(lp.get("restBench"), default=False),
                    illuminance_lux=lux,
                )

                neighbors = [c.getToLane() for c in lane.getOutgoing()] + list(
                    lane.getIncoming()
                )
                adj_crossings = sorted(
                    {
                        nl.getEdge().getID()
                        for nl in neighbors
                        if nl.getEdge().getFunction() == "crossing"
                    }
                )
                touches_sidewalk = any(
                    nl.getEdge().getFunction() == "" for nl in neighbors
                )
                explicit_refuge = _parse_bool(
                    ep.get("refugeIsland", lp.get("refugeIsland")), default=False
                )
                if explicit_refuge or (
                    len(adj_crossings) >= 2 and not touches_sidewalk
                ):
                    ref_depth = float(lp.get("refugeDepthM", max(w, length)))
                    self.refuge_islands.append(
                        RefugeIslandRecord(
                            walkingarea_id=eid,
                            lane_id=lid,
                            width_m=w,
                            length_m=length,
                            connected_crossings=tuple(adj_crossings),
                            depth_compliant=(ref_depth >= 1.50),
                        )
                    )

        # Connect pedestrian-allowing lanes bidirectionally
        for edge in self.net.getEdges():
            for lane in edge.getLanes():
                u = lane.getID()
                if u not in self.graph:
                    continue
                for conn in lane.getOutgoing():
                    v = conn.getToLane().getID()
                    via = conn.getViaLaneID()
                    if via and via in self.graph and v in self.graph:
                        self._connect(u, via)
                        self._connect(via, v)
                    elif v in self.graph:
                        self._connect(u, v)

    def _connect(self, u: str, v: str) -> None:
        if u == v:
            return
        d_uv = 0.5 * (
            float(self.graph.nodes[u]["length_m"])
            + float(self.graph.nodes[v]["length_m"])
        )
        self.graph.add_edge(u, v, distance_m=d_uv)

    def _edge_generalized_weight(
        self,
        u: str,
        v: str,
        edge_data: Dict[str, Any],
        profile: str,
        night_mode: bool,
        night_cost_hook: Optional[Callable[[str, float, Dict[str, Any]], float]],
    ) -> float:
        base_d = float(edge_data["distance_m"])
        nu = self.graph.nodes[u]
        nv = self.graph.nodes[v]

        mult = 1.0
        if profile == "blind_low_vision":
            # Reward tactile paving guidance; penalize untactile crossings
            both_tactile = bool(nu.get("tactile") or nv.get("tactile"))
            is_crossing = nu.get("kind") == "crossing" or nv.get("kind") == "crossing"
            if both_tactile:
                mult *= 0.78
            elif is_crossing:
                mult *= 1.30
        elif profile in ("older_adult", "wheelchair", "wheelchair_manual"):
            # Rest benches reduce perceived physical exertion on long approaches
            if nu.get("rest_bench") or nv.get("rest_bench"):
                mult *= 0.85

        cost = base_d * mult
        if night_mode:
            lux_u = float(nu.get("illuminance_lux", 20.0))
            lux_v = float(nv.get("illuminance_lux", 20.0))
            avg_lux = 0.5 * (lux_u + lux_v)
            min_lux = min(lux_u, lux_v)
            if night_cost_hook is not None:
                cost = float(
                    night_cost_hook(
                        f"{u}->{v}",
                        cost,
                        {
                            "avg_illuminance_lux": avg_lux,
                            "min_illuminance_lux": min_lux,
                            "u": nu,
                            "v": nv,
                        },
                    )
                )
            elif min_lux < 10.0:
                # Default dark-corridor perceived-safety penalty (EN 13201 E_min < 10 lux)
                cost *= 1.0 + 0.08 * (10.0 - min_lux)
        return max(1e-6, cost)

    def compute_accessible_route(
        self,
        origin_lane: str,
        dest_lane: str,
        profile: str = "wheelchair",
        night_mode: bool = False,
        night_cost_hook: Optional[Callable[[str, float, Dict[str, Any]], float]] = None,
    ) -> AccessibilityRouteResult:
        """Compute an accessible route, returning ``UNREACHABLE_ACCESSIBILITY_BARRIER``
        when curb-step (`> 0.02 m`) or slope (`> 8%`) barriers sever reachability.
        """
        if origin_lane not in self.graph or dest_lane not in self.graph:
            return AccessibilityRouteResult(
                status=UNREACHABLE_ACCESSIBILITY_BARRIER,
                origin_lane=origin_lane,
                dest_lane=dest_lane,
                profile=profile,
                path_lanes=(),
                physical_distance_m=math.inf,
                generalized_cost=math.inf,
                baseline_distance_m=math.inf,
                detour_ratio=math.inf,
            )

        try:
            base_dist = float(
                nx.shortest_path_length(
                    self.graph, origin_lane, dest_lane, weight="distance_m"
                )
            )
        except nx.NetworkXNoPath:
            return AccessibilityRouteResult(
                status=UNREACHABLE_ACCESSIBILITY_BARRIER,
                origin_lane=origin_lane,
                dest_lane=dest_lane,
                profile=profile,
                path_lanes=(),
                physical_distance_m=math.inf,
                generalized_cost=math.inf,
                baseline_distance_m=math.inf,
                detour_ratio=math.inf,
            )

        enforce_wheelchair_barriers = profile in ("wheelchair", "wheelchair_manual")
        allowed_nodes = (
            [
                n
                for n, data in self.graph.nodes(data=True)
                if bool(data.get("wheelchair_ok", True))
            ]
            if enforce_wheelchair_barriers
            else list(self.graph.nodes)
        )
        sub = self.graph.subgraph(allowed_nodes)
        if origin_lane not in sub or dest_lane not in sub:
            return AccessibilityRouteResult(
                status=UNREACHABLE_ACCESSIBILITY_BARRIER,
                origin_lane=origin_lane,
                dest_lane=dest_lane,
                profile=profile,
                path_lanes=(),
                physical_distance_m=math.inf,
                generalized_cost=math.inf,
                baseline_distance_m=base_dist,
                detour_ratio=math.inf,
            )

        def weight_fn(u: str, v: str, d: Dict[str, Any]) -> float:
            return self._edge_generalized_weight(
                u, v, d, profile=profile, night_mode=night_mode, night_cost_hook=night_cost_hook
            )

        try:
            path = nx.shortest_path(sub, origin_lane, dest_lane, weight=weight_fn)
        except nx.NetworkXNoPath:
            return AccessibilityRouteResult(
                status=UNREACHABLE_ACCESSIBILITY_BARRIER,
                origin_lane=origin_lane,
                dest_lane=dest_lane,
                profile=profile,
                path_lanes=(),
                physical_distance_m=math.inf,
                generalized_cost=math.inf,
                baseline_distance_m=base_dist,
                detour_ratio=math.inf,
            )

        phys_dist = sum(
            float(sub.edges[path[k], path[k + 1]]["distance_m"])
            for k in range(len(path) - 1)
        )
        gen_cost = sum(
            weight_fn(path[k], path[k + 1], sub.edges[path[k], path[k + 1]])
            for k in range(len(path) - 1)
        )
        ratio = 1.0 if base_dist <= 1e-6 else max(1.0, phys_dist / base_dist)
        return AccessibilityRouteResult(
            status="OK",
            origin_lane=origin_lane,
            dest_lane=dest_lane,
            profile=profile,
            path_lanes=tuple(path),
            physical_distance_m=phys_dist,
            generalized_cost=gen_cost,
            baseline_distance_m=base_dist,
            detour_ratio=ratio,
        )


def audit_network_accessibility(
    net_file: str | Path,
    max_curb_height_m: float = MAX_WHEELCHAIR_CURB_HEIGHT_M,
    max_slope: float = MAX_WHEELCHAIR_SLOPE,
    min_sidewalk_width_m: float = 1.50,
    night_mode: bool = False,
    night_cost_hook: Optional[Callable[[str, float, Dict[str, Any]], float]] = None,
) -> NetworkAccessibilityReport:
    """Run full static curb, slope, refuge, tactile, bench, and detour audit."""
    router = AccessibilityGraphRouter(
        net_file=net_file,
        max_curb_height_m=max_curb_height_m,
        max_slope=max_slope,
        min_sidewalk_width_m=min_sidewalk_width_m,
    )

    seen: Set[Tuple[str, str]] = set()
    ratios: List[float] = []
    unreachable = 0
    evaluated = 0

    for nid in sorted(router.junction_sidewalks):
        sws = sorted(set(router.junction_sidewalks[nid]))
        for i, src in enumerate(sws):
            for dst in sws[i + 1 :]:
                if (src, dst) in seen:
                    continue
                seen.add((src, dst))
                res = router.compute_accessible_route(
                    src,
                    dst,
                    profile="wheelchair",
                    night_mode=night_mode,
                    night_cost_hook=night_cost_hook,
                )
                if math.isinf(res.baseline_distance_m):
                    continue
                evaluated += 1
                if res.status == UNREACHABLE_ACCESSIBILITY_BARRIER:
                    unreachable += 1
                else:
                    ratios.append(res.detour_ratio)

    n_sw = len(router.sidewalks)
    sw_comp = (
        sum(1 for s in router.sidewalks if s.width_compliant) / n_sw
        if n_sw > 0
        else 1.0
    )
    n_cr = len(router.crossings)
    n_acc_cr = sum(1 for c in router.crossings if c.wheelchair_accessible)
    acc_share = n_acc_cr / n_cr if n_cr > 0 else 1.0

    return NetworkAccessibilityReport(
        net_file=str(net_file),
        sidewalk_count=n_sw,
        sidewalk_compliance_share=sw_comp,
        crossing_count=n_cr,
        accessible_crossing_count=n_acc_cr,
        accessible_crossing_share=acc_share,
        tactile_crossing_count=sum(
            1 for c in router.crossings if c.has_tactile_paving
        ),
        aps_crossing_count=sum(1 for c in router.crossings if c.has_aps_beacon),
        rest_bench_sidewalk_count=sum(
            1 for s in router.sidewalks if s.has_rest_bench
        ),
        refuge_island_count=len(router.refuge_islands),
        evaluated_od_pairs=evaluated,
        unreachable_od_pairs=unreachable,
        barrier_status=(
            UNREACHABLE_ACCESSIBILITY_BARRIER if unreachable > 0 else "OK"
        ),
        mean_detour_ratio=(sum(ratios) / len(ratios) if ratios else 1.0),
        max_detour_ratio=(max(ratios) if ratios else 1.0),
        sidewalks=router.sidewalks,
        crossings=router.crossings,
        refuge_islands=router.refuge_islands,
    )


def get_options(args: Optional[Sequence[str]] = None) -> Any:
    opt_parser = ArgumentParser(
        description="Audit SUMO .net.xml universal-design accessibility and detour ratios."
    )
    opt_parser.add_option(
        "-n", "--net-file", category="input", dest="net_file", required=True, type=ArgumentParser.net_file
    )
    opt_parser.add_option(
        "-o", "--json-output", category="output", dest="json_output", default=None, type=ArgumentParser.file
    )
    opt_parser.add_option(
        "--night", dest="night_mode", action="store_true", default=False, help="Enable night lighting penalty"
    )
    return opt_parser.parse_args(args=args)


def main(args: Optional[Sequence[str]] = None) -> int:
    opts = get_options(args)
    report = audit_network_accessibility(opts.net_file, night_mode=opts.night_mode)
    payload = report.to_dict()
    if opts.json_output:
        Path(opts.json_output).write_text(json.dumps(payload, indent=2), encoding="utf-8")
    else:
        print(json.dumps(payload, indent=2))
    return 0


if __name__ == "__main__":
    sys.exit(main())
