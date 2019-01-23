/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// activitygen module
// Copyright 2010 TUM (Technische Universitaet Muenchen, http://www.tum.de/)
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    AGBus.h
/// @author  Piotr Woznica
/// @author  Daniel Krajzewicz
/// @author  Walter Bamberger
/// @date    July 2010
/// @version $Id$
///
// A bus driving in the city
/****************************************************************************/
#ifndef AGBUS_H
#define AGBUS_H

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <iostream>
#include <string>


// ===========================================================================
// class definitions
// ===========================================================================
class AGBus {
public:
    AGBus(std::string name, int depTime) :
        name(name),
        departureTime(depTime) {};
    AGBus(int depTime) :
        departureTime(depTime) {};
    void setName(std::string name);
    int getDeparture();
    std::string getName();
    void print() const;

private:
    std::string name;
    int departureTime;
};

#endif

/****************************************************************************/
