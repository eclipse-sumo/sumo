#!/usr/bin/env python3
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    download_artifact.py
# @author  Michael Behrisch
# @date    2024-05-03

import argparse
import io
import zipfile

import requests


def request(url, token):
    if token:
        return requests.get(url, headers={"Authorization": "Bearer " + token})
    return requests.get(url)


def get_latest_artifact_url(options):
    prefix = "%s/repos/%s/%s/actions/" % (options.api_url, options.owner, options.repository)
    workflow_id = None
    response = request(prefix + "workflows", options.token)
    for workflow in response.json()['workflows']:
        if workflow['name'] == options.workflow:
            workflow_id = workflow['id']
    if workflow_id is None:
        raise RuntimeError("Workflow '%s' not found." % options.workflow)

    workflow_run_ids = []
    response = request("%sworkflows/%s/runs" % (prefix, workflow_id), options.token)
    for workflow_run in response.json()['workflow_runs']:
        if options.branch == "main" and workflow_run['head_branch'][:1] == "v":
            # there seems to be no easy way to identify a tag so we take the first letter
            workflow_run_ids.append(workflow_run['id'])
    for workflow_run in response.json()['workflow_runs']:
        if (workflow_run['status'] == "completed"
            and (options.allow_failed or workflow_run['conclusion'] == "success")
                and workflow_run['head_branch'] == options.branch):
            workflow_run_ids.append(workflow_run['id'])
            break
    if not workflow_run_ids:
        raise RuntimeError("No successful workflow run found in branch '%s'." % options.branch)

    for workflow_run_id in workflow_run_ids:
        response = request("%sruns/%s/artifacts" % (prefix, workflow_run_id), options.token)
        for artifact in response.json()['artifacts']:
            if artifact['name'].startswith(options.prefix):
                yield "%sartifacts/%s/zip" % (prefix, artifact['id'])


if __name__ == "__main__":
    ap = argparse.ArgumentParser()
    ap.add_argument("--api-url", default="https://api.github.com")
    ap.add_argument("--owner", default="eclipse-sumo")
    ap.add_argument("--repository", default="sumo")
    ap.add_argument("--workflow", default="windows-wheels")
    ap.add_argument("--branch", default="main")
    ap.add_argument("--token", help="GitHub authentication token")
    ap.add_argument("--directory", help="output directory")
    ap.add_argument("--prefix", default="libsumo-python-3.", help="prefix of the artifact zip file")
    ap.add_argument("--allow-failed", action="store_true", default=False, help="download even if the build failed")
    ap.add_argument("-v", "--verbose", action="store_true", default=False, help="tell me more")
    options = ap.parse_args()

    for artifact_url in get_latest_artifact_url(options):
        response = request(artifact_url, options.token)
        if response.status_code == 200:
            with zipfile.ZipFile(io.BytesIO(response.content)) as zip:
                zip.extractall(options.directory)
        if options.verbose:
            print(artifact_url, response)
