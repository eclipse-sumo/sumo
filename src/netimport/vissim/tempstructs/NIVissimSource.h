/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NIVissimSource.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
/****************************************************************************/
#ifndef NIVissimSource_h
#define NIVissimSource_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>


#include <string>
#include <map>

// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class NIVissimSource {
public:
    NIVissimSource(const std::string& id, const std::string& name,
                   const std::string& edgeid);
    ~NIVissimSource();
    static bool dictionary(const std::string& id, const std::string& name,
                           const std::string& edgeid);
    static bool dictionary(const std::string& id, NIVissimSource* o);
    static NIVissimSource* dictionary(const std::string& id);
    static void clearDict();
private:
    std::string myID;
    std::string myName;
    std::string myEdgeID;

private:
    typedef std::map<std::string, NIVissimSource*> DictType;
    static DictType myDict;
};


#endif

/****************************************************************************/

