/****************************************************************************/
/// @file    NBJunctionLogicCont.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Class for the io-ing between junctions (nodes) and the computers
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NBJunctionLogicCont_h
#define NBJunctionLogicCont_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <map>
#include <iostream>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * NBJunctionLogicCont
 * This class tries to load the logic for a given logickey even when
 * key rotation must have been done
 * If this is not possible (no permutation of the logic has been build),
 * The class may be used for saving the logic build by NBRequest under the
 * given key
 */
class NBJunctionLogicCont
{
public:
    NBJunctionLogicCont();
    ~NBJunctionLogicCont();

    /** returns the number of rotations that must be performed to map the key
        onto an existing logic.
        returns a negative number when the key could not be fitted to any of
        the previously saved keys */
    //int try2convert(const std::string &key);

    /// saves the given junction logic bitfield
    void add(const std::string &key, const std::string &xmlDescription);

    /// saves all known logics
    void writeXML(std::ostream &into);

    /// destroys all stored logics
    void clear();

private:
    /// tries to load a logic with the given key (and all of his permutations)
    bool exists(const std::string &key);

private:
    /// definition o the logic-storage: a map from request to respond
    typedef std::map<std::string, std::string> LogicMap;

    /// the storage for the computed logic
    LogicMap _map;

private:
    /** invalid copy constructor */
    NBJunctionLogicCont(const NBJunctionLogicCont &s);

    /** invalid assignment operator */
    NBJunctionLogicCont &operator=(const NBJunctionLogicCont &s);
};


#endif

/****************************************************************************/

