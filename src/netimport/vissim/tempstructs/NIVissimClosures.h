/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2002-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NIVissimClosures.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
/****************************************************************************/
#ifndef NIVissimClosures_h
#define NIVissimClosures_h


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
class NIVissimClosures {
public:
    NIVissimClosures(const std::string& id,
                     int from_node, int to_node,
                     std::vector<int>& overEdges);
    ~NIVissimClosures();
    static bool dictionary(const std::string& id,
                           int from_node, int to_node, std::vector<int>& overEdges);
    static bool dictionary(const std::string& name, NIVissimClosures* o);
    static NIVissimClosures* dictionary(const std::string& name);
    static void clearDict();
private:
    typedef std::map<std::string, NIVissimClosures*> DictType;
    static DictType myDict;
    const std::string myID;
    int myFromNode, myToNode;
    std::vector<int> myOverEdges;

private:
    /// @brief invalidated copy constructor
    NIVissimClosures(const NIVissimClosures& s);

    /// @brief invalidated assignment operator
    NIVissimClosures& operator=(const NIVissimClosures& s);


};


#endif

/****************************************************************************/

