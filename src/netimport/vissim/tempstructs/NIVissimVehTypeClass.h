/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2002-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NIVissimVehTypeClass.h
/// @author  Michael Behrisch
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
/****************************************************************************/
#ifndef NIVissimVehTypeClass_h
#define NIVissimVehTypeClass_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <string>
#include <map>
#include <utils/common/RGBColor.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class NIVissimVehTypeClass {
public:
    NIVissimVehTypeClass(int id, const std::string& name,
                         const RGBColor& color, std::vector<int>& types);
    ~NIVissimVehTypeClass();
    static bool dictionary(int id, const std::string& name,
                           const RGBColor& color, std::vector<int>& types);
    static bool dictionary(int id, NIVissimVehTypeClass* o);
    static NIVissimVehTypeClass* dictionary(int name);
    static void clearDict();
    double meanSpeed() const;
private:
    int myID;
    std::string myName;
    RGBColor myColor;
    std::vector<int> myTypes;
private:
    typedef std::map<int, NIVissimVehTypeClass*> DictType;
    static DictType myDict;
};


#endif

/****************************************************************************/

