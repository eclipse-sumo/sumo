/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    windows_config.h
/// @author  Daniel Krajzewicz
/// @author  Tino Morenz
/// @author  Axel Wegener
/// @author  Michael Behrisch
/// @author  Felix Brack
/// @author  Jakob Erdmann
/// @date    Mon, 17 Dec 2001
/// @version $Id$
///
// The general windows configuration file
/****************************************************************************/
#ifndef config_h
#define config_h

#ifndef _MSC_VER
#error This file is for MSVC compilation only. GCC should use configure generated config.h.
#endif

/* Disable "decorated name length exceeded, name was truncated" warnings. */
#pragma warning(disable: 4503)
/* Disable "identifier truncated in debug info" warnings. */
#pragma warning(disable: 4786)
/* Disable "C++ Exception Specification ignored" warnings */
#pragma warning(disable: 4290)
/* Disable DLL-Interface warnings */
#pragma warning(disable: 4251)

/* Disable "unsafe" warnings for crt functions in VC++ 2005. */
#if _MSC_VER >= 1400
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef WIN32
#define WIN32
#endif

/* Define for dynamic Fox linkage */
#define FOXDLL 1

/* Temporary workaround to enable cmake compilation with MSVC */
#ifndef HAVE_ENABLED
#include <config.h>
#else

/* defines the epsilon to use on position comparison */
#define POSITION_EPS 0.1

/* defines the epsilon to use on general floating point comparison */
#define NUMERICAL_EPS 0.001

/* Define if auto-generated version.h should be used. */
#define HAVE_VERSION_H 1

/* Version number of package */
#ifndef HAVE_VERSION_H
#define VERSION_STRING "0.32.0"
#endif

/* Definition for the character function of Xerces  */
#define XERCES3_SIZE_t XMLSize_t //Xerces >= 3.0
//#define XERCES3_SIZE_t unsigned int //Xerces < 3.0

#endif // HAVE_ENABLED

#endif
