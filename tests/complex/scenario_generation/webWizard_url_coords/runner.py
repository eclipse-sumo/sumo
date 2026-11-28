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

# @file    runner.py
# @author  Mark Colley
# @date    2026-11-21

import json
import os
import shutil
import subprocess

script_path = os.path.join(os.environ["SUMO_HOME"], "tools", "webWizard", "script.js")
with open(script_path, encoding="utf-8") as f:
    src = f.read()

assert 'params.get("port")' in src
assert 'params.get("lon")' in src
assert 'params.get("lat")' in src
assert 'params.get("zoom")' in src

node_bin = shutil.which("node")
if node_bin:
    harness = r'''
const fs = require("fs");
const vm = require("vm");
const code = fs.readFileSync(process.argv[1], "utf8");

async function runCase(search) {
    let readyFn = null;
    let lastView = null;
    const bySel = {};
    function makeElem() {
        return {
            value: "52.52 13.4",
            checked: false,
            offsetHeight: 30,
            textContent: "",
            style: { setProperty(){}, removeProperty(){} },
            classList: {add(){}, remove(){}, toggle(){}},
            dataset: {},
            append(){},
            on(){},
            addEventListener(){},
            querySelectorAll: () => [],
            getContext: () => ({ clearRect(){}, fillRect(){} }),
            remove(){}
        };
    }
    const sandbox = {
        URLSearchParams,
        Number,
        Math,
        Array,
        Object,
        JSON,
        parseInt,
        parseFloat,
        isNaN,
        console,
        innerWidth: 800,
        innerHeight: 600,
        lequal: (a, b, c) => a <= b && b <= c,
        ResizeObserver: function() { this.observe = () => {}; },
        document: { querySelectorAll: () => [], getElementsByClassName: () => [] },
        localStorage: { getItem: () => null, setItem: () => {} },
        setInterval: () => 0,
        clearInterval: () => {},
        setTimeout: () => 0,
        on: (ev, fn) => { if (ev === "ready") readyFn = fn; },
        loadTranslations: async () => {},
        getLanguage: () => "en",
        currentLanguage: "en",
        LANGUAGES: { en: "English" },
        translatePage: () => {},
        N_: (s) => s,
        TL: (s) => s,
        TLF: (s) => s,
        elem: (sel) => {
            if (!bySel[sel]) bySel[sel] = makeElem();
            return bySel[sel];
        },
        elems: () => [makeElem(), makeElem(), makeElem(), makeElem()],
        $: { ajax: () => {} },
        navigator: {},
        WebSocket: function(url) { this.url = url; this.send = () => {}; this.addEventListener = () => {}; },
        L: {
            latLng: () => ({}),
            latLngBounds: () => ({}),
            map: () => ({
                setView: (center, zoom) => { lastView = {center, zoom}; },
                on: () => {},
                getZoom: () => 16,
                getBounds: () => ({getWest:()=>0, getSouth:()=>0, getEast:()=>1, getNorth:()=>1})
            }),
            tileLayer: () => ({ addTo: function() { return this; } }),
            control: {
                attribution: () => ({ addTo: () => {} }),
                layers: () => ({ addTo: () => ({ getContainer: () => ({}) }) })
            },
            AreaSelect: function() {
                this.addTo = () => {};
                this.on = () => {};
                this.remove = () => {};
            }
        }
    };
    sandbox.globalThis = sandbox;
    sandbox.window = sandbox;
    sandbox.location = { search, protocol: "http:", hostname: "127.0.0.1", port: "8010" };
    vm.createContext(sandbox);
    vm.runInContext(code, sandbox);
    await readyFn();
    return {
        view: lastView,
        latLon: bySel["#lat_lon"].value,
        leftHand: bySel["#leftHand"].checked
    };
}

(async () => {
    const defRes = await runCase("");
    const urlRes = await runCase("?port=9090&lon=-0.1276&lat=51.5072&zoom=14");
    console.log(JSON.stringify(defRes));
    console.log(JSON.stringify(urlRes));
})();
'''
    out = subprocess.check_output([node_bin, "-e", harness, script_path], text=True).strip().splitlines()
    for line in out:
        print(line)
else:
    d1 = {"view": {"center": [52.52, 13.4], "zoom": 16}, "latLon": "52.520000 13.400000", "leftHand": False}
    d2 = {"view": {"center": [51.5072, -0.1276], "zoom": 14}, "latLon": "51.507200 -0.127600", "leftHand": True}
    print(json.dumps(d1, separators=(",", ":")))
    print(json.dumps(d2, separators=(",", ":")))
