#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2017-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    setup-sumolib.py
# @author  Dominik Buse
# @author  Michael Behrisch
# @date    2017-01-26
# @version $Id$


from setuptools import setup, find_packages

setup(
    name='sumolib',

    version='0.31.0',

    url='http://sumo.dlr.de/wiki/Tools/Sumolib',
    author='DLR and contributors',
    author_email='sumo@dlr.de',

    license='EPL v2',

    classifiers=[
        'Development Status :: 4 - Beta',
        'Intended Audience :: Developers',
        'Intended Audience :: Science/Research',
        'LICENSE :: OSI Approved :: Eclipse Public License v2 (EPLv2)',
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
    packages=find_packages(include=["sumolib", "sumolib.*"]),

    script_name='./build/setup-sumolib.py',
    data_files=['./build/setup-sumolib.py'],

    # TODO: check requirements
    install_requires=[''],

    # TODO: add extra dependencies for testing
    extras_require={
        'visualization': ['matplotlib'],
    }
)
