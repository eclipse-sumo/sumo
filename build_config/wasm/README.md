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

### Exception handling

The module uses native WebAssembly exceptions, which need Chrome 95+,
Firefox 131+, Safari 15.2+ or Node 18+. Set `WASM_EXCEPTIONS=legacy` for both
scripts to fall back to the slower but universally supported JavaScript based
exceptions:

```bash
WASM_EXCEPTIONS=legacy ./build_dependencies.sh ~/sumo-wasm-deps
WASM_EXCEPTIONS=legacy ./build.sh ~/sumo-wasm-deps
```

SUMO and every cross compiled dependency have to agree on this, otherwise a
`throw` in Xerces-C never finds its `catch` in libsumo. `build_dependencies.sh`
therefore records the mode in the prefix and `build.sh` refuses to build
against a prefix which was built with the other one. When driving cmake by
hand, pass `-DWASM_LEGACY_EXCEPTIONS=ON` to match a `legacy` prefix.

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
WebAssembly has been instantiated, and every libsumo domain is a property of
that module. How the JavaScript API looks, how it differs from the C++ and
Python ones and what the WebAssembly build cannot do is described in
[the user documentation](../../docs/web/docs/Libsumo_WebAssembly.md);
`test_libsumo.js` and `demo/index.html` next to this file are working examples.

## Files

| file | purpose |
|------|---------|
| `build_dependencies.sh` | cross compiles Xerces-C and zlib |
| `build.sh` | configures and builds the `libsumojs` target |
| `test_libsumo.js` | Node.js smoke test |
| `demo/index.html` | browser demo drawing the simulation on a canvas |
| `example/` | a small grid scenario used by the test and the demo |
