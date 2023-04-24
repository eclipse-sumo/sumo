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

# @file    setup-sumolib.py
# @author  Dominik Buse
# @author  Michael Behrisch
# @date    2017-01-26


from setuptools import setup, find_packages
import os
import version

SUMO_VERSION = version.get_pep440_version()
package_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))

setup(
    name='sumolib',
    version=SUMO_VERSION,
    url='https://sumo.dlr.de/docs/Tools/Sumolib.html',
    download_url='https://sumo.dlr.de/download',
    author='DLR and contributors',
    author_email='sumo@dlr.de',
    license='EPL-2.0',
    description=('Python helper modules to read networks, parse output data and ' +
                 'do other useful stuff related to the traffic simulation Eclipse SUMO'),
    long_description='''# sumolib

sumolib is a set of python modules for working with SUMO networks, simulation output and other simulation artifacts.

## Installation

Install sumolib by simply executing:
```pip install sumolib```

A [daily version](https://test.pypi.org/project/sumolib/) is also available in TestPyPI:
```pip install -i https://test.pypi.org/simple/ sumolib```

## Getting Started

To use sumolib in your Python code, import the `sumolib` module. The following code snippet shows a basic example of how to load a network file and retrieve the coordinate of a node:

```python
import sumolib

# Parse the network
net = sumolib.net.readNet("myNet.net.xml")

# Retrieve the coordinate of a node based on its ID
print(net.getNode("myNodeID").getCoord())
```

## Documentation

The sumolib documentation is available online at [https://sumo.dlr.de/docs/Tools/Sumolib.html](https://sumo.dlr.de/docs/Tools/Sumolib.html). For a list of available functions take a look at the [pydoc generated documentation](http://sumo.dlr.de/pydoc/sumolib.html) or simply browse the [source code here](https://github.com/eclipse/sumo/tree/main/tools/sumolib).


## Contributing

If you find a bug in sumolib or have a suggestion for a new feature, please report it on the SUMO issue tracker at [https://github.com/eclipse/sumo/issues](https://github.com/eclipse/sumo/issues). If you would like to contribute code to sumolib, please submit a pull request to the SUMO repository at [https://github.com/eclipse/sumo](https://github.com/eclipse/sumo).

## License

sumolib is released under the Eclipse Public License 2.0 (EPL-2.0).''',
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

    packages=find_packages(package_dir, include=["sumolib", "sumolib.*"]),
    package_dir={'': package_dir},

    # TODO: add extra dependencies for testing
    extras_require={
        'visualization': ['matplotlib'],
    }
)
