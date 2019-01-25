/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NBLinkPossibilityMatrix.h
/// @author  Daniel Krajzewicz
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// A matric to describe whether two links are foes to each other
/****************************************************************************/
#ifndef NBLinkPossibilityMatrix_h
#define NBLinkPossibilityMatrix_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <bitset>
#include <vector>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NBLinkPossibilityMatrix
 * Storing the information which links may be used simultanously, this matrix
 * is simply made by a vector of bitsets
 */
typedef std::vector<std::bitset<SUMO_MAX_CONNECTIONS> > NBLinkPossibilityMatrix;


#endif

/****************************************************************************/
