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

# @file    v2pAcousticAssist.py
# @author  Horizon Europe MSCA STEP-UP Consortium
# @date    2026-11-20

"""
Acoustic & V2P pedestrian crossing assistance utilities (STEP-UP DC3):
1. ``APSBeaconHelper``: ``880 Hz`` Accessible Pedestrian Signal (ISO 23600 / MUTCD)
   ambient-adaptive locator & walk-phase acoustic guidance model.
2. ``UNR138AVASSNRDetector``: Evaluates UN Regulation No. 138 AVAS compliance vs.
   quiet EV (``< 20 km/h``, ``-12.5 dB`` deficit) auditory masking hazards for
   blind and low-vision pedestrians.
3. ``V2PCrossingAlertGenerator``: ``180 ms`` median LogNormal end-to-end V2P
   (C-V2X / UWB + smartphone haptic/binaural) collision alert generator with
   live ``traci`` / ``libsumo`` co-simulation step hook.
"""

from __future__ import annotations

import dataclasses
import math
import random
from typing import Any, Dict, List, Optional, Tuple


@dataclasses.dataclass(frozen=True)
class APSBeaconSignal:
    crossing_id: str
    carrier_freq_hz: float
    pulse_rate_hz: float
    emitted_spl_dba: float
    received_spl_dba: float
    snr_db: float
    azimuth_error_deg: float
    is_walk_indication: bool
    is_audible: bool


class APSBeaconHelper:
    """ISO 23600 / MUTCD ``880 Hz`` Accessible Pedestrian Signal beacon model."""

    def __init__(
        self,
        carrier_freq_hz: float = 880.0,
        base_spl_at_1m_dba: float = 60.0,
        ambient_gain_offset_db: float = 5.0,
        max_emitted_spl_dba: float = 85.0,
        locator_pulse_hz: float = 2.0,
        walk_pulse_hz: float = 10.0,
        audibility_snr_threshold_db: float = -10.0,
    ) -> None:
        self.carrier_freq_hz = float(carrier_freq_hz)
        self.base_spl_at_1m_dba = float(base_spl_at_1m_dba)
        self.ambient_gain_offset_db = float(ambient_gain_offset_db)
        self.max_emitted_spl_dba = float(max_emitted_spl_dba)
        self.locator_pulse_hz = float(locator_pulse_hz)
        self.walk_pulse_hz = float(walk_pulse_hz)
        self.audibility_snr_threshold_db = float(audibility_snr_threshold_db)

    def evaluate_beacon(
        self,
        crossing_id: str,
        ped_distance_m: float,
        ambient_spl_dba: float = 55.0,
        walk_phase_active: bool = False,
    ) -> APSBeaconSignal:
        emitted = min(
            self.max_emitted_spl_dba,
            max(self.base_spl_at_1m_dba, float(ambient_spl_dba) + self.ambient_gain_offset_db),
        )
        d_eff = max(0.5, float(ped_distance_m))
        recv_spl = emitted - 20.0 * math.log10(d_eff / 1.0)
        snr_db = recv_spl - float(ambient_spl_dba)
        audible = snr_db >= self.audibility_snr_threshold_db
        # Binaural localization precision at 880 Hz improves with positive SNR
        az_err = 3.5 + 14.0 / (1.0 + math.exp((snr_db + 2.0) / 4.0))
        return APSBeaconSignal(
            crossing_id=crossing_id,
            carrier_freq_hz=self.carrier_freq_hz,
            pulse_rate_hz=(
                self.walk_pulse_hz if walk_phase_active else self.locator_pulse_hz
            ),
            emitted_spl_dba=emitted,
            received_spl_dba=recv_spl,
            snr_db=snr_db,
            azimuth_error_deg=az_err,
            is_walk_indication=bool(walk_phase_active),
            is_audible=audible,
        )


@dataclasses.dataclass(frozen=True)
class AVASAuditoryAssessment:
    veh_id: str
    propulsion_mode: str
    speed_mps: float
    distance_m: float
    ttc_s: float
    source_spl_dba: float
    received_spl_dba: float
    snr_db: float
    audibly_detectable: bool
    silent_ev_hazard: bool


class UNR138AVASSNRDetector:
    """Detector for UN R138 AVAS compliance vs. `< 20 km/h` `-12.5 dB` quiet EV hazard."""

    def __init__(
        self,
        ambient_spl_dba: float = 48.0,
        quiet_ev_deficit_db: float = 12.5,
        avas_cutoff_kmh: float = 20.0,
        detection_snr_threshold_db: float = -6.0,
        hazard_ttc_threshold_s: float = 4.8,
    ) -> None:
        self.ambient_spl_dba = float(ambient_spl_dba)
        self.quiet_ev_deficit_db = float(quiet_ev_deficit_db)
        self.avas_cutoff_kmh = float(avas_cutoff_kmh)
        self.detection_snr_threshold_db = float(detection_snr_threshold_db)
        self.hazard_ttc_threshold_s = float(hazard_ttc_threshold_s)

    def source_spl_at_2m(self, speed_mps: float, propulsion_mode: str = "ev_avas") -> float:
        v_kmh = max(0.0, float(speed_mps) * 3.6)
        if v_kmh >= self.avas_cutoff_kmh:
            tire = 56.0 + 30.0 * math.log10(v_kmh / self.avas_cutoff_kmh)
            return max(tire, 58.5) if propulsion_mode == "ice" else tire

        frac = max(0.0, min(1.0, (v_kmh - 5.0) / 15.0))
        avas_spl = 50.0 + 6.0 * frac
        if propulsion_mode == "ev_avas":
            return avas_spl
        if propulsion_mode == "ev_quiet":
            return avas_spl - self.quiet_ev_deficit_db
        return 55.0 + 3.5 * (v_kmh / self.avas_cutoff_kmh)

    def assess_interaction(
        self,
        veh_id: str,
        distance_m: float,
        closing_speed_mps: float,
        veh_speed_mps: float,
        propulsion_mode: str = "ev_avas",
    ) -> AVASAuditoryAssessment:
        src_spl = self.source_spl_at_2m(veh_speed_mps, propulsion_mode=propulsion_mode)
        d_eff = max(2.0, float(distance_m))
        recv_spl = src_spl - 20.0 * math.log10(d_eff / 2.0)
        snr_db = recv_spl - self.ambient_spl_dba
        ttc_s = (
            float(distance_m) / float(closing_speed_mps)
            if closing_speed_mps > 0.1
            else math.inf
        )
        audible = snr_db >= self.detection_snr_threshold_db
        hazard = (ttc_s <= self.hazard_ttc_threshold_s) and (not audible)
        return AVASAuditoryAssessment(
            veh_id=veh_id,
            propulsion_mode=propulsion_mode,
            speed_mps=float(veh_speed_mps),
            distance_m=float(distance_m),
            ttc_s=ttc_s,
            source_spl_dba=src_spl,
            received_spl_dba=recv_spl,
            snr_db=snr_db,
            audibly_detectable=audible,
            silent_ev_hazard=hazard,
        )


@dataclasses.dataclass(frozen=True)
class V2PCrossingAlert:
    ped_id: str
    veh_id: str
    issue_time_s: float
    delivery_latency_ms: float
    delivery_time_s: float
    predicted_ttc_s: float
    modality: str
    rescued_silent_ev: bool


class V2PCrossingAlertGenerator:
    """180 ms median LogNormal V2P collision alert generator for SUMO co-simulation."""

    def __init__(
        self,
        median_latency_ms: float = 180.0,
        lognormal_sigma: float = 0.22,
        alert_ttc_threshold_s: float = 4.5,
        avas_detector: Optional[UNR138AVASSNRDetector] = None,
        rng_seed: Optional[int] = 42,
    ) -> None:
        self.median_latency_ms = float(median_latency_ms)
        self.lognormal_sigma = float(lognormal_sigma)
        self.alert_ttc_threshold_s = float(alert_ttc_threshold_s)
        self.avas_detector = avas_detector or UNR138AVASSNRDetector()
        self._rng = random.Random(rng_seed)
        self.emitted_alerts: List[V2PCrossingAlert] = []
        self._cooldown_until: Dict[Tuple[str, str], float] = {}

    def sample_latency_ms(self) -> float:
        """Sample LogNormal V2P alert delivery latency with exact median ``180.0 ms``."""
        mu_ln = math.log(self.median_latency_ms)
        return float(self._rng.lognormvariate(mu_ln, self.lognormal_sigma))

    def evaluate_pair(
        self,
        sim_time_s: float,
        ped_id: str,
        ped_xy: Tuple[float, float],
        ped_vel_xy: Tuple[float, float],
        veh_id: str,
        veh_xy: Tuple[float, float],
        veh_vel_xy: Tuple[float, float],
        propulsion_mode: str = "ev_quiet",
        apply_protective_stop_conn: Any = None,
    ) -> Optional[V2PCrossingAlert]:
        pair_key = (ped_id, veh_id)
        if sim_time_s < self._cooldown_until.get(pair_key, -math.inf):
            return None

        rx = veh_xy[0] - ped_xy[0]
        ry = veh_xy[1] - ped_xy[1]
        vx = veh_vel_xy[0] - ped_vel_xy[0]
        vy = veh_vel_xy[1] - ped_vel_xy[1]

        dist = math.hypot(rx, ry)
        v_sq = vx * vx + vy * vy
        if dist < 1e-6 or v_sq < 1e-6:
            return None

        r_dot_v = rx * vx + ry * vy
        if r_dot_v >= 0.0:
            # Diverging
            return None

        tau_cpa = -r_dot_v / v_sq
        d_cpa = math.hypot(rx + tau_cpa * vx, ry + tau_cpa * vy)
        if d_cpa > 2.6 or tau_cpa > self.alert_ttc_threshold_s:
            return None

        closing_spd = -r_dot_v / dist
        veh_spd = math.hypot(veh_vel_xy[0], veh_vel_xy[1])
        ac_eval = self.avas_detector.assess_interaction(
            veh_id=veh_id,
            distance_m=dist,
            closing_speed_mps=closing_spd,
            veh_speed_mps=veh_spd,
            propulsion_mode=propulsion_mode,
        )

        latency_ms = self.sample_latency_ms()
        delivery_s = sim_time_s + (latency_ms / 1000.0)
        alert = V2PCrossingAlert(
            ped_id=ped_id,
            veh_id=veh_id,
            issue_time_s=sim_time_s,
            delivery_latency_ms=latency_ms,
            delivery_time_s=delivery_s,
            predicted_ttc_s=tau_cpa,
            modality="binaural_haptic_v2p",
            rescued_silent_ev=ac_eval.silent_ev_hazard,
        )
        self.emitted_alerts.append(alert)
        self._cooldown_until[pair_key] = sim_time_s + 2.0

        # Optional protective vehicle yield trigger in live SUMO co-simulation
        if apply_protective_stop_conn is not None and tau_cpa > (latency_ms / 1000.0):
            try:
                apply_protective_stop_conn.vehicle.slowDown(veh_id, 0.0, max(0.5, tau_cpa * 0.6))
            except Exception:
                pass
        return alert

    def step_sumo(self, conn: Any, apply_protective_braking: bool = False) -> List[V2PCrossingAlert]:
        """Scan active SUMO persons and vehicles in ``conn`` (`traci` or `libsumo`)."""
        sim_t = float(conn.simulation.getTime())
        ped_ids = list(conn.person.getIDList())
        veh_ids = list(conn.vehicle.getIDList())
        step_alerts: List[V2PCrossingAlert] = []

        for pid in ped_ids:
            px, py = conn.person.getPosition(pid)
            pspd = float(conn.person.getSpeed(pid))
            pang_rad = math.radians(90.0 - float(conn.person.getAngle(pid)))
            pvx, pvy = pspd * math.cos(pang_rad), pspd * math.sin(pang_rad)

            for vid in veh_ids:
                vx, vy = conn.vehicle.getPosition(vid)
                if abs(vx - px) > 45.0 or abs(vy - py) > 45.0:
                    continue
                vspd = float(conn.vehicle.getSpeed(vid))
                vang_rad = math.radians(90.0 - float(conn.vehicle.getAngle(vid)))
                vvx, vvy = vspd * math.cos(vang_rad), vspd * math.sin(vang_rad)

                try:
                    pmode = conn.vehicle.getParameter(vid, "acousticMode") or "ev_avas"
                except Exception:
                    pmode = "ev_avas"
                if pmode not in ("ev_avas", "ev_quiet", "ice"):
                    vtype = conn.vehicle.getTypeID(vid).lower()
                    pmode = "ev_quiet" if "quiet" in vtype else "ev_avas"

                al = self.evaluate_pair(
                    sim_time_s=sim_t,
                    ped_id=pid,
                    ped_xy=(float(px), float(py)),
                    ped_vel_xy=(pvx, pvy),
                    veh_id=vid,
                    veh_xy=(float(vx), float(vy)),
                    veh_vel_xy=(vvx, vvy),
                    propulsion_mode=pmode,
                    apply_protective_stop_conn=(
                        conn if apply_protective_braking else None
                    ),
                )
                if al is not None:
                    step_alerts.append(al)
        return step_alerts
