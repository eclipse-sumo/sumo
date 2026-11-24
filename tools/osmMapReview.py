#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License, v. 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

"""Prepare a local OpenStreetMap review report from a SUMO diagnostic."""

from collections import defaultdict
from math import isfinite
from pathlib import Path
import re
import xml.etree.ElementTree as ET
from xml.sax import SAXException

import sumolib


OSM_URL = "https://www.openstreetmap.org"
OSM_ID = re.compile(r"[1-9][0-9]*\Z")
EDGE_ID = re.compile(r"-?([1-9][0-9]*)(?:#[0-9]+)?\Z")
FEATURE_ID = re.compile(r"""\b(edge|lane|junction|way|node)(?:\s+id)?\s+(['"])(.*?)\2""", re.IGNORECASE)
CATEGORIES = {
    "unclassified": "Check the map, the imported network and the simulation configuration.",
    "map-data": "Verify the mapped fact against current, permissible evidence before reporting it.",
    "import": "Check the imported geometry, connections, permissions and netconvert options.",
    "simulation": "Check the demand, routes and simulation settings.",
}


def diagnostic(options):
    if options.log_file is None:
        return None
    with sumolib.openz(str(options.log_file), encoding="utf-8", errors="replace") as log:
        if options.log_line is not None:
            for number, line in enumerate(log, start=1):
                if number == options.log_line:
                    if line.strip():
                        return number, line.strip()
                    raise ValueError("The requested diagnostic line is empty.")
            raise ValueError("The requested line is outside the log.")
        errors, warnings = [], []
        for number, line in enumerate(log, start=1):
            line = line.strip()
            if line.startswith("Error:"):
                errors.append((number, line))
            elif line.startswith("Warning:"):
                warnings.append((number, line))
    candidates = errors or warnings
    if len(candidates) != 1:
        numbers = ", ".join(str(number) for number, _ in candidates[:10])
        raise ValueError("Select one diagnostic with --log-line. Candidate lines: %s." % (numbers or "none"))
    return candidates[0]


def references(options, message):
    selected = {kind: dict.fromkeys(getattr(options, kind)) for kind in ("edge", "lane", "junction", "way", "node")}
    for kind, _, identifier in FEATURE_ID.findall(message):
        selected[kind.lower()][identifier] = None
    return selected


def original_ids(element):
    return element.getParam("origId", element.getParam("origID", "")).split()


def from_network(net, selected):
    origins = defaultdict(set)
    locations = []
    unresolved = []
    for kind in ("edge", "lane", "junction"):
        for identifier in selected[kind]:
            try:
                if net is None:
                    raise KeyError(identifier)
                if kind == "junction":
                    junction = net.getNode(identifier)
                    locations.append(junction.getCoord())
                    junction_origins = original_ids(junction)
                    if junction_origins:
                        for source in junction_origins:
                            if OSM_ID.fullmatch(source):
                                origins["node", source].add("SUMO origId")
                            else:
                                unresolved.append(("origId", source))
                    elif OSM_ID.fullmatch(identifier):
                        origins["node", identifier].add("matching SUMO junction ID")
                    else:
                        unresolved.append((kind, identifier))
                    continue
                lane = net.getLane(identifier) if kind == "lane" else None
                edge = lane.getEdge() if lane is not None else net.getEdge(identifier)
                shape = lane.getShape() if lane is not None else edge.getShape()
                locations.append(sumolib.geomhelper.positionAtShapeOffset(
                    shape, sumolib.geomhelper.polyLength(shape) / 2))
                lanes = [lane] if lane is not None else edge.getLanes()
                source_ids = set(original_ids(edge))
                for source_lane in lanes:
                    source_ids.update(original_ids(source_lane))
                if source_ids:
                    for source in source_ids:
                        if OSM_ID.fullmatch(source):
                            origins["way", source].add("SUMO origId")
                        else:
                            unresolved.append(("origId", source))
                else:
                    way = EDGE_ID.fullmatch(edge.getID())
                    if way:
                        origins["way", way[1]].add("matching SUMO edge ID")
                    else:
                        unresolved.append((kind, identifier))
            except (KeyError, IndexError, ValueError):
                unresolved.append((kind, identifier))
    for kind in ("way", "node"):
        for identifier in selected[kind]:
            if OSM_ID.fullmatch(identifier):
                origins[kind, identifier].add("selected OSM ID")
            else:
                unresolved.append((kind, identifier))
    return origins, locations, unresolved


def read_osm(files, wanted):
    found = {}
    for path in files:
        with sumolib.openz(str(path), mode="rb") as source:
            events = ET.iterparse(source, events=("start", "end"))
            _, root = next(events)
            if root.tag != "osm":
                raise ValueError("%s is not an OpenStreetMap XML extract." % path)
            depth = 1
            for event, element in events:
                if event == "start":
                    depth += 1
                    continue
                if depth == 2:
                    key = element.tag, element.get("id")
                    if key in wanted:
                        version = element.get("version", "")
                        rank = int(version) if version.isdigit() else 0
                        if key not in found or rank > found[key][0]:
                            found[key] = (rank, dict(element.attrib), [nd.get("ref") for nd in element.findall("nd")])
                    root.clear()
                depth -= 1
    return found


def review_location(options, net, locations, found):
    if options.latitude is not None:
        return options.latitude, options.longitude, "the supplied coordinates"
    if len(locations) == 1 and locations[0] is not None:
        try:
            if net.hasGeoProj():
                lon, lat = net.convertXY2LonLat(*locations[0])
                if valid_location(lat, lon):
                    return lat, lon, "imported network geometry"
        except (KeyError, RuntimeError, ValueError):
            pass
    if len(found) == 1:
        (kind, _), (_, attributes, node_ids) = next(iter(found.items()))
        source = "source node coordinates"
        if kind == "way" and node_ids:
            node = ("node", node_ids[len(node_ids) // 2])
            anchor = read_osm(options.osm_file, {node}).get(node)
            attributes = anchor[1] if anchor is not None else {}
            source = "an approximate waypoint on the full source way"
        try:
            lat, lon = float(attributes["lat"]), float(attributes["lon"])
            if valid_location(lat, lon):
                return lat, lon, source
        except (KeyError, ValueError):
            pass
    return None


def valid_location(lat, lon):
    return isfinite(lat) and isfinite(lon) and -90 <= lat <= 90 and -180 <= lon <= 180


def text_block(value):
    return "\n".join("    " + line for line in value.splitlines())


def report(options):
    entry = diagnostic(options)
    selected = references(options, entry[1] if entry else "")
    net = None
    if options.net_file is not None:
        try:
            net = sumolib.net.readNet(str(options.net_file), withInternal=True, withConnections=False, withFoes=False)
        except (OSError, KeyError, IndexError, SyntaxError, ValueError, SAXException) as error:
            raise ValueError("Could not read the SUMO network %s: %s" % (options.net_file, error)) from error
    origins, locations, unresolved = from_network(net, selected)
    found = read_osm(options.osm_file, origins)
    pin = review_location(options, net, list(dict.fromkeys(locations)), found)
    lines = ["# OpenStreetMap review", "", "Review category is **%s**." % options.category, "", CATEGORIES[options.category]]
    if entry:
        lines.extend(["", "## SUMO diagnostic (log line %s)" % entry[0], "", text_block(entry[1])])
    selected_net = ["%s %s" % (kind, identifier) for kind in ("edge", "lane", "junction")
                    for identifier in selected[kind]]
    if selected_net:
        lines.extend(["", "## SUMO network elements", "", text_block("\n".join(selected_net))])
    lines.extend(["", "## Source map features", ""])
    for (kind, identifier), provenance in sorted(origins.items()):
        feature = found.get((kind, identifier))
        name = "%s %s" % (kind, identifier)
        if feature is None:
            lines.append("- %s is absent from the supplied extract (%s)." % (name, ", ".join(sorted(provenance))))
        else:
            attributes = feature[1]
            version = attributes.get("version", "")
            version = version if version.isdigit() else "unavailable"
            lines.append("- [%s](%s/%s/%s), extract version %s (%s)." %
                         (name, OSM_URL, kind, identifier, version, ", ".join(sorted(provenance))))
    if not origins:
        lines.append("No source feature IDs were identified.")
    if unresolved:
        lines.extend(["", "Unresolved identifiers", "",
                      text_block("\n".join("%s %s" % item for item in dict.fromkeys(unresolved)))])
    lines.extend(["", "## Observation", "",
                  text_block(options.observation) if options.observation else "Record the mapped fact that needs review.",
                  "", "## Independent evidence", "",
                  text_block(options.evidence) if options.evidence else "Record what you verified and how.",
                  "", "## Review on OpenStreetMap", ""])
    if pin:
        lat, lon = ("%.6f" % position for position in pin[:2])
        lines.append("[Inspect %s, %s and nearby notes](%s/?mlat=%s&mlon=%s#map=19/%s/%s&layers=N)." %
                     (lat, lon, OSM_URL, lat, lon, lat, lon))
        lines.append("The pin uses %s." % pin[2])
    else:
        lines.append("Supply a location with `--latitude` and `--longitude` to pin the map.")
    lines.extend(["", "Check for duplicate notes. If current evidence confirms a map issue, "
                  "make a manual correction or write a concise note for a mapper.",
                  "", "Source map data © [OpenStreetMap contributors](%s/copyright)." % OSM_URL])
    return "\n".join(lines) + "\n"


def main():
    parser = sumolib.options.ArgumentParser(description=__doc__)
    parser.add_argument("--osm-file", type=Path, action="append", required=True, help="Local OSM XML extract (repeatable)")
    parser.add_argument("--net-file", type=Path, help="Local SUMO network imported from the extract")
    parser.add_argument("--log-file", type=Path, help="Local SUMO or netconvert log")
    parser.add_argument("--log-line", type=int, help="One-based diagnostic line when several errors are present")
    for kind in ("edge", "lane", "junction", "way", "node"):
        parser.add_argument("--" + kind, action="append", default=[], help="Include a %s identifier (repeatable)" % kind)
    parser.add_argument("--category", choices=CATEGORIES, default="unclassified", help="Reviewer's preliminary classification")
    parser.add_argument("--latitude", type=float, help="Latitude for the review pin")
    parser.add_argument("--longitude", type=float, help="Longitude for the review pin")
    parser.add_argument("--observation", help="Your description of the map detail to verify (optional)")
    parser.add_argument("--evidence", help="What you verified and the source you used (optional)")
    parser.add_argument("-o", "--output-file", type=Path, required=True, help="Write the local Markdown report to this file")
    options = parser.parse_args()
    if (options.latitude is None) != (options.longitude is None) or (
            options.latitude is not None and not valid_location(options.latitude, options.longitude)):
        parser.error("Supply both a valid latitude and longitude.")
    if options.log_line is not None and (options.log_line <= 0 or options.log_file is None):
        parser.error("--log-line requires a positive line number and --log-file.")
    if (options.log_file is None and options.latitude is None
            and not any(getattr(options, kind) for kind in ("edge", "lane", "junction", "way", "node"))):
        parser.error("Provide a diagnostic, a feature identifier or a review location.")
    inputs = options.osm_file + [path for path in (options.net_file, options.log_file) if path is not None]
    if any(not path.is_file() for path in inputs):
        parser.error("Each input must be a readable local file.")
    if options.output_file.resolve() in {path.resolve() for path in inputs}:
        parser.error("Choose an output file different from the inputs.")
    try:
        options.output_file.write_text(report(options), encoding="utf-8")
    except (OSError, ValueError, SyntaxError) as error:
        parser.error(str(error))


if __name__ == "__main__":
    main()
