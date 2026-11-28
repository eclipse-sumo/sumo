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

# @file    inclusiveGreenExtension.py
# @author  Mark Colley
# @date    2026-11-20

"""
Multi-mode inclusive pedestrian traffic signal controller supporting:
- ``FIXED``: Passive fixed-time baseline monitoring stranding and delay;
- ``ACTUATED``: Standard vehicular gap-out actuation without pedestrian speed sensing;
- ``INCLUSIVE_GREEN_EXT``: Zero-stranding dynamic pedestrian clearance law
  ``t_req = max(d_rem / max(v_est, 0.45)) + t_buffer``;
- ``EQUITY_RL``: Combines zero-stranding clearance protection with an
  equity-weighted max-pressure / linear RL value phase selector across
  vulnerable road user classes (``wheelchair``, ``older_adult``,
  ``blind_low_vision``, ``child``).
"""

from __future__ import annotations

import dataclasses
from enum import Enum
import math
import sys
from typing import Any, Dict, List, Optional, Sequence, Set, Tuple

import sumolib
from sumolib.options import ArgumentParser


MIN_SPEED_FLOOR_MPS: float = 0.45


class SignalControlMode(str, Enum):
    FIXED = "FIXED"
    ACTUATED = "ACTUATED"
    INCLUSIVE_GREEN_EXT = "INCLUSIVE_GREEN_EXT"
    EQUITY_RL = "EQUITY_RL"


FIXED = SignalControlMode.FIXED.value
ACTUATED = SignalControlMode.ACTUATED.value
INCLUSIVE_GREEN_EXT = SignalControlMode.INCLUSIVE_GREEN_EXT.value
EQUITY_RL = SignalControlMode.EQUITY_RL.value


@dataclasses.dataclass(frozen=True)
class MobilityClassProfile:
    """Signal control parameters for a pedestrian mobility class."""

    name: str
    equity_weight: float
    clearance_buffer_s: float
    nominal_speed_mps: float


VULNERABLE_CLASS_PROFILES: Dict[str, MobilityClassProfile] = {
    "wheelchair": MobilityClassProfile("wheelchair", 2.5, 3.0, 0.75),
    "older_adult": MobilityClassProfile("older_adult", 2.2, 2.5, 0.85),
    "blind_low_vision": MobilityClassProfile("blind_low_vision", 2.6, 3.5, 0.75),
    "child": MobilityClassProfile("child", 2.0, 2.5, 1.00),
    "standard": MobilityClassProfile("standard", 1.0, 1.5, 1.25),
}


def resolve_mobility_class(conn: Any, person_id: str) -> MobilityClassProfile:
    """Resolve the mobility class profile of a SUMO person via parameter or vType."""
    try:
        param_cls = conn.person.getParameter(person_id, "mobilityClass")
    except Exception:
        param_cls = ""
    if param_cls and param_cls in VULNERABLE_CLASS_PROFILES:
        return VULNERABLE_CLASS_PROFILES[param_cls]

    type_id = conn.person.getTypeID(person_id)
    if type_id in VULNERABLE_CLASS_PROFILES:
        return VULNERABLE_CLASS_PROFILES[type_id]
    lowered = type_id.lower()
    for key, profile in VULNERABLE_CLASS_PROFILES.items():
        if key in lowered:
            return profile
    return VULNERABLE_CLASS_PROFILES["standard"]


def compute_required_clearance_time(
    d_rem_m: float,
    v_est_mps: float,
    t_buffer_s: float,
    min_speed_floor: float = MIN_SPEED_FLOOR_MPS,
) -> float:
    """Compute required pedestrian clearance time:

        ``t_req = (d_rem / max(v_est, 0.45)) + t_buffer``
    """
    if d_rem_m <= 0.0:
        return 0.0
    eff_speed = max(float(v_est_mps), float(min_speed_floor))
    return (float(d_rem_m) / eff_speed) + max(0.0, float(t_buffer_s))


class InclusiveGreenExtensionController:
    """Pure TraCI/libsumo traffic signal controller supporting ``FIXED``,
    ``ACTUATED``, ``INCLUSIVE_GREEN_EXT``, and ``EQUITY_RL`` modes.
    """

    def __init__(
        self,
        conn: Any,
        mode: str | SignalControlMode = SignalControlMode.INCLUSIVE_GREEN_EXT,
        net_file: Optional[str] = None,
        tls_ids: Optional[Sequence[str]] = None,
        max_extension_s: float = 45.0,
        min_green_s: float = 6.0,
        extension_trigger_horizon_s: float = 1.5,
        speed_ema_alpha: float = 0.35,
        ped_pressure_scale: float = 2.8,
        wait_urgency_alpha: float = 0.12,
        rl_learning_rate: float = 0.05,
    ) -> None:
        self.conn = conn
        self.mode = SignalControlMode(mode)
        self.net = (
            sumolib.net.readNet(
                str(net_file), withInternal=True, withPedestrianConnections=True
            )
            if net_file is not None
            else None
        )
        self.tls_ids: List[str] = (
            list(tls_ids)
            if tls_ids is not None
            else list(conn.trafficlight.getIDList())
        )
        self.max_extension_s = float(max_extension_s)
        self.min_green_s = float(min_green_s)
        self.extension_trigger_horizon_s = float(extension_trigger_horizon_s)
        self.speed_ema_alpha = float(speed_ema_alpha)
        self.ped_pressure_scale = float(ped_pressure_scale)
        self.wait_urgency_alpha = float(wait_urgency_alpha)
        self.rl_learning_rate = float(rl_learning_rate)
        # Linear RL feature weights [veh_queue, equity_ped_wait, phase_continuity]
        self.rl_weights: List[float] = [1.0, self.ped_pressure_scale, 0.35]

        self._speed_est: Dict[str, float] = {}
        self._prev_crossing_pos: Dict[Tuple[str, str], float] = {}
        self._crossing_dir: Dict[Tuple[str, str], int] = {}
        self._phase_last_switch: Dict[str, float] = {}
        self._phase_extended_accum: Dict[Tuple[str, int], float] = {}
        self._last_phase_idx: Dict[str, int] = {}

        self._tls_crossing_links: Dict[str, Dict[str, List[int]]] = {}
        self._tls_crossing_lengths: Dict[str, Dict[str, float]] = {}
        self._tls_crossing_conflict_links: Dict[str, Dict[str, Set[int]]] = {}
        self._tls_veh_links: Dict[str, Dict[int, str]] = {}

        self.total_extensions_applied: int = 0
        self.total_extended_seconds: float = 0.0
        self.mid_crossing_stranding_steps: int = 0
        self.phase_switches_commanded: int = 0
        self.extension_events: List[Dict[str, Any]] = []

        self._build_tls_topology()

    def _build_tls_topology(self) -> None:
        for tls_id in self.tls_ids:
            controlled_links = self.conn.trafficlight.getControlledLinks(tls_id)
            crossing_links: Dict[str, List[int]] = {}
            crossing_lengths: Dict[str, float] = {}
            veh_links: Dict[int, str] = {}

            for link_idx, link_tuples in enumerate(controlled_links):
                if not link_tuples:
                    continue
                for in_lane, out_lane, _ in link_tuples:
                    if "_c" in out_lane and out_lane.startswith(":"):
                        edge_id = out_lane.rsplit("_", 1)[0]
                        crossing_links.setdefault(edge_id, []).append(link_idx)
                        crossing_lengths[edge_id] = float(
                            self.conn.lane.getLength(out_lane)
                        )
                    elif not in_lane.startswith(":"):
                        veh_links[link_idx] = in_lane

            conflict_map: Dict[str, Set[int]] = {c: set() for c in crossing_links}
            logics = self.conn.trafficlight.getAllProgramLogics(tls_id)
            if logics:
                phases = logics[0].phases
                for c_edge, c_indices in crossing_links.items():
                    always_red_on_ped_green: Optional[Set[int]] = None
                    ever_green_veh: Set[int] = set()
                    for ph in phases:
                        st = ph.state
                        for v_idx in veh_links:
                            if v_idx < len(st) and st[v_idx] in ("G", "g"):
                                ever_green_veh.add(v_idx)
                        if any(
                            idx < len(st) and st[idx] in ("G", "g")
                            for idx in c_indices
                        ):
                            red_now = {
                                v_idx
                                for v_idx in veh_links
                                if v_idx < len(st) and st[v_idx] in ("r", "R")
                            }
                            if always_red_on_ped_green is None:
                                always_red_on_ped_green = red_now
                            else:
                                always_red_on_ped_green &= red_now
                    if always_red_on_ped_green is not None:
                        conflict_map[c_edge] = always_red_on_ped_green & ever_green_veh
                    else:
                        conflict_map[c_edge] = set(veh_links.keys())

            self._tls_crossing_links[tls_id] = crossing_links
            self._tls_crossing_lengths[tls_id] = crossing_lengths
            self._tls_crossing_conflict_links[tls_id] = conflict_map
            self._tls_veh_links[tls_id] = veh_links

    def _estimate_person_speed(
        self, person_id: str, profile: MobilityClassProfile
    ) -> float:
        inst_speed = float(self.conn.person.getSpeed(person_id))
        prev = self._speed_est.get(person_id, profile.nominal_speed_mps)
        if inst_speed > 0.05:
            est = (
                self.speed_ema_alpha * inst_speed
                + (1.0 - self.speed_ema_alpha) * prev
            )
        else:
            est = prev
        self._speed_est[person_id] = est
        return est

    def _estimate_remaining_crossing_distance(
        self, person_id: str, crossing_edge: str, length_m: float
    ) -> float:
        pos = max(0.0, min(length_m, float(self.conn.person.getLanePosition(person_id))))
        key = (person_id, crossing_edge)
        if key in self._prev_crossing_pos:
            delta = pos - self._prev_crossing_pos[key]
            if delta > 1e-3:
                self._crossing_dir[key] = 1
            elif delta < -1e-3:
                self._crossing_dir[key] = -1
        else:
            self._crossing_dir[key] = 1 if pos <= 0.5 * length_m else -1

        self._prev_crossing_pos[key] = pos
        return (length_m - pos) if self._crossing_dir[key] >= 0 else pos

    def is_crossing_exposed_to_conflicting_green(
        self, tls_id: str, crossing_edge: str, state: str
    ) -> bool:
        conflict_links = self._tls_crossing_conflict_links[tls_id].get(
            crossing_edge, set()
        )
        return any(
            idx < len(state) and state[idx] in ("G", "g")
            for idx in conflict_links
        )

    def compute_equity_rl_phase_pressure(
        self, tls_id: str, candidate_phases: Sequence[int], curr_phase: int
    ) -> Tuple[int, Dict[int, float]]:
        """Score candidate green phases under ``EQUITY_RL`` linear Q-pressure."""
        logics = self.conn.trafficlight.getAllProgramLogics(tls_id)
        if not logics or not candidate_phases:
            return curr_phase, {}
        phases = logics[0].phases
        veh_links = self._tls_veh_links[tls_id]
        crossing_links = self._tls_crossing_links[tls_id]

        waiting_peds_by_crossing: Dict[str, List[Tuple[MobilityClassProfile, float]]] = {
            c: [] for c in crossing_links
        }
        for pid in self.conn.person.getIDList():
            if float(self.conn.person.getSpeed(pid)) > 0.25:
                continue
            wait_s = float(self.conn.person.getWaitingTime(pid))
            prof = resolve_mobility_class(self.conn, pid)
            rd = self.conn.person.getRoadID(pid)
            try:
                edges = list(self.conn.person.getEdges(pid))
            except Exception:
                edges = []
            for c_edge in crossing_links:
                if c_edge in edges or rd.startswith(f":{tls_id}_w"):
                    waiting_peds_by_crossing[c_edge].append((prof, wait_s))

        scores: Dict[int, float] = {}
        best_ph = candidate_phases[0]
        best_val = -math.inf
        for ph_idx in candidate_phases:
            st = phases[ph_idx].state
            served_lanes = {
                lid
                for l_idx, lid in veh_links.items()
                if l_idx < len(st) and st[l_idx] in ("G", "g")
            }
            veh_q = sum(
                float(self.conn.lane.getLastStepHaltingNumber(lid))
                + 0.25 * float(self.conn.lane.getLastStepVehicleNumber(lid))
                for lid in served_lanes
            )
            ped_q = 0.0
            for c_edge, c_indices in crossing_links.items():
                if any(idx < len(st) and st[idx] in ("G", "g") for idx in c_indices):
                    for prof, wt in waiting_peds_by_crossing[c_edge]:
                        ped_q += prof.equity_weight * (
                            1.0 + self.wait_urgency_alpha * wt
                        )
            cont_bonus = 1.0 if ph_idx == curr_phase else 0.0
            val = (
                self.rl_weights[0] * veh_q
                + self.rl_weights[1] * ped_q
                + self.rl_weights[2] * cont_bonus
            )
            scores[ph_idx] = val
            if val > best_val:
                best_val = val
                best_ph = ph_idx

        # Online semi-gradient weight adaptation reducing vulnerable pedestrian wait
        total_ped_wait = sum(
            wt for lst in waiting_peds_by_crossing.values() for _, wt in lst
        )
        if total_ped_wait > 15.0:
            self.rl_weights[1] = min(
                8.0, self.rl_weights[1] + self.rl_learning_rate * 0.1
            )
        return int(best_ph), scores

    def step(self) -> Dict[str, Any]:
        """Advance signal controller by one simulation step."""
        sim_time = float(self.conn.simulation.getTime())
        active_persons = list(self.conn.person.getIDList())
        active_set = set(active_persons)

        peds_on_edge: Dict[str, List[str]] = {}
        for pid in active_persons:
            rd = self.conn.person.getRoadID(pid)
            if rd.startswith(":") and "_c" in rd:
                peds_on_edge.setdefault(rd, []).append(pid)

        stale = [
            k
            for k in self._prev_crossing_pos
            if k[0] not in active_set or k[0] not in peds_on_edge.get(k[1], ())
        ]
        for k in stale:
            self._prev_crossing_pos.pop(k, None)
            self._crossing_dir.pop(k, None)

        step_extensions = 0
        for tls_id in self.tls_ids:
            curr_phase = int(self.conn.trafficlight.getPhase(tls_id))
            curr_state = self.conn.trafficlight.getRedYellowGreenState(tls_id)
            next_switch = float(self.conn.trafficlight.getNextSwitch(tls_id))
            rem_phase_s = max(0.0, next_switch - sim_time)

            if self._last_phase_idx.get(tls_id) != curr_phase:
                if tls_id in self._last_phase_idx:
                    self._phase_extended_accum.pop(
                        (tls_id, self._last_phase_idx[tls_id]), None
                    )
                self._last_phase_idx[tls_id] = curr_phase
                self._phase_last_switch[tls_id] = sim_time

            crossing_links = self._tls_crossing_links[tls_id]
            crossing_lengths = self._tls_crossing_lengths[tls_id]

            # Record stranding across ALL modes so FIXED/ACTUATED baselines can be compared
            max_t_req = 0.0
            critical_person: Optional[str] = None
            critical_class: Optional[str] = None

            for c_edge in crossing_links:
                pids = peds_on_edge.get(c_edge, [])
                if not pids:
                    continue
                if self.is_crossing_exposed_to_conflicting_green(
                    tls_id, c_edge, curr_state
                ):
                    self.mid_crossing_stranding_steps += len(pids)
                    continue

                c_len = crossing_lengths[c_edge]
                for pid in pids:
                    prof = resolve_mobility_class(self.conn, pid)
                    v_est = self._estimate_person_speed(pid, prof)
                    d_rem = self._estimate_remaining_crossing_distance(
                        pid, c_edge, c_len
                    )
                    t_req = compute_required_clearance_time(
                        d_rem, v_est, prof.clearance_buffer_s
                    )
                    if t_req > max_t_req:
                        max_t_req = t_req
                        critical_person = pid
                        critical_class = prof.name

            if self.mode == SignalControlMode.FIXED:
                continue

            logics = self.conn.trafficlight.getAllProgramLogics(tls_id)
            phases = logics[0].phases if logics else []

            if self.mode == SignalControlMode.ACTUATED:
                # Vehicle gap-out actuation: terminate green early if zero approaching vehicles
                if (
                    phases
                    and (sim_time - self._phase_last_switch.get(tls_id, 0.0))
                    >= self.min_green_s
                    and rem_phase_s > 2.0
                    and "G" in curr_state
                ):
                    veh_links = self._tls_veh_links[tls_id]
                    served_lanes = {
                        lid
                        for l_idx, lid in veh_links.items()
                        if l_idx < len(curr_state) and curr_state[l_idx] in ("G", "g")
                    }
                    moving_veh = sum(
                        int(self.conn.lane.getLastStepVehicleNumber(lid))
                        for lid in served_lanes
                    )
                    if moving_veh == 0:
                        self.conn.trafficlight.setPhaseDuration(tls_id, 0.5)
                        self.phase_switches_commanded += 1
                continue

            # Modes INCLUSIVE_GREEN_EXT and EQUITY_RL enforce zero-stranding clearance!
            next_phase_exposes = True
            if phases and len(phases) > 1:
                next_ph_state = phases[(curr_phase + 1) % len(phases)].state
                next_phase_exposes = False
                for c_edge in crossing_links:
                    if peds_on_edge.get(c_edge):
                        if (
                            self.is_crossing_exposed_to_conflicting_green(
                                tls_id, c_edge, next_ph_state
                            )
                            or "y" in next_ph_state.lower()
                        ):
                            next_phase_exposes = True
                            break

            if (
                max_t_req > rem_phase_s
                and rem_phase_s <= self.extension_trigger_horizon_s
                and next_phase_exposes
            ):
                already_ext = self._phase_extended_accum.get(
                    (tls_id, curr_phase), 0.0
                )
                budget_rem = max(0.0, self.max_extension_s - already_ext)
                if budget_rem > 1e-3:
                    add_s = min(max_t_req - rem_phase_s, budget_rem)
                    self.conn.trafficlight.setPhaseDuration(
                        tls_id, rem_phase_s + add_s
                    )
                    self._phase_extended_accum[(tls_id, curr_phase)] = (
                        already_ext + add_s
                    )
                    self.total_extensions_applied += 1
                    self.total_extended_seconds += add_s
                    step_extensions += 1
                    self.extension_events.append(
                        {
                            "time_s": sim_time,
                            "tls_id": tls_id,
                            "mode": self.mode.value,
                            "phase": curr_phase,
                            "added_s": add_s,
                            "t_req_s": max_t_req,
                            "critical_person": critical_person,
                            "mobility_class": critical_class,
                        }
                    )
                    continue

            if (
                self.mode == SignalControlMode.EQUITY_RL
                and max_t_req == 0.0
                and phases
                and (sim_time - self._phase_last_switch.get(tls_id, 0.0))
                >= self.min_green_s
                and rem_phase_s <= self.extension_trigger_horizon_s
            ):
                green_candidates = [
                    idx
                    for idx, ph in enumerate(phases)
                    if ("G" in ph.state or "g" in ph.state)
                    and "y" not in ph.state.lower()
                ]
                if len(green_candidates) > 1 and curr_phase in green_candidates:
                    target_ph, _ = self.compute_equity_rl_phase_pressure(
                        tls_id, green_candidates, curr_phase
                    )
                    if target_ph != curr_phase:
                        next_idx = (curr_phase + 1) % len(phases)
                        if "y" in phases[next_idx].state.lower():
                            self.conn.trafficlight.setPhase(tls_id, next_idx)
                        else:
                            self.conn.trafficlight.setPhase(tls_id, target_ph)
                        self._phase_last_switch[tls_id] = sim_time
                        self.phase_switches_commanded += 1

        return {
            "time_s": sim_time,
            "mode": self.mode.value,
            "step_extensions": step_extensions,
            "mid_crossing_stranding_steps": self.mid_crossing_stranding_steps,
        }


def get_options(args: Optional[Sequence[str]] = None) -> Any:
    opt_parser = ArgumentParser(
        description="Run SUMO simulation with inclusive pedestrian TLS control."
    )
    opt_parser.add_option(
        "-c", "--sumocfg", category="input", dest="sumocfg", required=True, help="SUMO .sumocfg file"
    )
    opt_parser.add_option(
        "-m",
        "--mode",
        dest="mode",
        default=INCLUSIVE_GREEN_EXT,
        choices=[m.value for m in SignalControlMode],
        help="Signal control mode",
    )
    opt_parser.add_option(
        "--max-extension", dest="max_extension", type=float, default=45.0, help="Max clearance extension (s)"
    )
    return opt_parser.parse_args(args=args)


def main(args: Optional[Sequence[str]] = None) -> int:
    options = get_options(args)
    import traci as conn

    sumo_bin = sumolib.checkBinary("sumo")
    conn.start([sumo_bin, "-c", options.sumocfg, "--no-step-log"])
    try:
        ctrl = InclusiveGreenExtensionController(
            conn=conn, mode=options.mode, max_extension_s=options.max_extension
        )
        while conn.simulation.getMinExpectedNumber() > 0:
            conn.simulationStep()
            ctrl.step()
    finally:
        conn.close()
    return 0


if __name__ == "__main__":
    sys.exit(main())
