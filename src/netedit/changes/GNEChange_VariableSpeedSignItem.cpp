/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEChange_VariableSpeedSignItem.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2017
/// @version $Id$
///
// A change in the values of Variable Speed Signs in netedit
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/common/MsgHandler.h>
#include <utils/options/OptionsCont.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/additionals/GNEVariableSpeedSign.h>
#include <netedit/additionals/GNEVariableSpeedSignStep.h>

#include "GNEChange_VariableSpeedSignItem.h"

// ===========================================================================
// FOX-declarations
// ===========================================================================

// ===========================================================================
// member method definitions
// ===========================================================================

