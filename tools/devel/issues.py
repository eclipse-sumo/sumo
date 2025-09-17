#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2010-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    issues.py
# @author  Michael Behrisch
# @date    2025-07-31

import os
import sys

import requests
if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa

BASE_URL = "https://api.github.com"


def get_options():
    op = sumolib.options.ArgumentParser()
    op.add_argument("--repository", default='eclipse-sumo/sumo', help="repository to use")
    op.add_argument("--token", help="use token for authentication")
    op.add_argument("-a", "--assignee", help="filter by assignee")
    op.add_argument("-l", "--label", help="filter by label(s)")
    op.add_argument("-m", "--milestone", help="filter by milestone")
    op.add_argument("-t", "--type", help="filter by type")
    op.add_argument("-A", "--set-assignee", help="set assignee")
    op.add_argument("-L", "--add-label", help="add label")
    op.add_argument("-r", "--remove-label", help="remove label")
    op.add_argument("-M", "--set-milestone", help="set milestone")
    op.add_argument("-T", "--set-type", help="set type")
    op.add_argument("--all", action="store_true", default=False, help="include open and closed issues")
    return op.parse_args()


def get_all(s, url):
    r = s.get(url)
    result = r.json()
    while "Link" in r.headers and 'rel="next"' in r.headers["Link"]:
        r = s.get(r.headers["Link"].split('; rel="next"')[0][1:-1])
        result += r.json()
    return result


def main():
    options = get_options()
    s = requests.Session()
    token = options.token
    if not token:
        for cred_path in (".", os.path.dirname(__file__), os.path.expanduser("~")):
            if os.path.exists(os.path.join(cred_path, ".git-credentials")):
                with open(os.path.join(cred_path, ".git-credentials")) as f:
                    token = f.read().split(":")[-1].split("@")[0]
                    break
    if not token:
        sys.exit("no authentication token found, please use the option --token or provide a .git-credentials file")
    s.headers.update({"Accept": "application/vnd.github+json",
                      "Authorization": f"token {token}",
                      "X-GitHub-Api-Version": "2022-11-28"})
    milestones = {}
    if options.milestone or options.set_milestone:
        result = get_all(s, BASE_URL + "/repos/" + options.repository + "/milestones?state=all&per_page=100")
        milestones = {m["title"]: str(m["number"]) for m in result}
    url = BASE_URL + "/repos/" + options.repository + "/issues?per_page=100"
    if options.assignee:
        url += "&assignee=" + options.assignee
    if options.label:
        url += "&labels=" + options.label
    if options.milestone:
        url += "&milestone=" + milestones.get(options.milestone, options.milestone)
    if options.type:
        url += "&type=" + options.type
    if options.all:
        url += "&state=all"
    for issue in get_all(s, url):
        update = {}
        issue_type = None if issue.get("type") is None else issue["type"].get("name")
        milestone = None if issue.get("milestone") is None else issue["milestone"].get("title")
        if options.set_type and issue_type != options.set_type:
            update["type"] = options.set_type
        if options.set_assignee:
            update["assignee"] = options.set_assignee
        if options.set_milestone:
            update["milestone"] = milestones.get(options.set_milestone, options.set_milestone)
        response = None
        if update:
            response = s.patch(f"{BASE_URL}/repos/{options.repository}/issues/{issue['number']}", json=update)
        if options.add_label:
            response = s.post(f"{BASE_URL}/repos/{options.repository}/issues/{issue['number']}/labels",
                              {"labels": [options.add_label]})
        if options.remove_label:
            response = s.delete(
                f"{BASE_URL}/repos/{options.repository}/issues/{issue['number']}/labels/{options.remove_label}")
        if response:
            print("Updated issue", issue["number"], response)
        else:
            print({"number": issue["number"],
                   "title": issue["title"],
                   "assignees": [assignee["login"] for assignee in issue.get("assignees", [])],
                   "labels": [label["name"] for label in issue.get("labels", [])],
                   "milestone": milestone,
                   "type": issue_type})


if __name__ == "__main__":
    main()
