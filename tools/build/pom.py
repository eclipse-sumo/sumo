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
# @date    2021-01-15

"""
Generates pom files to build libsumo and libtraci jars
"""
from __future__ import absolute_import
from __future__ import print_function
import sys
import os

import version


v = version.get_pep440_version()
if ".post" in v:
    v = v[:v.index(".post")] + "-SNAPSHOT"
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
            <id>github</id>
            <name>GitHub Eclipse SUMO Apache Maven Packages</name>
            <url>https://maven.pkg.github.com/eclipse/sumo</url>
        </repository>
    </distributionManagement>

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
