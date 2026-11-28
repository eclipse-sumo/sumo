#!/bin/bash
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2025-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# List the shared Debian/Ubuntu packages for the requested dependency groups.
# A question mark prefix selects a package only when the APT repositories have a candidate.

set -eu

script_dir=$(dirname "$0")
if [[ $# -eq 0 ]]; then
    printf 'Specify at least one group (build or tools).\n' >&2
    exit 2
fi
for group in "$@"; do
    case "$group" in
        build|tools) ;;
        *)
            printf 'Unknown dependency group %s.\n' "$group" >&2
            exit 2
            ;;
    esac
    while IFS= read -r package; do
        case "$package" in
            ""|\#*) continue ;;
            \?*)
                package=${package#?}
                if ! LC_ALL=C apt-cache policy "$package" | grep -Eq '^[[:space:]]*Candidate:[[:space:]]+[^[:space:](]'; then
                    continue
                fi
                ;;
        esac
        printf '%s\n' "$package"
    done < "$script_dir/${group}_req_deb.txt"
done
