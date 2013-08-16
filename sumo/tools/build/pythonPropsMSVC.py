#!/usr/bin/env python
"""
@file    pythonPropsMSVC.py
@author  Michael Behrisch
@author  Daniel Krajzewicz
@author  Jakob Erdmann
@date    2011
@version $Id$

This script rebuilds "../../build/msvc/python.props", the file which
gives information about the python includes and library.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2011-2013 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""

import sys, distutils.sysconfig
from os.path import dirname, join

propsFile = join(dirname(__file__), '..', '..', 'build', 'msvc10', 'python.props')
print('generating %s ' % propsFile)
props = open(propsFile, 'w')
print >> props, """<?xml version="1.0" encoding="utf-8"?>
<Project DefaultTargets="Build" ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <PropertyGroup Label="UserMacros">
    <PYTHON_LIB>%s\libs\python%s%s.lib</PYTHON_LIB>
  </PropertyGroup>
  <ItemDefinitionGroup>
    <ClCompile>
      <AdditionalIncludeDirectories>%s;%%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>
      <PreprocessorDefinitions>HAVE_PYTHON;%%(PreprocessorDefinitions)</PreprocessorDefinitions>
    </ClCompile>
  </ItemDefinitionGroup>
  <ItemGroup>
    <BuildMacro Include="PYTHON_LIB">
      <Value>$(PYTHON_LIB)</Value>
    </BuildMacro>
  </ItemGroup>
</Project>""" % (sys.prefix, sys.version[0], sys.version[2],
			distutils.sysconfig.get_config_var('INCLUDEPY'))
props.close()
