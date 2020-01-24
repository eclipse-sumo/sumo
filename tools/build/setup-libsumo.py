#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2017-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    setup-libsumo.py
# @author  Benjamin Striner
# @author  Michael Behrisch
# @date    2017-01-26


from setuptools import setup
import os
import glob
import version

SUMO_VERSION = version.get_version(padZero=False)[1:-11].replace("_", ".").replace("+", ".")
package_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
data_files = glob.glob(os.path.join(os.path.dirname(package_dir), 'bin', '*.dll'))
print(data_files)

setup(
    name='libsumo',
    version=SUMO_VERSION,
    url='https://sumo.dlr.de/docs/Libsumo.html',
    author='DLR and contributors',
    author_email='sumo@dlr.de',
    license='EPL-2.0',

    classifiers=[
        'Development Status :: 4 - Beta',
        'Intended Audience :: Developers',
        'Intended Audience :: Science/Research',
        'LICENSE :: OSI Approved :: Eclipse Public License v2 (EPL-2.0)',
        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.3',
        'Programming Language :: Python :: 3.4',
        'Programming Language :: Python :: 3.5',
        'Programming Language :: Python :: 3.6',
        'Programming Language :: Python :: 3.7',
        'Programming Language :: Python :: 3.8',
    ],
    keywords='traffic simulation traci sumo',

    packages=['libsumo'],
    package_dir={'': package_dir},
    package_data={'libsumo': ['*.pyd', '*.so', '*.dylib']},
    data_files=[("", data_files)]
)
