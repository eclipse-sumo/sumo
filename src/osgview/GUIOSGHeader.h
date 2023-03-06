/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GUIOSGHeader.h
/// @author  Pablo Alvarez lopez
/// @date    March 2021
///
// Headers for OSG -- Oldest supported version is OSG 3.2.1
/****************************************************************************/
#pragma once
#include <config.h>

#ifdef HAVE_OSG

// Define NOMIMAX, due osg may include windows.h somewhere so we need to guard against macro pollution
#ifdef WIN32
#define NOMINMAX
#endif

// avoid certain warnings in MSVC
#ifdef _MSC_VER
// avoid warnings in clang
#ifdef __clang__
#pragma clang system_header
#endif
#pragma warning(push, 0)
#pragma warning(disable: 4275) // do not warn about the DLL interface for OSG
#pragma warning(disable: 4005) // do not warn about macro redefinition
#endif

// OSG Headers
#include <osg/ComputeBoundsVisitor>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Group>
#include <osg/Light>
#include <osg/LightSource>
#include <osg/Material>
#include <osg/MatrixTransform>
#include <osg/Node>
#include <osg/PositionAttitudeTransform>
#include <osg/Sequence>
#include <osg/ShadeModel>
#include <osg/ShapeDrawable>
#include <osg/Texture2D>
#include <osg/Vec4>
#include <osg/Version>
#include <osg/ref_ptr>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgGA/FirstPersonManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
//#include <osgGA/NodeTrackerManipulator>
#include <osgGA/StandardManipulator>
#include <osgGA/TerrainManipulator>
//#include <osgGA/TrackballManipulator>
#include <osgText/FadeText>
#include <osgUtil/Optimizer>
#include <osgUtil/Tessellator>
#include <osgUtil/SmoothingVisitor>
#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>

// pop MSVC warnings
#ifdef _MSC_VER
#pragma warning(pop)
#endif

// Undefine NOMIMAX
#ifdef WIN32
#undef NOMINMAX
#endif

#endif
