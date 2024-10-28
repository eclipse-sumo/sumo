#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2020-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    setup-sumo.py
# @author  Michael Behrisch
# @date    2020-07-28

import os
from skbuild import setup
import version

SUMO_VERSION = version.get_pep440_version()
sumo_dir = os.path.abspath(os.path.dirname(__file__))
while not os.path.exists(os.path.join(sumo_dir, 'README.md')) and sumo_dir != os.path.dirname(sumo_dir):
    sumo_dir = os.path.dirname(sumo_dir)

setup(
    name='eclipse-sumo',
    version=SUMO_VERSION,
    url='https://sumo.dlr.de/',
    download_url='https://sumo.dlr.de/download',
    author='DLR and contributors',
    author_email='sumo@dlr.de',
    license='EPL-2.0',
    description=("A microscopic, multi-modal traffic simulation package"),
    long_description=open(os.path.join(sumo_dir, 'README.md')).read(),
    long_description_content_type='text/markdown',

    classifiers=[
        'Development Status :: 5 - Production/Stable',
        'Intended Audience :: Developers',
        'Intended Audience :: Science/Research',
        'License :: OSI Approved :: Eclipse Public License 2.0 (EPL-2.0)',
        'Programming Language :: C++',
        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 3',
    ],
    keywords='traffic simulation traci sumo',

    packages=['sumo'],
    package_dir={'': 'tools/build_config'},

    cmake_install_dir='tools/build_config/sumo',

    entry_points={
        'console_scripts': [
            'activitygen=sumo:activitygen',
            'dfrouter=sumo:dfrouter',
            'duarouter=sumo:duarouter',
            'emissionsDrivingCycle=sumo:emissionsDrivingCycle',
            'emissionsMap=sumo:emissionsMap',
            'jtrrouter=sumo:jtrrouter',
            'marouter=sumo:marouter',
            'netconvert=sumo:netconvert',
            'netedit=sumo:netedit',
            'netgenerate=sumo:netgenerate',
            'od2trips=sumo:od2trips',
            'polyconvert=sumo:polyconvert',
            'sumo=sumo:sumo',
            'sumo-gui=sumo:sumo_gui',
        ]
    },
)
