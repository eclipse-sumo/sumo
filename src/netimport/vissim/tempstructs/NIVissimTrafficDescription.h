/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NIVissimTrafficDescription.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
/****************************************************************************/
#ifndef NIVissimTrafficDescription_h
#define NIVissimTrafficDescription_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>


#include <string>
#include <map>
#include "NIVissimVehicleClassVector.h"

// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class NIVissimTrafficDescription {
public:
    NIVissimTrafficDescription(const std::string& name,
                               const NIVissimVehicleClassVector& vehicleTypes);
    ~NIVissimTrafficDescription();
    static bool dictionary(int id, const std::string& name,
                           const NIVissimVehicleClassVector& vehicleTypes);
    static bool dictionary(int id, NIVissimTrafficDescription* o);
    static NIVissimTrafficDescription* dictionary(int id);
    static void clearDict();
    static double meanSpeed(int id);
    double meanSpeed() const;
private:
    std::string myName;
    NIVissimVehicleClassVector myVehicleTypes;
private:
    typedef std::map<int, NIVissimTrafficDescription*> DictType;
    static DictType myDict;
};


#endif

/****************************************************************************/

