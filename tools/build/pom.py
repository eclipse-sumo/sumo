#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2011-2021 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    pom.py
# @author  Michael Behrisch
# @author  Robert Hilbrich
# @date    2021-01-15

"""
Generates pom files to build libsumo and libtraci jars
"""
from __future__ import absolute_import
from __future__ import print_function
import sys
import os

import version

# SUMO versioning is "backwards" - it is based on the
# last git tag encountered in the repository and adds
# the amount of additional commits at the end.
#
# MAVEN versioning works slightly different -
# 1.9.0-SNAPSHOT (according to SUMO versioning)
# corresponds 1.10.0-SNAPSHOT in the MAVEN world.
#
# In #7921 we decide to make things simple by just
# adding 1 to the minor part of the SUMO version to
# move it to the MAVEN world.
# (Except when we are precisely at a release / tagged commit,
#  then we do not add the 1, because versions are exactly
#  the same in both worlds here.)
#
#   - SUMO: 1.9.0           --> MAVEN: 1.9.0  (release)
#   - SUMO: 1.9.1           --> MAVEN: 1.9.1  (release)
#   - SUMO: 1.9.0.post18    --> MAVEN: 1.10.0-SNAPSHOT
#   - SUMO: 1.9.1.post23    --> MAVEN: 1.10.0-SNAPSHOT

v = version.get_pep440_version()  # v = '1.9.0.post180'

# Are we past a release?
# (If there is no ".post", we are exactly at a release, so we dont touch it)
if ".post" in v:
    # example: '1.9.0.post10' -> '1', '9', '0.post10'
    major, minor, _ = v.split(".", 2)
    # Want v = '1.10.0-SNAPSHOT',
    # but need to make sure, minor releases like
    # 1.9.1-SNAPSHOT work ok too!
    # --> we just override the patch-level version with 0
    v = '%s.%s.0-SNAPSHOT' % (major, int(minor) + 1)

root = os.path.dirname(os.path.dirname(os.path.dirname(__file__)))

with open("pom.xml", "w") as pom:
    pom.write("""<?xml version="1.0" encoding="UTF-8"?>
<project xmlns="http://maven.apache.org/POM/4.0.0" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" \
xsi:schemaLocation="http://maven.apache.org/POM/4.0.0 http://maven.apache.org/xsd/maven-4.0.0.xsd">
    <modelVersion>4.0.0</modelVersion>
    <groupId>org.eclipse.sumo</groupId>
    <artifactId>%s</artifactId>
    <version>%s</version>
    <packaging>jar</packaging>
    <properties>
        <project.build.sourceEncoding>UTF-8</project.build.sourceEncoding>
        <maven.compiler.source>1.8</maven.compiler.source>
        <maven.compiler.target>1.8</maven.compiler.target>
    </properties>

    <distributionManagement>
        <repository>
            <id>repo.eclipse.org</id>
            <name>Project Repository - Releases</name>
            <url>https://repo.eclipse.org/content/repositories/sumo-releases/</url>
        </repository>
        <snapshotRepository>
            <id>repo.eclipse.org</id>
            <name>Project Repository - Snapshots</name>
            <url>https://repo.eclipse.org/content/repositories/sumo-snapshots/</url>
        </snapshotRepository>
    </distributionManagement>

    <licenses>
        <license>
            <name>Eclipse Public License - v 2.0</name>
            <url>https://www.eclipse.org/legal/epl-2.0</url>
        </license>
        <license>
            <name>GNU General Public License, version 2 or later</name>
            <url>https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html</url>
        </license>
    </licenses>

    <build>
        <plugins>
            <plugin>
                <groupId>org.apache.maven.plugins</groupId>
                <artifactId>maven-jar-plugin</artifactId>
                <version>2.3.1</version>
                <configuration>
                    <outputDirectory>%s/bin</outputDirectory>
                </configuration>
            </plugin>
            <plugin>
                <groupId>org.apache.maven.plugins</groupId>
                <artifactId>maven-source-plugin</artifactId>
                <version>3.2.0</version>
                <configuration>
                    <outputDirectory>%s/bin</outputDirectory>
                </configuration>
            </plugin>
        </plugins>
        <resources>
            <resource>
                <directory>%s</directory>
                <includes>
                    <include>LICENSE</include>
                    <include>NOTICE.md</include>
                </includes>
                <targetPath>META-INF</targetPath>
            </resource>
        </resources>
    </build>
</project>
""" % (sys.argv[1], v, root, root, root))
