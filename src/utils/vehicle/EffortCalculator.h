/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    EffortCalculator.h
/// @author  Michael Behrisch
/// @date    2018-08-21
/// @version $Id$
///
// The EffortCalculator is an interface for additional edge effort calculators
/****************************************************************************/
#ifndef EffortCalculator_h
#define EffortCalculator_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>


// ===========================================================================
// class definitions
// ===========================================================================
/// @brief the effort calculator interface
class EffortCalculator {
public:
    virtual void init(const std::vector<std::string>& edges) = 0;
    virtual double getEffort(const int edge) const = 0;
    virtual void update(const int edge, const int prev, const double length) = 0;
};


#endif

/****************************************************************************/
