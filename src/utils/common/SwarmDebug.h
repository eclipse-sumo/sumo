/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    SwarmDebug.h
/// @author  Riccardo Belletti
/// @date    2014-03-21
/// @version $Id$
///
// Used for additional optional debug messages
/****************************************************************************/
#pragma once


//#ifndef ASSERT_H
//#define ASSERT_H
#include <assert.h>
//#endif


#ifndef SWARM_DEBUG
#define DBG(X) {}
#else
#define DBG(X) {X}
#endif/* DEBUG_H_ */
