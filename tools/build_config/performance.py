#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2011-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    performance.py
# @author  Michael Behrisch
# @date    2023-10-02

"""Measure SUMO performance for a current binary or a set of archived versions."""

import glob
import json
import math
import os
from pathlib import Path
import platform
import shutil
import statistics
import subprocess
import sys
from tempfile import TemporaryDirectory
import time
from xml.etree import ElementTree as ET

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import sumolib  # noqa


STANDARD_CONFIGS = (
    ("quickstart", "quickstart.sumocfg"),
    ("public_transport", "run.sumocfg"),
    ("sumolympics", "sumolympics.sumocfg"),
)


def run_current_binary(options, binary):
    tutorials = Path(__file__).resolve().parents[2] / "tests/complex/tutorial"
    configurations = [(Path(cfg).name, Path(cfg).resolve()) for cfg in options.cfg]
    if not configurations:
        configurations = [(name, tutorials / name / "data" / cfg) for name, cfg in STANDARD_CONFIGS]

    report = {
        "version": subprocess.check_output([binary, "--version"], text=True).splitlines()[0],
        "revision": os.environ.get("GITHUB_SHA"),
        "platform": platform.platform(),
        "cpu_count": os.cpu_count(),
        "scenarios": [],
    }
    with TemporaryDirectory(prefix="sumo-performance-") as directory:
        statistics_file = Path(directory) / "statistics.xml"
        for name, config in configurations:
            runs = []
            for _ in range(options.repeat):
                statistics_file.unlink(missing_ok=True)
                start = time.perf_counter()
                subprocess.run([binary, "-c", config, "--no-step-log", "--statistic-output", statistics_file],
                               cwd=directory, stdout=subprocess.DEVNULL, check=True, timeout=options.timeout)
                wall_time = time.perf_counter() - start
                result = ET.parse(statistics_file).getroot()
                performance = result.find("performance")
                runs.append({
                    "wall_seconds": round(wall_time, 6),
                    "simulation_seconds": float(performance.get("duration")),
                    "vehicle_updates_per_second": float(performance.get("vehicleUpdatesPerSecond")),
                    "person_updates_per_second": float(performance.get("personUpdatesPerSecond")),
                    "vehicles_inserted": int(result.find("vehicles").get("inserted")),
                    "persons_loaded": int(result.find("persons").get("loaded")),
                })
            median = statistics.median(row["wall_seconds"] for row in runs)
            report["scenarios"].append({"name": name, "median_wall_seconds": median, "runs": runs})
            print(f"{name} {median:.3f} s median wall time ({options.repeat} runs)", flush=True)

    if options.report:
        options.report.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")


def run_archived_versions(options):
    import psutil

    versions = []
    for sumo in glob.glob(os.path.join(options.version_dir, "bin*", "sumo")):
        versions.append(([int(d) for d in sumo[len(options.version_dir) + 5:-5].split("_")], sumo))
    versions.sort()

    for config in options.cfg:
        for version, sumo in versions:
            times = []
            mems = []
            for _ in range(options.repeat):
                p = subprocess.Popen([sumo, config], stdout=subprocess.DEVNULL)
                utime = 0
                mem = 0
                try:
                    ps = psutil.Process(p.pid)
                    while p.poll() is None:
                        with ps.oneshot():
                            utime = ps.cpu_times().user
                            mem = max(mem, ps.memory_full_info().uss)
                        time.sleep(0.1)
                except psutil.NoSuchProcess:
                    pass
                if p.wait() != 0:
                    raise subprocess.CalledProcessError(p.returncode, p.args)
                times.append(utime)
                mems.append(mem)
            times.sort()
            mems.sort()
            num = options.repeat
            if num > 4:
                times = times[1:-1]
                mems = mems[1:-1]
                num -= 2
            print(".".join(map(str, version)), "%.2f" % (sum(times) / num), "%.2f" % (sum(mems) / num / 2 ** 20))


def main():
    parser = sumolib.options.ArgumentParser()
    parser.add_argument("cfg", nargs="*", help="SUMO configurations to run")
    parser.add_argument("-d", "--version-dir", default=".history", help="where to find archived SUMO versions")
    parser.add_argument("-r", "--repeat", type=int, default=5, help="number of runs per scenario")
    parser.add_argument("--binary", help="benchmark this SUMO binary using the standard suite or specified configs")
    parser.add_argument("--report", type=Path, help="write current-binary measurements to a JSON file")
    parser.add_argument("--timeout", type=float, default=120, help="timeout per current-binary run in seconds")
    options = parser.parse_args()
    if options.repeat < 1 or not math.isfinite(options.timeout) or options.timeout <= 0:
        parser.error("repeat and timeout must be finite and positive")
    if options.binary:
        binary = shutil.which(options.binary)
        if not binary:
            parser.error(f"could not find SUMO binary {options.binary}")
        run_current_binary(options, Path(binary).resolve())
    else:
        if options.report or not options.cfg:
            parser.error("provide SUMO configurations for archived versions, or use --binary")
        run_archived_versions(options)


if __name__ == "__main__":
    main()
