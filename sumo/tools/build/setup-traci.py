#!/usr/bin/env python
"""
@file    setup-traci.py
@date    2017-01-26
@author  Dominik Buse
@author  Michael Behrisch
@version $Id$

setuptool based setup module for SUMO's TraCI python client library

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2017-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

from setuptools import setup, find_packages

import os

VERSION = '0.29.1'

setup(
    name='traci',

    version=VERSION,

    url='http://sumo.dlr.de/wiki/TraCI/Interfacing_TraCI_from_Python',
    author='DLR and contributors',
    author_email='sumo@dlr.de',

    license='GNU GPL v3',

    classifiers=[
        'Development Status :: 4 - Beta',
        'Intended Audience :: Developers',
        'Intended Audience :: Science/Research',
        'LICENSE :: OSI Approved :: GNU General Public License v3 (GPLv3)',
        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.3',
        'Programming Language :: Python :: 3.4',
        'Programming Language :: Python :: 3.5',
        'Programming Language :: Python :: 3.6',
    ],

    keywords='traffic simulation traci sumo',

    # automatically find packages
    packages=find_packages(include=["traci", "traci.*"]),

    script_name='./build/setup-traci.py',
    data_files = ['./build/setup-traci.py'],

    # TODO: check requirements
    install_requires=['sumolib>=%s' % VERSION],

    # TODO: add extra dependencies for testing
    extras_require={}
)
