/****************************************************************************/
/// @file    NBDistrictCont.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A container for districts
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
#ifndef NBDistrictCont_h
#define NBDistrictCont_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <map>
#include <iostream>
#include <string>


// ===========================================================================
// class declarations
// ===========================================================================
class NBDistrict;
class NBEdge;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NBDistrictCont
 * A container of districts
 */
class NBDistrictCont
{
public:
    NBDistrictCont();
    ~NBDistrictCont();

    /** adds a district to the dictionary;
        returns false if the districts already was in the dictionary */
    bool insert(NBDistrict *district);

    /// returns the districts that has the given id
    NBDistrict *retrieve(const std::string &id);

    /** prints all edges */
    void writeXML(std::ostream &into);

    /** returns the number of districts inside the container */
    int size();

    /** deletes all districts */
    void clear();

    /// reports how many districts were loaded
    void report();

    /// adds a source to the named district
    bool addSource(const std::string &dist, NBEdge *source,
                   SUMOReal weight);

    /// adds a destination to the named district
    bool addSink(const std::string &dist, NBEdge *destination,
                 SUMOReal weight);

    void removeFromSinksAndSources(NBEdge *e);

private:
    /// the type of the dictionary where a node may be found by her id
    typedef std::map<std::string, NBDistrict*> DistrictCont;

    /// the instance of the dictionary
    DistrictCont _districts;

private:
    /** invalid copy constructor */
    NBDistrictCont(const NBDistrictCont &s);

    /** invalid assignment operator */
    NBDistrictCont &operator=(const NBDistrictCont &s);

};


#endif

/****************************************************************************/

