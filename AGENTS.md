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

Place regression fixtures in the affected application suite. Reuse an existing test network when it covers the case. Check related open pull requests for suitable fixtures. If no existing network covers the case, generate the network with netconvert or netgenerate and record the generator inputs and command. Keep fixtures, simulation horizons, and written output attributes minimal (generally under 50 KB) so test suites stay fast. Check the relevant simulation or routing variants when a change affects shared behavior. Inspect differences before updating expected output.

For Python tools under `tests/tools`, follow the declarative TextTest paradigm with [toolrunner.py](tests/toolrunner.py) and [config.tools](tests/tools/config.tools): specify options and the target tool script (listed last) in `options.tools`, provide small static input files, and check expected results (`output.tools`, `errors.tools`, and collated files). Keep as little code as possible in tests. Do not add custom `runner.py` scripts to `tests/tools` unless testing `sumolib` as a Python library; reserve `tests/complex` for scenarios that require multiple application or tool calls or TraCI scripts.

Existing test and code coverage is limited, so passing current suites alone does not confirm that modified code paths are exercised. Check the [daily LCOV coverage report](https://sumo.dlr.de/daily/lcov/html/) to verify whether touched code is covered and identify gaps that need targeted tests.

Prioritize shared testing resources. Run targeted builds, unit tests, and focused TextTest selections locally first rather than relying on full CI matrices for iteration, and avoid unnecessary pushes that re-trigger multi-platform workflows.

## Keep interfaces and documentation aligned

- Preserve the units used by `SUMOTime` and use the repository's time conversion helpers.
- Keep the C++ and Python TraCI interfaces consistent when changing a shared command. Check the server, client, and binding paths that the change affects.
- Update the relevant [schemas](data/xsd) and user documentation when changing an XML format or an option. The [configuration generator](tools/build_config/rebuildConfigDocsAndXsd.py) updates application option documentation and configuration schemas.

## Submit changes

- Break oversized issues and broad tasks into smaller, well-scoped issues and atomic, single-topic pull requests that can be reviewed and merged independently.
- Keep standalone test-coverage additions and test-suite cleanups in dedicated test-only pull requests rather than combining them with functional code changes.
- Structure pull requests that modify application or tool behavior into three commits as specified in [CONTRIBUTING.md](CONTRIBUTING.md):
  1. One commit that adds the new test.
  2. One commit that modifies the source code.
  3. One commit that updates the results of all affected tests.
- Use context-based rebasing on open pull requests. Rebasing and force-pushing clears completed CI check results, requires workflows to run again, and disrupts the review flow. Inspect upstream `main` changes first and rebase only when semantically needed, such as for merge conflicts, upstream changes that affect the touched code, interfaces, schemas, or expected test outputs, or an explicit maintainer request.
- Link the relevant issue and report the reproduction and verification in the PR. Sign off the commits, complete the Eclipse Contributor Agreement, and allow maintainer edits.
- Declare co-authorship on every commit by adding option `--trailer "Co-authored-by: <Agent Name> <email>"` (i.e. `--trailer "Co-authored-by: Claude <noreply@anthropic.com>"`)
