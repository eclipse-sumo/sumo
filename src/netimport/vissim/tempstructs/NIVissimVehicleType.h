/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NIVissimVehicleType.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
/****************************************************************************/
#ifndef NIVissimVehicleType_h
#define NIVissimVehicleType_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>


#include <utils/common/RGBColor.h>
#include <string>
#include <map>

// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class NIVissimVehicleType {
public:
    NIVissimVehicleType(const std::string& name,
                        const std::string& category, const RGBColor& color);
    ~NIVissimVehicleType();
    static bool dictionary(int id, const std::string& name,
                           const std::string& category, const RGBColor& color);
    static bool dictionary(int id, NIVissimVehicleType* o);
    static NIVissimVehicleType* dictionary(int id);
    static void clearDict();

private:
    std::string myName;
    std::string myCategory;
    RGBColor myColor;
private:
    typedef std::map<int, NIVissimVehicleType*> DictType;
    static DictType myDict;
};


#endif

/****************************************************************************/

