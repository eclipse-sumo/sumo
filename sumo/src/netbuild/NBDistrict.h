#ifndef NBDistrict_h
#define NBDistrict_h
//---------------------------------------------------------------------------//
//                        NBDistrict.h -
//  A class representing districts
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
// Revision 1.11  2005/10/07 11:38:18  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.10  2005/09/23 06:01:05  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.9  2005/09/15 12:02:45  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.8  2005/04/27 11:48:25  dkrajzew
// level3 warnings removed; made containers non-static
//
// Revision 1.7  2003/11/11 08:33:54  dkrajzew
// consequent position2D instead of two SUMOReals added
//
// Revision 1.6  2003/05/20 09:33:47  dkrajzew
// false computation of yielding on lane ends debugged; some debugging on
//  tl-import; further work on vissim-import
//
// Revision 1.5  2003/04/01 15:15:49  dkrajzew
// further work on vissim-import
//
// Revision 1.4  2003/03/20 16:23:08  dkrajzew
// windows eol removed; multiple vehicle emission added
//
// Revision 1.3  2003/03/03 14:58:54  dkrajzew
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
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <vector>
#include <string>
#include <iostream>
#include <utility>
#include "NBCont.h"
#include <utils/common/Named.h>
#include <utils/common/DoubleVector.h>
#include <utils/geom/Position2D.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class NBEdge;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * NBDistrict
 * A class that represents a district together with incoming and outgoing
 * connections.
 */
class NBDistrict : public Named {
public:
    /** @brief constructor with position */
    NBDistrict(const std::string &id, const std::string &name,
        SUMOReal x, SUMOReal y);

    /** @brief constructor without position
        The position must be computed later */
    NBDistrict(const std::string &id, const std::string &name);

    /// destructor
    ~NBDistrict();

    /// adds a source
    bool addSource(NBEdge *source, SUMOReal weight=-1);

    /// adds a sink
    bool addSink(NBEdge *sink, SUMOReal weight=-1);

    /// writes the sumo-xml-representation into the given stream
    void writeXML(std::ostream &into);

    /// Returns the position of this district's center
    const Position2D &getPosition() const;

    /// computes the center of the district
    void computeCenter();

    /** @brief Sets the center coordinates
        Throws an exception when a center was already given */
    void setCenter(SUMOReal x, SUMOReal y);

    /** @brief normalises the districts connection usage propabilities */
    void normalise(DoubleVector &dv, size_t num);

    /// replaces incoming edges from the vector (sinks) by the given edge
    void replaceIncoming(const EdgeVector &which, NBEdge *by);

    /// replaces outgoing edges from the vector (sources) by the given edge
    void replaceOutgoing(const EdgeVector &which, NBEdge *by);

    void removeFromSinksAndSources(NBEdge *e);

private:
    /// a secondary name
    std::string _name;

    /// definition of a vector of connection weights
    typedef DoubleVector WeightsCont;

    /// the sources (connection from district to network)
    EdgeVector _sources;

    /// the weights of the sources
    WeightsCont _sourceWeights;

    /// the sinks (connection from network to district)
    EdgeVector _sinks;

    /// the weights of the sinks
    WeightsCont _sinkWeights;

    /// the position of the distrct (it's mass-middle-point)
    Position2D myPosition;


    /// Information whether the position is given
    bool _posKnown;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

