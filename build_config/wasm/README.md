# libsumo for WebAssembly

This directory contains everything needed to compile [libsumo](../../docs/web/docs/Libsumo.md)
into a WebAssembly module which runs a full SUMO simulation inside a browser,
a web worker or Node.js, without any server or native binary.

The build produces two files:

| file          | content                                         |
|---------------|-------------------------------------------------|
| `libsumo.js`  | the JavaScript glue code, exporting `createLibsumo()` |
| `libsumo.wasm`| the compiled simulation                         |

## Prerequisites

* the [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html)
  (`./emsdk install latest && ./emsdk activate latest`, verified with 6.0.9)
* CMake 3.18 or newer, Python 3 and a native C++ toolchain (used for the build tooling)
* Node.js, if you want to run the test below

Activate the SDK in your shell before running any of the scripts:

```bash
source /path/to/emsdk/emsdk_env.sh
```

## Building

The two mandatory dependencies of libsumo, Xerces-C and zlib, have to be cross
compiled first. They are fetched from their upstream git repositories and
installed into a prefix of your choice (`~/sumo-wasm-deps` by default):

```bash
./build_dependencies.sh ~/sumo-wasm-deps
```

Then build the module itself:

```bash
./build.sh ~/sumo-wasm-deps
```

This configures SUMO with the Emscripten toolchain into `<sumo>/build-wasm` and
builds the `libsumojs` target, which ends up in `<sumo>/build-wasm/bin/`.
Doing it by hand instead is equally fine:

```bash
emcmake cmake -S <sumo> -B build-wasm -DCMAKE_BUILD_TYPE=Release \
              -DISOLATED_BUILD=ON -DCMAKE_FIND_ROOT_PATH=$HOME/sumo-wasm-deps
cmake --build build-wasm -j$(nproc) --target libsumojs
```

`CMAKE_FIND_ROOT_PATH` is what makes CMake look for the cross compiled
dependencies instead of the ones installed on the host. Building the default
target (everything) is not supported, the command line applications are not
part of the WebAssembly build.

### Trying it out

A smoke test which loads a small scenario, runs it and exercises the most
important parts of the API:

```bash
node test_libsumo.js ../../build-wasm/bin/libsumo.js
```

The browser demo draws the network and the moving vehicles on a canvas. Copy
the module next to it and serve the directory over HTTP (`file://` URLs cannot
fetch the `.wasm`):

```bash
cp ../../build-wasm/bin/libsumo.js ../../build-wasm/bin/libsumo.wasm demo/
cp -r example demo/
python3 -m http.server -d demo 8000
```

and open <http://localhost:8000>.

## Using the module

`libsumo.js` exports a factory function which resolves to the module once the
WebAssembly has been instantiated. Every libsumo domain is a property of that
module:

```js
const createLibsumo = require('./libsumo.js');   // or a <script> tag in the browser

const libsumo = await createLibsumo();

// the simulation reads its input from the in memory file system
libsumo.FS.mkdir('/scenario');
libsumo.FS.writeFile('/scenario/net.net.xml', netXmlAsStringOrUint8Array);
libsumo.FS.writeFile('/scenario/routes.rou.xml', routesXml);

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

The bound domains are `Simulation`, `Edge`, `Lane`, `Junction`, `Route`,
`Vehicle`, `VehicleType`, `Person`, `TrafficLight`, `InductionLoop`, `LaneArea`,
`MultiEntryExit`, `Poi` and `Polygon`.

### How the API differs from the C++ one

* **Lists are plain arrays.** Everything returning a `std::vector` gives a real
  JavaScript `Array`, so nothing has to be freed by hand. Functions taking a
  list accept a plain array as well.
* **Structs are plain objects.** `TraCIPosition` arrives as `{x, y, z}`,
  `TraCIColor` as `{r, g, b, a}` and so on.
* **Errors are `Error` objects** with `name` set to `TraCIError`, so
  `try`/`catch` and `error.message` work as expected.
* **There are no optional arguments.** Embind cannot express default arguments,
  so every parameter of a bound function has to be passed. Where a default is
  particularly useful the binding exposes a shorter signature instead, for
  example `Vehicle.getPosition(vehID)` and
  `Vehicle.add(vehID, routeID, typeID, depart, departLane, departPos, departSpeed)`.
* **Subscriptions are not bound.** Call the getters in a loop instead, they are
  cheap because there is no socket in between.

## Limitations

* **Single threaded.** The module is built without `-pthread`, since that would
  require the `SharedArrayBuffer` cross origin isolation headers. Options such
  as `--threads` or `--device.rerouting.threads` therefore have to stay at 1.
* **No GUI.** FOX, OpenGL and consequently `sumo-gui`, netedit and `libsumo.GUI`
  are unavailable. Visualisation has to happen on the JavaScript side, as the
  demo shows.
* **No geo projection and no shapefiles by default.** PROJ and GDAL are not
  built by `build_dependencies.sh`. Networks which need a projection at runtime
  (`--proj` options, `Simulation.convertGeo` on a projected network) will fail.
  Cross compiling PROJ into the same prefix and reconfiguring enables them.
* **No sockets.** libsumo never needed them, but this also means a wasm build
  cannot act as a TraCI server for external clients.
* **Exception handling.** The module uses native WebAssembly exceptions, which
  need Chrome 95+, Firefox 131+, Safari 15.2+ or Node 18+ (verified with
  Node 22 and Chromium 141). Configure with
  `-DWASM_LEGACY_EXCEPTIONS=ON` for the slower but universally supported
  JavaScript based exceptions. The very same setting has to be used for the
  dependencies, pass `WASM_EXCEPTION_FLAG=-fexceptions` to
  `build_dependencies.sh` in that case.
* **Memory.** The module starts with 64 MB and grows on demand. Large networks
  need a lot of memory and a 32 bit address space limits it to 4 GB.

## Files

| file | purpose |
|------|---------|
| `build_dependencies.sh` | cross compiles Xerces-C and zlib |
| `build.sh` | configures and builds the `libsumojs` target |
| `test_libsumo.js` | Node.js smoke test |
| `demo/index.html` | browser demo drawing the simulation on a canvas |
| `example/` | a small grid scenario used by the test and the demo |
