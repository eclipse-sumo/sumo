#!/usr/bin/env python3
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2024 German Aerospace Center (DLR) and others.
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
import os
import zipfile

import requests


def request(url, token):
    if token:
        return requests.get(url, headers={"Authorization": "Bearer " + token})
    return requests.get(url)


def get_latest_artifact_url(options, artifact_name):
    prefix = "%s/repos/%s/%s/actions/" % (options.api_url, options.owner, options.repository)
    workflow_id = None
    response = request(prefix + "workflows", options.token)
    for workflow in response.json()['workflows']:
        if workflow['name'] == options.workflow:
            workflow_id = workflow['id']
    if workflow_id is None:
        raise RuntimeError("Workflow '%s' not found." % options.workflow)

    workflow_run_id = None
    response = request("%sworkflows/%s/runs" % (prefix, workflow_id), options.token)
    for workflow_run in response.json()['workflow_runs']:
        if (workflow_run['status'] == "completed"
            and workflow_run['conclusion'] == "success"
                and workflow_run['head_branch'] == options.branch):
            workflow_run_id = workflow_run['id']
            break
    if workflow_run_id is None:
        raise RuntimeError("No successful workflow run found in branch '%s'." % options.branch)

    artifact_id = None
    response = request("%sruns/%s/artifacts" % (prefix, workflow_run_id), options.token)
    for artifact in response.json()['artifacts']:
        if artifact['name'] == artifact_name:
            artifact_id = artifact['id']
    if artifact_id:
        return "%sartifacts/%s/zip" % (prefix, artifact_id)


if __name__ == "__main__":
    ap = argparse.ArgumentParser()
    ap.add_argument("--api-url", default="https://api.github.com")
    ap.add_argument("--owner", default="eclipse-sumo")
    ap.add_argument("--repository", default="sumo")
    ap.add_argument("--workflow", default="windows-wheels")
    ap.add_argument("--branch", default="main")
    ap.add_argument("--token", help="GitHub authentication token")
    ap.add_argument("--directory", help="output directory")
    ap.add_argument("-v", "--verbose", action="store_true", default=False, help="tell me more")
    options = ap.parse_args()

    for minor in range(7, 13):
        artifact_url = get_latest_artifact_url(options, "libsumo-python-3.%s-wheels" % minor)
        response = request(artifact_url, options.token)
        if response.status_code == 200:
            with open("wheels.zip", "wb") as w:
                w.write(response.content)
            zip = zipfile.ZipFile(w.name)
            zip.extractall(options.directory)
            os.remove(w.name)
        if options.verbose:
            print(artifact_url, response)
