/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2010-2019 German Aerospace Center (DLR) and others.
// activitygen module
// Copyright 2010 TUM (Technische Universitaet Muenchen, http://www.tum.de/)
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    AGCar.h
/// @author  Piotr Woznica
/// @author  Daniel Krajzewicz
/// @author  Walter Bamberger
/// @author  Michael Behrisch
/// @date    July 2010
/// @version $Id$
///
// Cars owned by people of the city: included in households.
/****************************************************************************/
#ifndef AGCAR_H
#define AGCAR_H


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <iostream>
#include <string>
#include "AGAdult.h"


// ===========================================================================
// class definitions
// ===========================================================================
class AGCar {
public:
    AGCar(std::string name) :
        idName(name) {};
    AGCar(int idHH, int idCar) :
        idName(createName(idHH, idCar)) {};
    bool associateTo(AGAdult* pers);
    bool isAssociated() const;
    std::string getName() const;

private:
    std::string createName(int idHH, int idCar);

    std::string idName;
    AGAdult* currentUser;

};

#endif

/****************************************************************************/
