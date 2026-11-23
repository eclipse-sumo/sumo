# Working on Eclipse SUMO

Follow [CONTRIBUTING.md](CONTRIBUTING.md) and the [code style guide](docs/web/docs/Developer/CodeStyle.md). Consult the [source contribution notes](src/README_Contributing.md) when changing a public interface.

## Find the relevant code

| Area | Paths |
| --- | --- |
| Microscopic and mesoscopic simulation | `src/microsim`, `src/mesosim` |
| Network import, building, and editing | `src/netimport`, `src/netbuild`, `src/netedit` |
| Routing and route tools | `src/router`, `src/duarouter`, `tools/route` |
| TraCI clients, server, and in-process API | `src/libtraci`, `src/traci-server`, `src/libsumo`, `tools/traci` |
| Python utilities and shared library | `tools`, `tools/sumolib` |
| Schemas, documentation, and tests | `data/xsd`, `docs/web/docs`, `tests`, `unittest` |

## Build and test

Use CMake and the platform dependencies described in the build guides for [Linux](docs/web/docs/Installing/Linux_Build.md), [macOS](docs/web/docs/Installing/MacOS_Build.md), and [Windows](docs/web/docs/Installing/Windows_Build.md). Configure a separate build directory and build the application affected by the change.

```sh
cmake -S . -B build -DISOLATED_BUILD=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build --target sumo --parallel
```

For a multi-configuration generator, pass `--config Release` when building. For native unit tests, see the [GoogleTest and CTest instructions](docs/web/docs/Developer/Unit_Tests.md).

Use [TextTest](docs/web/docs/Developer/Tests.md) for application regressions and expected outputs. Set `SUMO_HOME` to the checkout and `TEXTTEST_HOME` to its `tests` directory. Point the application binary variable at the executable from your build, such as `SUMO_BINARY` for sumo or `NETCONVERT_BINARY` for netconvert. The repository launchers are [runTests.sh](tests/runTests.sh) and [runAllTests.bat](tests/runAllTests.bat).

```sh
texttest -a sumo -ts basic/routes/simtime_fractional -con
```

Place regression fixtures in the affected application suite. Reuse shared input files where possible and generate network fixtures with netconvert or netgenerate. Check the relevant simulation or routing variants when a change affects shared behavior. Inspect differences before updating expected output.

## Keep interfaces and documentation aligned

- Preserve the units used by `SUMOTime` and use the repository's time conversion helpers.
- Keep the C++ and Python TraCI interfaces consistent when changing a shared command. Check the server, client, and binding paths that the change affects.
- Update the relevant [schemas](data/xsd) and user documentation when changing an XML format or an option. The [configuration generator](tools/build_config/rebuildConfigDocsAndXsd.py) updates application option documentation and configuration schemas.

## Submit changes

Link the relevant issue and report the reproduction and verification in the PR. Follow the contribution guide's preferred commit order for behavior changes. Add the regression, change the implementation, then update affected expectations. Sign off the commits, complete the Eclipse Contributor Agreement, and allow maintainer edits.
