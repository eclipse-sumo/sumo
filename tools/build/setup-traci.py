#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2017-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    setup-traci.py
# @author  Dominik Buse
# @author  Michael Behrisch
# @date    2017-01-26


from setuptools import setup
import os
import version

SUMO_VERSION = version.get_pep440_version()
package_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))

setup(
    name='traci',
    version=SUMO_VERSION,
    url='https://sumo.dlr.de/docs/TraCI/Interfacing_TraCI_from_Python.html',
    download_url='https://sumo.dlr.de/download',
    author='DLR and contributors',
    author_email='sumo@dlr.de',
    license='EPL-2.0',
    description='The pure python version of the TraCI API to communicate with the traffic simulation Eclipse SUMO',
    long_description='''# TraCI - Traffic Control Interface

TraCI (short for Traffic Control Interface) is an API that provides access to a SUMO traffic simulation, enabling controlling the behavior of multiple simulation objects during a live simulation. It allows for external scripts to interact with the simulation and its vehicles, pedestrians, and infrastructure.

## Installation

To use TraCI, you must first have an Eclipse SUMO installation. Install TraCI by simply executing:
```pip install traci```

A [daily version](https://test.pypi.org/project/traci/) is also available in TestPyPI:
```pip install -i https://test.pypi.org/simple/ traci```

## Getting Started

To use TraCI in your Python code, import the `traci` module. The following code snippet shows a basic example of how to connect to a running SUMO simulation using TraCI:

```python
import traci

# Connect to SUMO simulation
traci.start(["sumo", "-c", "path/to/your/sumocfg/file.sumocfg"])

# Simulation loop
step = 0
while step < 1000:
    traci.simulationStep()
    # Your simulation logic here
    step += 1

# Close TraCI connection
traci.close()
```

Once connected to the SUMO simulation, TraCI provides a range of functions that can be used to query and modify the state of the simulation. For example, you can use TraCI to control the behavior of individual vehicles, modify the traffic light phases, or query the current state of the simulation.

## Documentation

The TraCI documentation is available online at [http://sumo.dlr.de/docs/TraCI.html](http://sumo.dlr.de/docs/TraCI.html). The documentation provides detailed information on the TraCI API, including a list of available functions and their parameters.

## Examples

There are some [TraCI Tutorials](https://sumo.dlr.de/docs/Tutorials/index.html#traci_tutorials) available.

## Contributing

If you find a bug in TraCI or have a suggestion for a new feature, please report it on the SUMO issue tracker at [https://github.com/eclipse/sumo/issues](https://github.com/eclipse/sumo/issues). If you would like to contribute code to TraCI, please submit a pull request to the SUMO repository at [https://github.com/eclipse/sumo](https://github.com/eclipse/sumo).

## License

TraCI is released under the Eclipse Public License 2.0 (EPL-2.0).''',
    long_description_content_type='text/markdown',

    classifiers=[
        'Development Status :: 5 - Production/Stable',
        'Intended Audience :: Developers',
        'Intended Audience :: Science/Research',
        'License :: OSI Approved :: Eclipse Public License 2.0 (EPL-2.0)',
        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 3',
    ],
    keywords='traffic simulation traci sumo',

    packages=["traci", "simpla"],
    package_dir={'': package_dir},

    install_requires=['sumolib>='+SUMO_VERSION],
)
