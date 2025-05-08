#!/bin/bash
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    ubuntu_release.sh
# @author  Michael Behrisch
# @date    2025-05-08

UBUNTU=$(grep UBUNTU_CODENAME /etc/os-release)
RELEASE=${UBUNTU:16}
export DEBEMAIL="Michael Behrisch <oss@behrisch.de>"

cp -a build_config/debian debian
dch -d
dch -r $RELEASE
dpkg-buildpackage -S
HEADER=$(head -1 debian/changelog)
for ubuntu_version in xenial bionic focal jammy oracular plucky; do
  NEW_HEADER=$(echo $HEADER | sed "s/. $RELEASE/~$ubuntu_version\) $ubuntu_version/")
  sed -i "1 s/.*/$NEW_HEADER/" debian/changelog
  dpkg-buildpackage -S
done
