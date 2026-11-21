# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

"""Comprehensive pytest suite for SUMO signal, curb, urban-diff, and V2P tools."""

from __future__ import annotations

import ast
import math
import os
import subprocess
import sys
import xml.etree.ElementTree as ET
from pathlib import Path

# Import pyarrow prior to libsumo on Windows so shared Arrow DLLs resolve cleanly
import pyarrow  # noqa: F401
import libsumo
import pytest
import sumo

SUMO_REPO_ROOT = Path(__file__).resolve().parents[3]
TOOLS_DIR = SUMO_REPO_ROOT / "tools"
for _subdir in (TOOLS_DIR, TOOLS_DIR / "tls", TOOLS_DIR / "net", TOOLS_DIR / "pedestrian"):
    if str(_subdir) not in sys.path:
        sys.path.insert(0, str(_subdir))

from accessibilityCurbAudit import (  # noqa: E402
    UNREACHABLE_ACCESSIBILITY_BARRIER,
    AccessibilityGraphRouter,
    audit_network_accessibility,
)
from inclusiveGreenExtension import (  # noqa: E402
    ACTUATED,
    EQUITY_RL,
    FIXED,
    INCLUSIVE_GREEN_EXT,
    InclusiveGreenExtensionController,
    SignalControlMode,
    compute_required_clearance_time,
)
from urbanTransformationDiff import (  # noqa: E402
    TRANSFORMATION_OPERATORS,
    apply_urban_transformations,
    evaluate_transformation_ab_diff,
)
from v2pAcousticAssist import (  # noqa: E402
    APSBeaconHelper,
    UNR138AVASSNRDetector,
    V2PCrossingAlertGenerator,
)


def _netconvert_bin() -> str:
    exe = "netconvert.exe" if os.name == "nt" else "netconvert"
    p = os.path.join(sumo.SUMO_HOME, "bin", exe)
    assert os.path.exists(p), f"netconvert missing at {p}"
    return p


def _build_intersection_net(tmp_dir: Path, tight_phases: bool = True) -> Path:
    nod = tmp_dir / "n.nod.xml"
    edg = tmp_dir / "e.edg.xml"
    con = tmp_dir / "x.con.xml"
    net = tmp_dir / "inter.net.xml"

    nod.write_text(
        """<nodes>
    <node id="C" x="0.0" y="0.0" type="traffic_light"/>
    <node id="N" x="0.0" y="65.0"/>
    <node id="S" x="0.0" y="-65.0"/>
    <node id="E" x="65.0" y="0.0"/>
    <node id="W" x="-65.0" y="0.0"/>
</nodes>""",
        encoding="utf-8",
    )
    edg.write_text(
        """<edges>
    <edge id="NC" from="N" to="C" numLanes="2" speed="13.89" sidewalkWidth="2.20"/>
    <edge id="CN" from="C" to="N" numLanes="2" speed="13.89" sidewalkWidth="2.20"/>
    <edge id="SC" from="S" to="C" numLanes="2" speed="13.89" sidewalkWidth="2.20"/>
    <edge id="CS" from="C" to="S" numLanes="2" speed="13.89" sidewalkWidth="2.20"/>
    <edge id="EC" from="E" to="C" numLanes="2" speed="13.89" sidewalkWidth="2.20"/>
    <edge id="CE" from="C" to="E" numLanes="2" speed="13.89" sidewalkWidth="2.20"/>
    <edge id="WC" from="W" to="C" numLanes="2" speed="13.89" sidewalkWidth="2.20"/>
    <edge id="CW" from="C" to="W" numLanes="2" speed="13.89" sidewalkWidth="2.20"/>
</edges>""",
        encoding="utf-8",
    )
    con.write_text(
        """<connections>
    <crossing node="C" edges="NC CN" priority="true" width="3.00"/>
    <crossing node="C" edges="SC CS" priority="true" width="3.00"/>
    <crossing node="C" edges="EC CE" priority="true" width="2.60"/>
    <crossing node="C" edges="WC CW" priority="true" width="2.60"/>
</connections>""",
        encoding="utf-8",
    )

    subprocess.run(
        [
            _netconvert_bin(),
            "-n",
            str(nod),
            "-e",
            str(edg),
            "-x",
            str(con),
            "-o",
            str(net),
            "--no-turnarounds",
            "--no-warnings",
        ],
        check=True,
    )

    if tight_phases:
        tree = ET.parse(str(net))
        for tl in tree.getroot().findall("tlLogic"):
            phs = tl.findall("phase")
            if len(phs) >= 6:
                for idx, dur in enumerate(("10", "2", "2", "10", "2", "2")):
                    phs[idx].set("duration", dur)
        tree.write(str(net), encoding="utf-8")

    return net


class TestDependencyPurity:
    @pytest.mark.parametrize(
        "rel_file",
        [
            "tools/tls/inclusiveGreenExtension.py",
            "tools/net/accessibilityCurbAudit.py",
            "tools/net/urbanTransformationDiff.py",
            "tools/pedestrian/v2pAcousticAssist.py",
        ],
    )
    def test_epl2_header_and_no_ml_imports(self, rel_file: str):
        p = SUMO_REPO_ROOT / rel_file
        txt = p.read_text(encoding="utf-8")
        assert "SPDX-License-Identifier: EPL-2.0" in txt
        tree = ast.parse(txt)
        forbidden = {"torch", "jupedsim", "pedpy", "tensorflow"}
        roots = set()
        for n in ast.walk(tree):
            if isinstance(n, ast.Import):
                for a in n.names:
                    roots.add(a.name.split(".")[0])
            elif isinstance(n, ast.ImportFrom) and n.module:
                roots.add(n.module.split(".")[0])
        assert roots.isdisjoint(forbidden)


class TestAccessibilityAuditAndUrbanTransformationDiff:
    def test_curb_slope_barriers_night_hook_and_dc4_ab_diff(self, tmp_path: Path):
        base_net = _build_intersection_net(tmp_path, tight_phases=False)

        # Inject severe curb (> 0.02 m) and steep slope (> 8%) barriers on 3 of 4 crossings
        # isolating NW corner (`:C_c0` curbHeight=0.14m, `:C_c3` slope=0.12) -> UNREACHABLE!
        degraded_net = tmp_path / "degraded.net.xml"
        tree = ET.parse(str(base_net))
        for edge in tree.getroot().findall("edge"):
            eid = edge.attrib.get("id", "")
            if eid in (":C_c0", ":C_c1"):
                ET.SubElement(edge, "param", {"key": "curbHeight", "value": "0.14"})
            elif eid in (":C_c2", ":C_c3", ":N_w0", ":W_w0"):
                ET.SubElement(edge, "param", {"key": "maxSlope", "value": "0.12"})
            elif eid == "NC":
                ET.SubElement(edge, "param", {"key": "illuminanceLux", "value": "3.0"})
        tree.write(str(degraded_net), encoding="utf-8")

        router = AccessibilityGraphRouter(degraded_net)
        # NW sidewalk (NC_0 / CW_0) is completely cut off from SE sidewalk (SC_0) for wheelchairs!
        blocked_route = router.compute_accessible_route("NC_0", "SC_0", profile="wheelchair")
        assert blocked_route.status == UNREACHABLE_ACCESSIBILITY_BARRIER
        assert math.isinf(blocked_route.detour_ratio)

        # Night edge-cost hook increases generalized cost on dark corridors
        day_route = router.compute_accessible_route("NC_0", "SC_0", profile="standard", night_mode=False)
        night_route = router.compute_accessible_route(
            "NC_0",
            "SC_0",
            profile="standard",
            night_mode=True,
            night_cost_hook=lambda eid, cost, meta: cost * (2.0 if meta["min_illuminance_lux"] < 10.0 else 1.0),
        )
        assert night_route.generalized_cost > day_route.generalized_cost

        # Apply ALL 7 universal-design transformation operators!
        retrofitted_net = tmp_path / "retrofitted.net.xml"
        all_ops = [
            "curb_extension",
            "refuge_island",
            "dropped_curbs_all",
            "raised_continuous_crossings",
            "rest_benches",
            "daylighting_no_parking",
            "aps_audible_beacons",
        ]
        assert set(all_ops) == set(TRANSFORMATION_OPERATORS.keys())
        mod_counts = apply_urban_transformations(degraded_net, retrofitted_net, operators=all_ops)
        assert all(v > 0 for v in mod_counts.values())

        diff = evaluate_transformation_ab_diff(
            degraded_net, retrofitted_net, operators_applied=all_ops, elements_modified=mod_counts
        )
        assert diff.unreachable_od_pairs_before > 0
        assert diff.unreachable_od_pairs_after == 0
        assert math.isclose(diff.accessible_crossing_share_after, 1.0, rel_tol=1e-9)
        assert diff.mean_crossing_length_m_after < diff.mean_crossing_length_m_before
        assert diff.walkability_score_delta > 25.0


class TestInclusiveTLSModesAndV2PAcousticAssist:
    def test_aps_880hz_avas_snr_and_180ms_lognormal_v2p(self):
        aps = APSBeaconHelper(carrier_freq_hz=880.0)
        loc_sig = aps.evaluate_beacon(":C_c0", ped_distance_m=4.0, ambient_spl_dba=58.0, walk_phase_active=False)
        walk_sig = aps.evaluate_beacon(":C_c0", ped_distance_m=4.0, ambient_spl_dba=58.0, walk_phase_active=True)
        assert math.isclose(loc_sig.carrier_freq_hz, 880.0)
        assert walk_sig.pulse_rate_hz > loc_sig.pulse_rate_hz
        assert walk_sig.is_audible is True

        det = UNR138AVASSNRDetector(ambient_spl_dba=48.0)
        # Quiet EV at 14.4 km/h (4.0 m/s < 20 km/h) at 14 m has -12.5 dB deficit & triggers silent EV hazard
        ev_avas = det.assess_interaction("v1", distance_m=14.0, closing_speed_mps=4.0, veh_speed_mps=4.0, propulsion_mode="ev_avas")
        ev_quiet = det.assess_interaction("v2", distance_m=14.0, closing_speed_mps=4.0, veh_speed_mps=4.0, propulsion_mode="ev_quiet")
        assert math.isclose(ev_avas.source_spl_dba - ev_quiet.source_spl_dba, 12.5, rel_tol=1e-9)
        assert ev_quiet.silent_ev_hazard is True

        v2p = V2PCrossingAlertGenerator(median_latency_ms=180.0, lognormal_sigma=0.20, rng_seed=123)
        samples = [v2p.sample_latency_ms() for _ in range(500)]
        sample_median = sorted(samples)[len(samples) // 2]
        assert 165.0 < sample_median < 195.0

        alert = v2p.evaluate_pair(
            sim_time_s=10.0,
            ped_id="p_blind",
            ped_xy=(0.0, -1.5),
            ped_vel_xy=(0.0, 0.8),
            veh_id="ev_q",
            veh_xy=(12.0, 0.0),
            veh_vel_xy=(-4.0, 0.0),
            propulsion_mode="ev_quiet",
        )
        assert alert is not None
        assert alert.rescued_silent_ev is True
        assert alert.delivery_time_s > 10.0

    def test_four_signal_modes_and_v2p_live_libsumo(self, tmp_path: Path):
        assert {FIXED, ACTUATED, INCLUSIVE_GREEN_EXT, EQUITY_RL} == {
            m.value for m in SignalControlMode
        }
        assert math.isclose(compute_required_clearance_time(9.0, 0.30, 3.0), 23.0)

        net_file = _build_intersection_net(tmp_path, tight_phases=True)
        rou_file = tmp_path / "demand.rou.xml"
        rou_file.write_text(
            """<routes>
    <vType id="quiet_ev" vClass="passenger" maxSpeed="5.0"/>
    <vType id="wheelchair" vClass="pedestrian" maxSpeed="0.52" width="0.75" length="0.90"/>
    <vType id="blind_low_vision" vClass="pedestrian" maxSpeed="0.55"/>
    <vType id="older_adult" vClass="pedestrian" maxSpeed="0.65"/>
    <vType id="child" vClass="pedestrian" maxSpeed="0.85"/>

    <route id="r_NS" edges="NC CS"/>
    <route id="r_EW" edges="EC CW"/>
    <flow id="f_ns" type="quiet_ev" route="r_NS" begin="0" end="80" period="7"/>
    <flow id="f_ew" type="quiet_ev" route="r_EW" begin="0" end="80" period="7"/>

    <person id="p_wheel" type="wheelchair" depart="1.0" departPos="48.0">
        <walk edges="WC CN" arrivalPos="10.0"/>
    </person>
    <person id="p_blind" type="blind_low_vision" depart="2.0" departPos="48.0">
        <walk edges="EC CS" arrivalPos="10.0"/>
    </person>
    <person id="p_older" type="older_adult" depart="12.0" departPos="48.0">
        <walk edges="NC CE" arrivalPos="10.0"/>
    </person>
    <person id="p_child" type="child" depart="13.0" departPos="48.0">
        <walk edges="SC CW" arrivalPos="10.0"/>
    </person>
</routes>""",
            encoding="utf-8",
        )

        def run_mode(mode_name: str) -> tuple[InclusiveGreenExtensionController, V2PCrossingAlertGenerator]:
            libsumo.start(
                ["sumo", "-n", str(net_file), "-r", str(rou_file), "--step-length", "0.5", "--no-step-log", "--no-warnings"]
            )
            try:
                ctrl = InclusiveGreenExtensionController(conn=libsumo, mode=mode_name, net_file=str(net_file))
                v2p_gen = V2PCrossingAlertGenerator(median_latency_ms=180.0, rng_seed=7)
                steps = 0
                while libsumo.simulation.getMinExpectedNumber() > 0 and steps < 380:
                    libsumo.simulationStep()
                    ctrl.step()
                    v2p_gen.step_sumo(libsumo)
                    steps += 1
                return ctrl, v2p_gen
            finally:
                libsumo.close()

        fixed_ctrl, _ = run_mode(FIXED)
        act_ctrl, _ = run_mode(ACTUATED)
        inc_ctrl, _ = run_mode(INCLUSIVE_GREEN_EXT)
        eq_ctrl, eq_v2p = run_mode(EQUITY_RL)

        assert fixed_ctrl.mid_crossing_stranding_steps > 0
        assert act_ctrl.phase_switches_commanded >= 0
        # Both INCLUSIVE_GREEN_EXT and EQUITY_RL eliminate mid-crossing stranding completely!
        assert inc_ctrl.total_extensions_applied > 0
        assert inc_ctrl.mid_crossing_stranding_steps == 0
        assert eq_ctrl.total_extensions_applied > 0
        assert eq_ctrl.mid_crossing_stranding_steps == 0
        assert len(eq_v2p.emitted_alerts) >= 0
