#ifndef NBDistrictCont_h
#define NBDistrictCont_h
//---------------------------------------------------------------------------//
//                        NBDistrictCont.h -
//  A container for districts
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.2  2003/02/07 10:43:44  dkrajzew
// updated
//
//


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

/* =========================================================================
 * included modules
 * ======================================================================= */
#include <map>
#include <iostream>
#include <string>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class NBDistrict;
class NBEdge;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * NBDistrictCont
 * A container of districts
 */
class NBDistrictCont {
public:
    /** adds a district to the dictionary;
        returns false if the districts already was in the dictionary */
    static bool insert(NBDistrict *edge);

    /// returns the districts that has the given id
    static NBDistrict *retrieve(const std::string &id);

    /** prints all edges */
    static void writeXML(std::ostream &into);

    /** returns the number of districts inside the container */
    static int size();

    /** deletes all districts */
    static void clear();

    /// reports how many districts were loaded
    static void report(bool verbose);

    /// adds a source to the named district
    static bool addSource(const std::string &dist, NBEdge *source,
        double weight);

    /// adds a destination to the named district
    static bool addSink(const std::string &dist, NBEdge *destination,
        double weight);

private:
    /// the type of the dictionary where a node may be found by her id
    typedef std::map<std::string, NBDistrict*> DistrictCont;

    /// the instance of the dictionary
    static DistrictCont _districts;

private:
    /** invalid copy constructor */
    NBDistrictCont(const NBDistrictCont &s);
    /** invalid assignment operator */
    NBDistrictCont &operator=(const NBDistrictCont &s);
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NBDistrictCont.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

