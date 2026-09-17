// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2012-2026 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

// @file    test_libsumo.js
// @author  Masaya Kataoka
// @date    17.09.2026
//
// Smoke test for the WebAssembly build of libsumo, run it with
//   node test_libsumo.js [path/to/libsumo.js] [path/to/scenario-dir]

'use strict';

const fs = require('fs');
const path = require('path');
const assert = require('assert');

const modulePath = path.resolve(process.argv[2] || path.join(__dirname, '..', '..', 'build-wasm', 'bin', 'libsumo.js'));
const scenarioDir = path.resolve(process.argv[3] || path.join(__dirname, 'example'));

const createLibsumo = require(modulePath);

function checkClose(actual, expected, tolerance, what) {
    assert.ok(Math.abs(actual - expected) <= tolerance,
        `${what}: expected ${expected} +/- ${tolerance} but got ${actual}`);
}

createLibsumo().then((libsumo) => {
    // the simulation reads its input from the in memory file system
    libsumo.FS.mkdir('/scenario');
    for (const name of fs.readdirSync(scenarioDir)) {
        libsumo.FS.writeFile('/scenario/' + name, fs.readFileSync(path.join(scenarioDir, name)));
    }

    const version = libsumo.Simulation.getVersion();
    console.log(`${version.softwareVersion} (TraCI API version ${version.apiVersion})`);

    libsumo.Simulation.load(['-c', '/scenario/example.sumocfg', '--no-step-log', '--xml-validation', 'never']);
    assert.ok(libsumo.Simulation.isLoaded(), 'simulation should be loaded');

    // --- the network is available right after loading ---
    const edges = libsumo.Edge.getIDList();
    assert.ok(Array.isArray(edges), 'getIDList should return a plain array');
    assert.ok(edges.length > 0, 'the network should have edges');
    assert.strictEqual(edges.length, libsumo.Edge.getIDCount());
    console.log(`network: ${edges.length} edges, ${libsumo.Lane.getIDCount()} lanes, ` +
                `${libsumo.Junction.getIDCount()} junctions, ${libsumo.TrafficLight.getIDCount()} traffic lights`);

    const someLane = libsumo.Lane.getIDList()[0];
    assert.ok(libsumo.Lane.getLength(someLane) > 0, 'lanes should have a length');
    const shape = libsumo.Lane.getShape(someLane);
    assert.ok(shape.length >= 2, 'a lane shape needs at least two points');
    assert.strictEqual(typeof shape[0].x, 'number', 'shape points are {x, y, z} objects');

    const boundary = libsumo.Simulation.getNetBoundary();
    assert.strictEqual(boundary.length, 2, 'the net boundary is given by two points');

    // --- run the simulation ---
    const deltaT = libsumo.Simulation.getDeltaT();
    checkClose(deltaT, 1.0, 1e-9, 'default step length');

    let maxRunning = 0;
    let departed = 0;
    let arrived = 0;
    let steps = 0;
    let inspected = false;
    while (libsumo.Simulation.getMinExpectedNumber() > 0 && libsumo.Simulation.getTime() < 300) {
        libsumo.Simulation.step(0);
        steps++;
        departed += libsumo.Simulation.getDepartedIDList().length;
        arrived += libsumo.Simulation.getArrivedIDList().length;
        const running = libsumo.Vehicle.getIDList();
        maxRunning = Math.max(maxRunning, running.length);

        if (!inspected && steps >= 60) {
            // inspect a vehicle which has just departed, so that it is
            // guaranteed to stay in the network for the next few steps
            const fresh = libsumo.Simulation.getDepartedIDList();
            if (fresh.length === 0) {
                continue;
            }
            inspected = true;
            const vehID = fresh[0];
            const position = libsumo.Vehicle.getPosition(vehID);
            assert.strictEqual(typeof position.x, 'number');
            assert.ok(libsumo.Vehicle.getSpeed(vehID) >= 0);
            assert.ok(libsumo.Vehicle.getRoute(vehID).length > 0, 'a vehicle has a route');
            assert.ok(edges.includes(libsumo.Vehicle.getRoadID(vehID)) ||
                      libsumo.Vehicle.getRoadID(vehID).startsWith(':'), 'the vehicle is on a known edge');
            assert.strictEqual(typeof libsumo.Vehicle.getTypeID(vehID), 'string');
            assert.ok(Array.isArray(libsumo.Vehicle.getNextTLS(vehID)));
            const leader = libsumo.Vehicle.getLeader(vehID, 100);
            assert.strictEqual(typeof leader.id, 'string');
            assert.strictEqual(typeof leader.dist, 'number');

            libsumo.Vehicle.setParameter(vehID, 'wasm.test', 'hello');
            assert.strictEqual(libsumo.Vehicle.getParameter(vehID, 'wasm.test'), 'hello');

            // setting values has to become visible in the following steps, the
            // vehicle needs a few of them because setSpeed respects its
            // maximum acceleration and deceleration
            libsumo.Vehicle.setSpeed(vehID, 3.0);
            for (let i = 0; i < 5; i++) {
                libsumo.Simulation.step(0);
                steps++;
            }
            assert.ok(libsumo.Vehicle.getIDList().includes(vehID), 'the inspected vehicle is still running');
            checkClose(libsumo.Vehicle.getSpeed(vehID), 3.0, 1e-6, 'speed set via setSpeed');
            libsumo.Vehicle.setSpeed(vehID, -1);
            console.log(`inspected vehicle ${vehID} of type ${libsumo.Vehicle.getTypeID(vehID)} ` +
                        `on edge ${libsumo.Vehicle.getRoadID(vehID)}`);
        }
    }
    console.log(`simulated ${steps} steps, ${departed} vehicles departed, ${arrived} arrived, ` +
                `at most ${maxRunning} at the same time`);
    assert.ok(departed > 0, 'vehicles should have departed');
    assert.ok(arrived > 0, 'vehicles should have arrived');

    // --- traffic lights ---
    const tlsIDs = libsumo.TrafficLight.getIDList();
    if (tlsIDs.length > 0) {
        const tlsID = tlsIDs[0];
        const state = libsumo.TrafficLight.getRedYellowGreenState(tlsID);
        assert.ok(state.length > 0, 'a traffic light has a state string');
        const logics = libsumo.TrafficLight.getAllProgramLogics(tlsID);
        assert.ok(logics.length > 0 && logics[0].phases.length > 0, 'a traffic light has phases');
        assert.strictEqual(typeof logics[0].phases[0].state, 'string');
        libsumo.TrafficLight.setRedYellowGreenState(tlsID, 'r'.repeat(state.length));
        assert.strictEqual(libsumo.TrafficLight.getRedYellowGreenState(tlsID), 'r'.repeat(state.length));
        console.log(`traffic light ${tlsID}: ${logics[0].phases.length} phases, state ${state}`);
    }

    // --- errors arrive as regular JavaScript errors ---
    assert.throws(() => libsumo.Vehicle.getSpeed('no-such-vehicle'), (error) => {
        assert.ok(error instanceof Error, 'libsumo errors should be Error instances');
        assert.ok(error.message.length > 0, 'libsumo errors should carry a message');
        console.log(`error handling: ${error.name}: ${error.message}`);
        return true;
    });

    // --- writing output works through the in memory file system ---
    libsumo.Simulation.saveState('/scenario/state.xml');
    assert.ok(libsumo.FS.readFile('/scenario/state.xml').length > 0, 'the state file should not be empty');

    libsumo.Simulation.close();
    assert.ok(!libsumo.Simulation.isLoaded(), 'the simulation should be closed');

    console.log('\nall checks passed');
}).catch((error) => {
    console.error(error);
    process.exitCode = 1;
});
