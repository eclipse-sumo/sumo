/****************************************************************************/
/// @file    ODDistrict.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: $
///
// A district
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
#ifndef ODDistrict_h
#define ODDistrict_h
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
#include <utility>
#include <utils/common/Named.h>
#include <utils/helpers/RandomDistributor.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ODDistrict
 * Class representing a district which has some ingoing and outgoing connections
 * to the road network which may be weighted.
 */
class ODDistrict : public Named
{
public:
    /// Constructor
    ODDistrict(const std::string &id);

    /// Destructor
    ~ODDistrict();

    /** @brief Adds a source connection
     *
     * A source is an edge where vehicles leave the district from to reach
     * the network */
    void addSource(const std::string &id, SUMOReal weight);

    /** @brief Adds a sink connection
     *
     * A sink connection is an edge which is used by vehicles to leave the
     * network and reach the district */
    void addSink(const std::string &id, SUMOReal weight);

    /// Returns the name of a source to use
    std::string getRandomSource() const;

    /// Returns the name of a sink to use
    std::string getRandomSink() const;

    /// Sets the abstract color
    void setColor(SUMOReal val);

    /// returns the color of the district
    SUMOReal getColor() const;

private:
    /// Container of weighted sources
    RandomDistributor<std::string> mySources;

    /// Container of weighted sinks
    RandomDistributor<std::string> mySinks;

private:
    /// The abstract color of the district
    SUMOReal myColor;

};


#endif

/****************************************************************************/

