/****************************************************************************/
/// @file    NBDistrict.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A class representing districts
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
#ifndef NBDistrict_h
#define NBDistrict_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <string>
#include <iostream>
#include <utility>
#include "NBCont.h"
#include <utils/common/Named.h>
#include <utils/common/DoubleVector.h>
#include <utils/geom/Position2D.h>


// ===========================================================================
// class declarations
// ===========================================================================
class NBEdge;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * NBDistrict
 * A class that represents a district together with incoming and outgoing
 * connections.
 */
class NBDistrict : public Named
{
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

    /** @brief normalises the districts connection usage probabilities */
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

};


#endif

/****************************************************************************/

