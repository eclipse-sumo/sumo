#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2026-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/ or later under the terms of the
# GNU General Public License, version 2 or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    approveWorkflowRuns.py
# @author  Michael Behrisch
# @date    2026-11-23

"""
Approves the workflow runs of pull requests from whitelisted authors which are waiting for approval
(first time contributors, forks) but only if the number of currently running or queued jobs is low.
Pull requests are approved oldest first (with all their workflow runs) until the number of running jobs
plus the estimated number of jobs of the approved runs would exceed the limit, so this is meant to be run
periodically (cron or a scheduled workflow). The job count of a workflow run is estimated from the latest
successful pull request run of the same workflow.
The token needs the "actions: write" permission (GITHUB_TOKEN or a personal access token).
"""
from __future__ import print_function
import argparse
import json
import os
import sys
import time
import urllib.error
import urllib.request

API = "https://api.github.com"

# maximum number of parallel macOS jobs of a pull request run (one build followed by the test shards)
MACOS_JOBS = {"macos": 3, "macos-wheels": 4}


def parse_args():
    ap = argparse.ArgumentParser(description=__doc__.strip().split("\n")[0])
    ap.add_argument("-r", "--repo", default="eclipse-sumo/sumo", help="repository as owner/name")
    ap.add_argument("-t", "--token", default=os.environ.get("GITHUB_TOKEN"),
                    help="GitHub token (default: $GITHUB_TOKEN)")
    ap.add_argument("-m", "--max-jobs", type=int, default=500,
                    help="concurrent job limit of the plan (all runners except macOS)")
    ap.add_argument("--max-macos-jobs", type=int, default=50, help="concurrent job limit for macOS runners")
    ap.add_argument("--no-rebase", action="store_true", help="do not rebase the pull request before approving")
    ap.add_argument("--rebase-wait", type=int, default=60,
                    help="seconds to wait for the new runs after rebasing")
    ap.add_argument("--threshold", type=float, default=0.8,
                    help="stop approving if running and queued jobs (including the estimate for the "
                         "approved runs) exceed this fraction of a limit")
    ap.add_argument("--default-jobs", type=int, default=10,
                    help="job count to assume for a workflow without a successful pull request run")
    ap.add_argument("-w", "--whitelist", default="",
                    help="comma separated list of GitHub logins whose pull requests may be approved")
    ap.add_argument("--whitelist-file",
                    help="file with one GitHub login per line (lines starting with # are ignored)")
    ap.add_argument("-n", "--dry-run", action="store_true", help="only print what would be done")
    ap.add_argument("-v", "--verbose", action="store_true", help="print more information")
    options = ap.parse_args()
    if not options.token:
        ap.error("a token is needed, use --token or set GITHUB_TOKEN")
    options.allowed = {u.strip().lower() for u in options.whitelist.split(",") if u.strip()}
    if options.whitelist_file:
        with open(options.whitelist_file) as f:
            options.allowed.update(line.strip().lower() for line in f
                                   if line.strip() and not line.startswith("#"))
    if not options.allowed:
        ap.error("the whitelist is empty, use --whitelist or --whitelist-file")
    return options


def request(options, path, method="GET", params=None, body=None):
    url = path if path.startswith("http") else API + path
    if params:
        url += ("&" if "?" in url else "?") + "&".join("%s=%s" % kv for kv in params.items())
    data = json.dumps(body).encode() if body else None
    req = urllib.request.Request(url, data=data, method=method, headers={
        "Authorization": "Bearer " + options.token,
        "Accept": "application/vnd.github+json",
        "X-GitHub-Api-Version": "2022-11-28"})
    with urllib.request.urlopen(req) as resp:
        body = resp.read()
    return json.loads(body) if body else {}


def paginate(options, path, key, params=None):
    page = 1
    while True:
        query = dict(params or {}, per_page=100, page=page)
        items = request(options, path, params=query)
        if key:
            items = items[key]
        for item in items:
            yield item
        if len(items) < 100:
            return
        page += 1


def count_active_jobs(options):
    """returns the number of running and queued jobs as a dict with the keys (pool, status), pool is macos or other"""
    counts = {(pool, status): 0 for pool in ("macos", "other") for status in ("in_progress", "queued")}
    for status in ("in_progress", "queued"):
        for run in paginate(options, "/repos/%s/actions/runs" % options.repo, "workflow_runs", {"status": status}):
            for job in paginate(options, "/repos/%s/actions/runs/%s/jobs" % (options.repo, run["id"]), "jobs"):
                if job["status"] in ("in_progress", "queued"):
                    pool = "macos" if any(label.startswith("macos") for label in job["labels"]) else "other"
                    counts[pool, job["status"]] += 1
    return counts


def waiting_runs(options, sha):
    return list(paginate(options, "/repos/%s/actions/runs" % options.repo, "workflow_runs",
                         {"status": "action_required", "head_sha": sha}))


def rebase(options, pr, runs):
    """rebases the pull request if it is behind its base branch, returns the runs waiting for approval on the new head"""
    sha = pr["head"]["sha"]
    cmp = request(options, "/repos/%s/compare/%s...%s" % (options.repo, pr["base"]["ref"], pr["head"]["label"]))
    if cmp["behind_by"] == 0:
        return runs
    try:
        request(options, "/repos/%s/pulls/%s/update-branch" % (options.repo, pr["number"]), method="PUT",
                body={"expected_head_sha": sha, "update_method": "rebase"})
    except urllib.error.HTTPError as e:
        # e.g. conflicts or no permission to push
        print("  could not rebase PR #%s, not approving: %s" % (pr["number"], e), file=sys.stderr)
        return []
    if options.verbose:
        print("  rebased PR #%s, waiting for the new runs." % pr["number"])
    for _ in range(options.rebase_wait // 5):
        time.sleep(5)
        new_sha = request(options, "/repos/%s/pulls/%s" % (options.repo, pr["number"]))["head"]["sha"]
        if new_sha != sha:
            # the runs are created one by one, so wait until there are as many as before
            new_runs = []
            for _ in range(options.rebase_wait // 5):
                new_runs = waiting_runs(options, new_sha)
                if len(new_runs) >= len(runs):
                    break
                time.sleep(5)
            if new_runs:
                if len(new_runs) < len(runs):
                    print("  only %s of %s runs appeared." % (len(new_runs), len(runs)), file=sys.stderr)
                return new_runs
            break
    print("  no runs waiting for the rebased PR #%s." % pr["number"], file=sys.stderr)
    return []


def main():
    options = parse_args()
    counts = count_active_jobs(options)
    active = {pool: counts[pool, "in_progress"] + counts[pool, "queued"] for pool in ("macos", "other")}
    limits = {"macos": options.threshold * options.max_macos_jobs, "other": options.threshold * options.max_jobs}
    if options.verbose:
        print("%s jobs are running and %s are queued (limit %s)." %
              (counts["other", "in_progress"], counts["other", "queued"], options.max_jobs))
        print("%s macOS jobs are running and %s are queued (limit %s)." %
              (counts["macos", "in_progress"], counts["macos", "queued"], options.max_macos_jobs))
    stopped = any(active[pool] >= limits[pool] for pool in active)
    if stopped and options.verbose:
        print("Stopping: more than %d%% of a limit is in use." % (100 * options.threshold))
    # the runs endpoint only returns the 1000 newest runs of a filtered query, so start from the open pull
    # requests (oldest first, only whitelisted authors) and ask for the runs of their current head commit
    pulls = paginate(options, "/repos/%s/pulls" % options.repo, None,
                     {"state": "open", "sort": "created", "direction": "asc"})
    approved = 0
    waiting = 0
    waiting_allowed = 0
    for pr in pulls:
        if stopped and not options.verbose:
            break
        author = pr["user"]["login"]
        runs = waiting_runs(options, pr["head"]["sha"])
        if options.verbose and not stopped:
            print("PR #%s by %s: %s workflow runs waiting." % (pr["number"], author, len(runs)))
        if not runs:
            continue
        waiting += 1
        if author.lower() not in options.allowed:
            continue
        waiting_allowed += 1
        if stopped:
            continue
        if not options.no_rebase:
            if options.dry_run:
                print("PR #%s: would rebase if behind before approving." % pr["number"])
            else:
                runs = rebase(options, pr, runs)
                if not runs:
                    continue
        # always allow the first pull request, otherwise a large one could be starved
        mac_jobs = sum(MACOS_JOBS.get(run["name"], 0) for run in runs)
        mac_runs = sum(1 for run in runs if run["name"] in MACOS_JOBS)
        estimate = {"macos": mac_jobs, "other": 2 * (len(runs) - mac_runs)}
        if approved and any(active[pool] + estimate[pool] > limits[pool] for pool in active):
            if options.verbose:
                print("Stopping: PR #%s would add about %s jobs (%s on macOS) to the %s active ones (%s on macOS)." %
                      (pr["number"], sum(estimate.values()), estimate["macos"], sum(active.values()), active["macos"]))
            stopped = True
            continue
        if options.verbose:
            print("Approving PR #%s by %s (%s workflow runs, about %s jobs, opened %s)." %
                  (pr["number"], author, len(runs), sum(estimate.values()), pr["created_at"]))
        for run in runs:
            if options.dry_run:
                print("  would approve %s (%s)" % (run["name"], run["html_url"]))
                continue
            try:
                request(options, "/repos/%s/actions/runs/%s/approve" % (options.repo, run["id"]), method="POST")
            except urllib.error.HTTPError as e:
                # e.g. the run has already been approved or cancelled in the meantime
                print("  could not approve %s: %s" % (run["name"], e), file=sys.stderr)
        approved += 1
        for pool in active:
            active[pool] += estimate[pool]
        if any(active[pool] >= limits[pool] for pool in active):
            stopped = True
    if options.verbose:
        print("%s pull requests approved." % approved)
        print("%s pull requests are still waiting for approval (%s of them from whitelisted authors)." %
              (waiting - approved, waiting_allowed - approved))
    return 0


if __name__ == "__main__":
    sys.exit(main())
