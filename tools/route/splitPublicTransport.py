#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2026-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    splitPublicTransport.py
# @author  Mark Colley
# @date    2026-04-12

"""
Separate public transport vehicles/flows, individual transport vehicles/flows/trips,
and persons/personFlows (along with their triggered/ridden private vehicles) into
distinct route files.
"""
from __future__ import absolute_import
from __future__ import print_function

import os
import sys

SUMO_HOME = os.environ.get('SUMO_HOME',
                           os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
sys.path.append(os.path.join(SUMO_HOME, 'tools'))
import sumolib  # noqa

DEFAULT_PT_VCLASSES = "bus,tram,rail,rail_urban,rail_electric,rail_fast,subway,ship,cable_car"


def get_options(args=None):
    ap = sumolib.options.ArgumentParser(
        description="Separate public transport, individual transport, and persons into distinct route files")
    ap.add_argument("-r", "--route-files", dest="routefiles", category="input",
                    type=ap.route_file_list, required=True,
                    help="comma-separated input route or vehroute XML file(s)")
    ap.add_argument("--pt-output", dest="ptOutput", category="output",
                    type=ap.route_file,
                    help="output file for public transport vehicles/flows")
    ap.add_argument("--individual-output", dest="indOutput", category="output",
                    type=ap.route_file,
                    help="output file for individual transport vehicles/flows/trips")
    ap.add_argument("--persons-output", dest="personsOutput", category="output",
                    type=ap.route_file,
                    help="output file for persons/personFlows (and their private ridden vehicles)")
    ap.add_argument("--keep-triggered-with-persons", dest="triggeredWithPersons",
                    action="store_true", default=True, category="processing",
                    help="keep triggered or person-ridden private vehicles in --persons-output (default)")
    ap.add_argument("--separate-triggered", dest="triggeredWithPersons",
                    action="store_false", category="processing",
                    help="place triggered or person-ridden private vehicles in --individual-output instead")
    ap.add_argument("--restore-triggered", dest="restoreTriggered",
                    action="store_true", default=False, category="processing",
                    help="set depart='triggered' on private person-ridden vehicles from vehroutes")
    ap.add_argument("--pt-vclasses", dest="ptVClasses", default=DEFAULT_PT_VCLASSES,
                    category="processing",
                    help="comma-separated vClasses classified as public transport")
    ap.add_argument("--pt-lines-only", dest="ptLinesOnly", action="store_true",
                    default=False, category="processing",
                    help="only classify vehicles/flows with a 'line' attribute or PT stop as PT")
    ap.add_argument("-v", "--verbose", action="store_true", default=False,
                    help="tell me what you are doing")
    options = ap.parse_args(args=args)

    if not options.ptOutput and not options.indOutput and not options.personsOutput:
        ap.print_help()
        sys.exit("Error! At least one of --pt-output, --individual-output, or --persons-output must be given.")

    options.routeFileList = [f.strip() for f in options.routefiles.split(",") if f.strip()]
    options.ptVClassSet = set(c.strip() for c in options.ptVClasses.split(",") if c.strip())
    return options


def get_attr_str(elem, attr_name):
    val = getattr(elem, attr_name, None)
    if isinstance(val, str):
        return val
    return elem.getAttributeSecure("attr_" + attr_name, None)


def has_pt_stop(elem, standalone_routes):
    stops = list(elem.stop) if isinstance(elem.stop, list) else []
    if isinstance(elem.route, list) and elem.route:
        r = elem.route[0]
        if isinstance(r.stop, list):
            stops.extend(r.stop)
    else:
        r_id = get_attr_str(elem, "route")
        if r_id and r_id in standalone_routes:
            r_elem = standalone_routes[r_id]
            if isinstance(r_elem.stop, list):
                stops.extend(r_elem.stop)
    for st in stops:
        if get_attr_str(st, "busStop") or get_attr_str(st, "trainStop"):
            return True
    return False


def is_pt_vehicle(elem, vtypes, standalone_routes, options):
    line = get_attr_str(elem, "line")
    if line and line != "taxi":
        return True
    depart = get_attr_str(elem, "depart")
    if depart not in ("triggered", "containerTriggered") and has_pt_stop(elem, standalone_routes):
        return True
    if not options.ptLinesOnly:
        vtype_id = get_attr_str(elem, "type")
        if vtype_id and vtype_id in vtypes:
            vclass = get_attr_str(vtypes[vtype_id], "vClass")
            if vclass in options.ptVClassSet:
                return True
    return False


def collect_person_referenced_ids(person):
    refs = []
    for child in person.getChildList():
        if child.name == "ride":
            veh = get_attr_str(child, "vehicle")
            if veh:
                refs.append(veh)
            lines = get_attr_str(child, "lines")
            if lines:
                for tok in lines.split():
                    if tok != "ANY":
                        refs.append(tok)
    return refs


def write_bucket_file(filepath, elements, vtypes, vtype_dists, standalone_routes, options):
    used_types = set()
    used_routes = set()
    for elem in elements:
        if elem.name in ("vehicle", "trip", "flow", "person", "personFlow", "container", "containerFlow"):
            t = get_attr_str(elem, "type")
            if t:
                used_types.add(t)
            r = get_attr_str(elem, "route")
            if r:
                used_routes.add(r)

    with sumolib.openz(filepath, "w") as outf:
        sumolib.writeXMLHeader(outf, "$Id$", "routes", schemaPath="routes_file.xsd", options=options)
        for vt_id, vt_elem in vtypes.items():
            if vt_id in used_types:
                outf.write(vt_elem.toXML(initialIndent="    "))
        for vtd_id, vtd_elem in vtype_dists.items():
            if vtd_id in used_types:
                outf.write(vtd_elem.toXML(initialIndent="    "))
        for r_id, r_elem in standalone_routes.items():
            if r_id in used_routes:
                outf.write(r_elem.toXML(initialIndent="    "))
        for elem in elements:
            outf.write(elem.toXML(initialIndent="    "))
        outf.write("</routes>\n")


def main(options):
    vtypes = {}
    vtype_dists = {}
    standalone_routes = {}
    all_items = []

    for rfile in options.routeFileList:
        if options.verbose:
            print("Reading '%s'" % rfile)
        for root in sumolib.xml.parse(rfile, ["routes", "alternatives"]):
            for elem in root.getChildList():
                if elem.name == "vType":
                    vtypes[elem.id] = elem
                elif elem.name == "vTypeDistribution":
                    vtype_dists[elem.id] = elem
                elif elem.name in ("route", "routeDistribution"):
                    if elem.id:
                        standalone_routes[elem.id] = elem
                elif elem.name in ("vehicle", "trip", "flow",
                                   "person", "personFlow", "container", "containerFlow"):
                    all_items.append(elem)

    veh_elems = {}
    pt_ids = set()
    for elem in all_items:
        if elem.name in ("vehicle", "trip", "flow"):
            veh_elems[elem.id] = elem
            if is_pt_vehicle(elem, vtypes, standalone_routes, options):
                pt_ids.add(elem.id)

    person_private_veh_map = {}
    all_private_ridden_ids = set()
    for elem in all_items:
        if elem.name in ("person", "personFlow"):
            priv = []
            for ref_id in collect_person_referenced_ids(elem):
                if ref_id in veh_elems and ref_id not in pt_ids and ref_id not in all_private_ridden_ids:
                    priv.append(ref_id)
                    all_private_ridden_ids.add(ref_id)
            person_private_veh_map[elem.id] = priv

    pt_bucket = []
    ind_bucket = []
    person_bucket = []
    emitted_with_person = set()

    for elem in all_items:
        if elem.name in ("vehicle", "trip", "flow"):
            vid = elem.id
            if vid in pt_ids:
                pt_bucket.append(elem)
            else:
                dep = get_attr_str(elem, "depart")
                is_trig = dep in ("triggered", "containerTriggered") or vid in all_private_ridden_ids
                if is_trig and options.triggeredWithPersons:
                    if vid not in all_private_ridden_ids:
                        person_bucket.append(elem)
                        emitted_with_person.add(vid)
                else:
                    ind_bucket.append(elem)
        elif elem.name in ("person", "personFlow"):
            if options.triggeredWithPersons:
                for priv_vid in person_private_veh_map.get(elem.id, []):
                    if priv_vid not in emitted_with_person:
                        v_elem = veh_elems[priv_vid]
                        if options.restoreTriggered and v_elem.hasAttribute("depart"):
                            v_elem.setAttribute("depart", "triggered")
                        person_bucket.append(v_elem)
                        emitted_with_person.add(priv_vid)
            person_bucket.append(elem)
        else:
            person_bucket.append(elem)

    if options.ptOutput:
        write_bucket_file(options.ptOutput, pt_bucket, vtypes, vtype_dists, standalone_routes, options)
    if options.indOutput:
        write_bucket_file(options.indOutput, ind_bucket, vtypes, vtype_dists, standalone_routes, options)
    if options.personsOutput:
        write_bucket_file(options.personsOutput, person_bucket, vtypes, vtype_dists, standalone_routes, options)

    if options.verbose:
        print("Separated %d PT elements, %d individual elements, %d person/triggered elements" % (
            len(pt_bucket), len(ind_bucket), len(person_bucket)))


if __name__ == "__main__":
    main(get_options())
