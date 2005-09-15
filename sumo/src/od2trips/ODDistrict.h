#ifndef ODDistrict_h
#define ODDistrict_h
//---------------------------------------------------------------------------//
//                        ODDistrict.h -
//  A district
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
// Revision 1.5  2005/09/15 12:04:48  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/05/04 08:44:57  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.3  2003/08/04 11:37:37  dkrajzew
// added the generation of colors from districts
//
// Revision 1.2  2003/02/07 10:44:19  dkrajzew
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
#include <config.h>
#endif // HAVE_CONFIG_H

#include <vector>
#include <string>
#include <utility>
#include <utils/common/Named.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class ODDistrict
 * Class representing a district which has some ingoing and outgoing connections
 * to the road network which may be weighted.
 */
class ODDistrict : public Named {
public:
    /// Constructor
    ODDistrict(const std::string &id);

    /// Destructor
    ~ODDistrict();

    /** @brief Adds a source connection
        A source is an edge where vehicles leave the district from to reach
        the network */
    void addSource(const std::string &id, double weight);

    /** @brief Adds a sink connection
        A sink connection is an edge which is used by vehicles to leave the
        network and reach the district */
    void addSink(const std::string &id, double weight);

    /// Returns the name of a source to use
    std::string getRandomSource() const;

    /// Returns the name of a sink to use
    std::string getRandomSink() const;

    /// Sets the abstract color
    void setColor(double val);

    /// returns the color of the district
    double getColor() const;

private:
    /// A name together with a weight (possibility to be chosen)
    typedef std::pair<std::string, double> WeightedName;

    /// List of string/value pairs
    typedef std::vector<WeightedName> WeightedEdgeIDCont;

    /// List of weighted sources
    WeightedEdgeIDCont _sources;

    /// List of weighted sinks
    WeightedEdgeIDCont _sinks;

private:
    /// Returns a member of the given container by random
    std::string getRandom(const WeightedEdgeIDCont &cont) const;

    /// The abstract color of the district
    double myColor;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

