---
title: Libsumo WebAssembly
---

# Libsumo as a WebAssembly module

[Libsumo](Libsumo.md) can be compiled to
[WebAssembly](https://webassembly.org/) with
[Emscripten](https://emscripten.org/). The result is a pair of files,
`libsumo.js` and `libsumo.wasm`, which run a complete SUMO simulation inside a
browser tab, a web worker or Node.js. No server, no native binary and no
installation of SUMO are involved, which makes it useful for teaching material,
online demonstrators and for embedding a simulation into an existing web
application.

!!! note
    This is a build variant of libsumo, not a separate library. The simulation
    core is exactly the same code as in [sumo](sumo.md), only the packaging and
    the language bindings differ.

## Building

The build scripts live in
[{{SUMO}}/build_config/wasm]({{Source}}build_config/wasm), the detailed
instructions are in the
[README]({{Source}}build_config/wasm/README.md) next to them. In short:

```bash
source /path/to/emsdk/emsdk_env.sh
build_config/wasm/build_dependencies.sh ~/sumo-wasm-deps  # Xerces-C and zlib
build_config/wasm/build.sh ~/sumo-wasm-deps               # libsumo.js / libsumo.wasm
```

The first script cross compiles Xerces-C and zlib, the second configures SUMO
with the Emscripten toolchain file and builds the `libsumojs` target into
`build-wasm/bin`. The README also documents the plain `emcmake cmake`
invocation for those who prefer to drive the build themselves.

## Usage

The generated `libsumo.js` exports a factory function which resolves to the
module once the WebAssembly has been instantiated. Input files are placed in
the in-memory file system of the module, afterwards the API is used just like
the Python or C++ one:

```js
const libsumo = await createLibsumo();

libsumo.FS.mkdir('/scenario');
libsumo.FS.writeFile('/scenario/net.net.xml', await (await fetch('net.net.xml')).text());
libsumo.FS.writeFile('/scenario/routes.rou.xml', await (await fetch('routes.rou.xml')).text());

libsumo.Simulation.load(['-n', '/scenario/net.net.xml', '-r', '/scenario/routes.rou.xml']);
while (libsumo.Simulation.getMinExpectedNumber() > 0) {
    libsumo.Simulation.step(0);
    for (const vehID of libsumo.Vehicle.getIDList()) {
        const {x, y} = libsumo.Vehicle.getPosition(vehID);
        console.log(vehID, x, y, libsumo.Vehicle.getSpeed(vehID));
    }
}
libsumo.Simulation.close();
```

The domains `Simulation`, `Edge`, `Lane`, `Junction`, `Route`, `Vehicle`,
`VehicleType`, `Person`, `TrafficLight`, `InductionLoop`, `LaneArea`,
`MultiEntryExit`, `Poi` and `Polygon` are available as properties of the
module. A runnable browser example which draws the network and the vehicles on
a canvas is in
[{{SUMO}}/build_config/wasm/demo]({{Source}}build_config/wasm/demo), a Node.js
smoke test in
[test_libsumo.js]({{Source}}build_config/wasm/test_libsumo.js).

### Differences to the other bindings

- Everything returning a list gives a plain JavaScript `Array`, and a plain
  array is accepted wherever a list is expected. Nothing has to be freed by hand.
- Result structures are plain objects, a position is `{x, y, z}`, a colour is
  `{r, g, b, a}`.
- Errors are regular `Error` objects with `name` set to `TraCIError`, so
  `try`/`catch` and `error.message` behave as expected.
- Optional arguments do not exist, since embind cannot express default values.
  Every parameter of a bound function has to be passed. For the most common
  calls a shorter signature is bound instead, for example
  `Vehicle.getPosition(vehID)`.
- Subscriptions are not bound. Since there is no socket in between, calling the
  getters in a loop is cheap.

## Limitations

In addition to the [general libsumo limitations](Libsumo.md#limitations):

- the module is single threaded, so `--threads` and
  `--device.rerouting.threads` have to stay at 1
- there is no GUI, `libsumo.GUI` and [sumo-gui](sumo-gui.md) are not part of
  the build, the visualisation has to be done in JavaScript
- PROJ and GDAL are not cross compiled by the dependency script, so networks
  which need a projection at runtime do not work out of the box
- the command line applications ([netconvert](netconvert.md),
  [duarouter](duarouter.md), ...) are not part of the WebAssembly build
- the module uses native WebAssembly exceptions, which need Chrome 95+,
  Firefox 131+, Safari 15.2+ or Node 18+; build with `WASM_EXCEPTIONS=legacy`
  to fall back to the JavaScript based ones
