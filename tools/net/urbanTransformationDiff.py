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

# @file    urbanTransformationDiff.py
# @author  Horizon Europe MSCA STEP-UP Consortium
# @date    2026-11-20

"""
Parametric universal-design street transformation operators and A/B walkability
detour evaluator for SUMO ``.net.xml`` networks (STEP-UP DC4).

Supported operators:
- ``curb_extension``
- ``refuge_island``
- ``dropped_curbs_all``
- ``raised_continuous_crossings``
- ``rest_benches``
- ``daylighting_no_parking``
- ``aps_audible_beacons``
"""

from __future__ import annotations

import dataclasses
import os
import sys
import xml.etree.ElementTree as ET
from pathlib import Path
from typing import Any, Callable, Dict, List, Optional, Sequence, Tuple

# Support both package import (`from tools.net import ...`) and standalone execution
_THIS_DIR = Path(__file__).resolve().parent
if str(_THIS_DIR) not in sys.path:
    sys.path.insert(0, str(_THIS_DIR))

from accessibilityCurbAudit import (  # type: ignore[import-not-found]
    NetworkAccessibilityReport,
    audit_network_accessibility,
)


def _set_xml_param(elem: ET.Element, key: str, value: str) -> None:
    for p in elem.findall("param"):
        if p.attrib.get("key") == key:
            p.set("value", value)
            return
    ET.SubElement(elem, "param", {"key": key, "value": value})


def curb_extension(
    root: ET.Element, bulbout_depth_m: float = 1.80, min_crossing_len_m: float = 3.20
) -> int:
    """Shorten pedestrian crossing lengths via corner curb bulb-outs and widen walkingareas."""
    modified = 0
    for edge in root.findall("edge"):
        func = edge.attrib.get("function", "")
        if func == "crossing":
            _set_xml_param(edge, "curbExtensionDepthM", f"{bulbout_depth_m:.2f}")
            for lane in edge.findall("lane"):
                old_len = float(lane.attrib.get("length", "10.0"))
                new_len = max(min_crossing_len_m, old_len - 2.0 * bulbout_depth_m)
                lane.set("length", f"{new_len:.2f}")
                modified += 1
        elif func == "walkingarea":
            for lane in edge.findall("lane"):
                old_w = float(lane.attrib.get("width", "2.0"))
                lane.set("width", f"{old_w + 0.60:.2f}")
    return modified


def refuge_island(root: ET.Element, island_depth_m: float = 2.00) -> int:
    """Install median pedestrian refuge islands on wide crossings / adjacent walkingareas."""
    modified = 0
    walkingareas = [
        e for e in root.findall("edge") if e.attrib.get("function") == "walkingarea"
    ]
    for edge in root.findall("edge"):
        if edge.attrib.get("function") == "crossing":
            lane = edge.find("lane")
            if lane is not None and float(lane.attrib.get("length", "0.0")) >= 6.0:
                _set_xml_param(edge, "hasMedianRefuge", "true")
                _set_xml_param(edge, "refugeDepthM", f"{island_depth_m:.2f}")
                modified += 1
    if walkingareas:
        # Tag primary junction median/corner island refuge area
        target_wa = walkingareas[0]
        _set_xml_param(target_wa, "refugeIsland", "true")
        _set_xml_param(target_wa, "refugeDepthM", f"{island_depth_m:.2f}")
        modified += 1
    return modified


def dropped_curbs_all(
    root: ET.Element,
    target_curb_height_m: float = 0.005,
    max_ramp_slope: float = 0.05,
) -> int:
    """Retrofit flush dropped curb ramps (``<= 0.02 m``, ``<= 8%``) across all crossings."""
    modified = 0
    for edge in root.findall("edge"):
        if edge.attrib.get("function") in ("crossing", "walkingarea"):
            _set_xml_param(edge, "curbRamp", "true")
            _set_xml_param(edge, "curbHeight", f"{target_curb_height_m:.4f}")
            _set_xml_param(edge, "maxSlope", f"{max_ramp_slope:.3f}")
            for lane in edge.findall("lane"):
                _set_xml_param(lane, "curbRamp", "true")
                _set_xml_param(lane, "curbHeight", f"{target_curb_height_m:.4f}")
                _set_xml_param(lane, "maxSlope", f"{max_ramp_slope:.3f}")
            modified += 1
    return modified


def raised_continuous_crossings(
    root: ET.Element, calmed_speed_mps: float = 5.56
) -> int:
    """Convert crossings into flush raised speed-table crossings and calm approach speeds."""
    modified = 0
    for edge in root.findall("edge"):
        func = edge.attrib.get("function", "")
        if func == "crossing":
            edge.set("priority", "1")
            _set_xml_param(edge, "raisedCrossing", "true")
            _set_xml_param(edge, "curbRamp", "true")
            _set_xml_param(edge, "curbHeight", "0.0000")
            modified += 1
        elif func == "":
            for lane in edge.findall("lane"):
                disallow = lane.attrib.get("disallow", "")
                allow = lane.attrib.get("allow", "")
                is_ped_only = ("pedestrian" in allow) and ("passenger" not in allow)
                if not is_ped_only and "all" not in disallow:
                    old_spd = float(lane.attrib.get("speed", "13.89"))
                    lane.set("speed", f"{min(old_spd, calmed_speed_mps):.2f}")
    return modified


def rest_benches(root: ET.Element, spacing_m: float = 45.0) -> int:
    """Equip pedestrian sidewalks with periodic inclusive rest benches."""
    modified = 0
    for edge in root.findall("edge"):
        if edge.attrib.get("function", "") != "":
            continue
        for lane in edge.findall("lane"):
            allow = lane.attrib.get("allow", "")
            if "pedestrian" in allow or allow == "":
                if float(lane.attrib.get("width", "2.0")) >= 1.50:
                    _set_xml_param(lane, "restBench", "true")
                    _set_xml_param(lane, "benchSpacingM", f"{spacing_m:.1f}")
                    modified += 1
    return modified


def daylighting_no_parking(root: ET.Element, clearance_m: float = 8.0) -> int:
    """Enforce curb daylighting (no parking within ``clearance_m`` of crossings)."""
    modified = 0
    for edge in root.findall("edge"):
        if edge.attrib.get("function") == "crossing":
            _set_xml_param(edge, "daylightingClearanceM", f"{clearance_m:.1f}")
            _set_xml_param(edge, "curbParkingCleared", "true")
            modified += 1
    return modified


def aps_audible_beacons(root: ET.Element, beacon_freq_hz: float = 880.0) -> int:
    """Install 880 Hz Accessible Pedestrian Signal beacons and tactile blisters."""
    modified = 0
    for edge in root.findall("edge"):
        if edge.attrib.get("function") == "crossing":
            _set_xml_param(edge, "apsBeacon", "true")
            _set_xml_param(edge, "apsFreqHz", f"{beacon_freq_hz:.1f}")
            _set_xml_param(edge, "tactilePaving", "true")
            for lane in edge.findall("lane"):
                _set_xml_param(lane, "apsBeacon", "true")
                _set_xml_param(lane, "tactilePaving", "true")
            modified += 1
    return modified


TRANSFORMATION_OPERATORS: Dict[str, Callable[..., int]] = {
    "curb_extension": curb_extension,
    "refuge_island": refuge_island,
    "dropped_curbs_all": dropped_curbs_all,
    "raised_continuous_crossings": raised_continuous_crossings,
    "rest_benches": rest_benches,
    "daylighting_no_parking": daylighting_no_parking,
    "aps_audible_beacons": aps_audible_beacons,
}


@dataclasses.dataclass(frozen=True)
class UrbanTransformationDiffReport:
    operators_applied: Tuple[str, ...]
    elements_modified: Dict[str, int]
    accessible_crossing_share_before: float
    accessible_crossing_share_after: float
    unreachable_od_pairs_before: int
    unreachable_od_pairs_after: int
    mean_detour_ratio_before: float
    mean_detour_ratio_after: float
    mean_crossing_length_m_before: float
    mean_crossing_length_m_after: float
    walkability_score_before: float
    walkability_score_after: float
    walkability_score_delta: float


def _compute_walkability_score(report: NetworkAccessibilityReport) -> float:
    mean_cr_len = (
        sum(c.length_m for c in report.crossings) / len(report.crossings)
        if report.crossings
        else 8.0
    )
    tactile_share = (
        report.tactile_crossing_count / report.crossing_count
        if report.crossing_count > 0
        else 0.0
    )
    bench_share = (
        report.rest_bench_sidewalk_count / report.sidewalk_count
        if report.sidewalk_count > 0
        else 0.0
    )
    detour_penalty = min(25.0, 20.0 * max(0.0, report.mean_detour_ratio - 1.0))
    barrier_penalty = (
        25.0 * (report.unreachable_od_pairs / max(1, report.evaluated_od_pairs))
    )
    exposure_bonus = max(0.0, 15.0 * (1.0 - (mean_cr_len / 18.0)))

    score = (
        35.0 * report.accessible_crossing_share
        + 20.0 * report.sidewalk_compliance_share
        + 15.0 * tactile_share
        + 10.0 * bench_share
        + (5.0 if report.refuge_island_count > 0 else 0.0)
        + exposure_bonus
        - detour_penalty
        - barrier_penalty
    )
    return float(max(0.0, min(100.0, score)))


def apply_urban_transformations(
    input_net_file: str | Path,
    output_net_file: str | Path,
    operators: Sequence[str],
    operator_kwargs: Optional[Dict[str, Dict[str, Any]]] = None,
) -> Dict[str, int]:
    """Apply an ordered sequence of DC4 street-transformation operators to ``input_net_file``."""
    unknown = [op for op in operators if op not in TRANSFORMATION_OPERATORS]
    if unknown:
        raise ValueError(
            f"Unknown transformation operator(s): {unknown}. "
            f"Valid: {sorted(TRANSFORMATION_OPERATORS)}"
        )

    tree = ET.parse(str(input_net_file))
    root = tree.getroot()
    kw_map = operator_kwargs or {}
    counts: Dict[str, int] = {}
    for op_name in operators:
        fn = TRANSFORMATION_OPERATORS[op_name]
        counts[op_name] = int(fn(root, **kw_map.get(op_name, {})))

    tree.write(str(output_net_file), encoding="utf-8")
    return counts


def evaluate_transformation_ab_diff(
    baseline_net_file: str | Path,
    transformed_net_file: str | Path,
    operators_applied: Sequence[str] = (),
    elements_modified: Optional[Dict[str, int]] = None,
) -> UrbanTransformationDiffReport:
    """Compute quantitative universal-design walkability & detour A/B diff."""
    rep_before = audit_network_accessibility(baseline_net_file)
    rep_after = audit_network_accessibility(transformed_net_file)

    len_before = (
        sum(c.length_m for c in rep_before.crossings) / len(rep_before.crossings)
        if rep_before.crossings
        else 0.0
    )
    len_after = (
        sum(c.length_m for c in rep_after.crossings) / len(rep_after.crossings)
        if rep_after.crossings
        else 0.0
    )

    w_before = _compute_walkability_score(rep_before)
    w_after = _compute_walkability_score(rep_after)

    return UrbanTransformationDiffReport(
        operators_applied=tuple(operators_applied),
        elements_modified=dict(elements_modified or {}),
        accessible_crossing_share_before=rep_before.accessible_crossing_share,
        accessible_crossing_share_after=rep_after.accessible_crossing_share,
        unreachable_od_pairs_before=rep_before.unreachable_od_pairs,
        unreachable_od_pairs_after=rep_after.unreachable_od_pairs,
        mean_detour_ratio_before=rep_before.mean_detour_ratio,
        mean_detour_ratio_after=rep_after.mean_detour_ratio,
        mean_crossing_length_m_before=len_before,
        mean_crossing_length_m_after=len_after,
        walkability_score_before=w_before,
        walkability_score_after=w_after,
        walkability_score_delta=w_after - w_before,
    )
