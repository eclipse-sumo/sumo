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
// Revision 1.7  2005/04/27 11:48:25  dkrajzew
// level3 warnings removed; made containers non-static
//
// Revision 1.6  2004/01/12 15:09:28  dkrajzew
// some work on the documentation
//
// Revision 1.5  2003/06/18 11:13:13  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.4  2003/03/20 16:23:08  dkrajzew
// windows eol removed; multiple vehicle emission added
//
// Revision 1.3  2003/03/03 14:59:01  dkrajzew
// debugging; handling of imported traffic light definitions
//
// Revision 1.2  2003/02/07 10:43:44  dkrajzew
// updated
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

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
 * @class NBDistrictCont
 * A container of districts
 */
class NBDistrictCont {
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
        double weight);

    /// adds a destination to the named district
    bool addSink(const std::string &dist, NBEdge *destination,
        double weight);

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

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:

