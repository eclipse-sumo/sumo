#!/usr/bin/env python3
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    approveNightlyPR.py
# @date    2026-11-23

"""Release reviewed GitHub Actions runs during the overnight window."""

import argparse
from datetime import datetime, timezone
import json
import os
import re
import subprocess


WORKFLOWS = {".github/workflows/" + name + ".yml" for name in ("linux", "macos", "windows")}
QUEUE_ENTRY = re.compile(r"([1-9][0-9]{0,18})[ \t]+([1-9][0-9]{0,18})")
MAX_RUNS = 2


def api(repo, endpoint, method="GET", allow_missing=False):
    command = ["gh", "api", "--method", method, "-H", "X-GitHub-Api-Version: 2022-11-28",
               "repos/" + repo + "/" + endpoint]
    result = subprocess.run(command, capture_output=True, text=True, encoding="utf-8")
    if result.returncode != 0:
        if allow_missing and "(HTTP 404)" in result.stderr:
            return None
        raise RuntimeError(result.stderr.strip())
    return json.loads(result.stdout) if result.stdout.strip() else None


def eligible(run, pull, base):
    if (run is None or pull is None or pull["state"] != "open" or pull["draft"]
            or pull["base"]["ref"] != base):
        return False
    head = pull["head"]
    head_repo = head["repo"]
    run_repo = run["head_repository"]
    return (run["event"] == "pull_request" and run["conclusion"] == "action_required"
            and run["path"] in WORKFLOWS and head_repo is not None and head_repo["fork"]
            and run_repo is not None and run_repo["id"] == head_repo["id"]
            and run["head_branch"] == head["ref"] and run["head_sha"] == head["sha"]
            and (not run["pull_requests"] or any(item["number"] == pull["number"] for item in run["pull_requests"])))


def parse_queue(queue):
    entries = []
    for line_number, line in enumerate(queue.splitlines(), 1):
        if line.strip():
            match = QUEUE_ENTRY.fullmatch(line.strip())
            if not match:
                raise ValueError(f"Invalid NIGHTLY_PR_CI_QUEUE line {line_number}. Expected a PR number and a run ID.")
            entries.append(match.groups())
    return entries


def process_queue(repo, base, queue, now, approve):
    if approve and not 2 <= now.hour < 5:
        print("The overnight approval window has ended.")
        return
    selected = set()
    for pull_number, run_id in parse_queue(queue):
        if run_id in selected:
            continue
        pull = api(repo, "pulls/" + pull_number, allow_missing=True)
        if pull is None:
            continue
        endpoint = "actions/runs/" + run_id
        run = api(repo, endpoint, allow_missing=True)
        if not eligible(run, pull, base):
            continue
        if approve:
            api(repo, endpoint + "/approve", method="POST")
        selected.add(run_id)
        action = "Approved" if approve else "Would approve"
        print(f"{action} run {run_id} for PR #{pull_number} at {run['head_sha']}.")
        if len(selected) == MAX_RUNS:
            break
    if not selected:
        print("No eligible pull request runs in the queue.")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--approve", action="store_true", help="Approve runs during the overnight UTC window.")
    args = parser.parse_args()
    process_queue(os.environ["GITHUB_REPOSITORY"], os.environ["GITHUB_REF_NAME"], os.environ["NIGHTLY_PR_CI_QUEUE"],
                  datetime.now(timezone.utc), args.approve)
