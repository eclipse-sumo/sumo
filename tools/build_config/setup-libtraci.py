#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2017-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    setup-libtraci.py
# @author  Benjamin Striner
# @author  Michael Behrisch
# @date    2017-01-26


from setuptools import setup
from setuptools.dist import Distribution
from setuptools.command.install import install
import os
import glob

import version

SUMO_VERSION = version.get_pep440_version()
package_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))


class InstallPlatlib(install):
    def finalize_options(self):
        install.finalize_options(self)
        if self.distribution.has_ext_modules():
            self.install_lib = self.install_platlib


class BinaryDistribution(Distribution):
    """Distribution which always forces a binary package with platform name"""

    def has_ext_modules(self):
        return True


setup(
    name='libtraci',
    version=SUMO_VERSION,
    url='https://sumo.dlr.de/docs/TraCI.html',
    author='DLR and contributors',
    author_email='sumo@dlr.de',
    license='EPL-2.0',
    description="The python version of the libtraci API to communicate with the traffic simulation Eclipse SUMO",
    long_description=open(os.path.join(os.path.dirname(package_dir), 'README.md')).read(),
    long_description_content_type='text/markdown',

    classifiers=[
        'Development Status :: 4 - Beta',
        'Intended Audience :: Developers',
        'Intended Audience :: Science/Research',
        'License :: OSI Approved :: Eclipse Public License 2.0 (EPL-2.0)',
        'Programming Language :: Python :: 3',
    ],
    keywords='traffic simulation traci sumo',

    packages=['libtraci'],
    package_dir={'': package_dir},
    package_data={'libtraci': ['*.pyd', '*.so', '*.dylib']},
    data_files=[("", glob.glob(os.path.join(os.path.dirname(package_dir), 'bin', '*.dll')))],
    install_requires=['traci>='+SUMO_VERSION],
    cmdclass={'install': InstallPlatlib},
    distclass=BinaryDistribution
)
