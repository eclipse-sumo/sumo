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
/// @file    fxheader.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2021
///
// Main header of FOX Toolkit
/****************************************************************************/
#pragma once


// Avoid warnings for external headers in MSVC
#ifdef _MSC_VER
// avoid warnings in clang
#ifdef __clang__
#pragma clang system_header
#endif
#include <fx.h>
#else
#include <fx.h>
#endif

// More info: https://devblogs.microsoft.com/cppblog/broken-warnings-theory/
