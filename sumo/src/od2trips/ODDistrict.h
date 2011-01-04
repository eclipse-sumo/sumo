/****************************************************************************/
/// @file    ODDistrict.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A district (origin/destination)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <string>
#include <utility>
#include <utils/common/Named.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/RandomDistributor.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ODDistrict
 * @brief A district (origin/destination)
 *
 * Class representing a district which has some ingoing and outgoing connections
 *  to the road network which may be weighted.
 */
class ODDistrict : public Named {
public:
    /** @brief Constructor
     *
     * @param[in] id The id of the district
     */
    ODDistrict(const std::string &id) throw();


    /// @brief Destructor
    ~ODDistrict() throw();


    /** @brief Adds a source connection
     *
     * A source is an edge where vehicles leave the district from to reach
     *  the network. The weight is used when a random source shall be
     *  chosen.
     *
     * BTW, it is possible to add a source twice. In this case it will occure
     *  twice within the distribution so that the behaviour is as adding
     *  both given probabilities.
     *
     * @param[in] id The id of the source
     * @param[in] weight The weight (probability to be chosen) of the source
     */
    void addSource(const std::string &id, SUMOReal weight) throw();


    /** @brief Adds a sink connection
     *
     * A sink connection is an edge which is used by vehicles to leave the
     *  network and reach the district.  The weight is used when a random
     *  sink shall be chosen.
     *
     * BTW, it is possible to add a sink twice. In this case it will occure
     *  twice within the distribution so that the behaviour is as adding
     *  both given probabilities.
     *
     * @param[in] id The id of the sink
     * @param[in] weight The weight (probability to be chosen) of the sink
     */
    void addSink(const std::string &id, SUMOReal weight) throw();


    /** @brief Returns the id of a source to use
     *
     * If the list of this district's sources is empty, an OutOfBoundsException
     *  -exception is thrown.
     *
     * @return One of this district's sources chosen randomly regarding their weights
     * @exception OutOfBoundsException If this district has no sources
     */
    std::string getRandomSource() const throw(OutOfBoundsException);


    /** @brief Returns the id of a sink to use
     *
     * If the list of this district's sinks is empty, an OutOfBoundsException
     *  -exception is thrown.
     *
     * @return One of this district's sinks chosen randomly regarding their weights
     * @exception OutOfBoundsException If this district has no sinks
     */
    std::string getRandomSink() const throw(OutOfBoundsException);


    /** @brief Returns the number of sinks
     *
     * @return The number of known sinks
     */
    unsigned int sinkNumber() const;


    /** @brief Returns the number of sources
     *
     * @return The number of known sources
     */
    unsigned int sourceNumber() const;


private:
    /// @brief Container of weighted sources
    RandomDistributor<std::string> mySources;

    /// @brief Container of weighted sinks
    RandomDistributor<std::string> mySinks;


private:
    /// @brief invalidated copy constructor
    ODDistrict(const ODDistrict &s);

    /// @brief invalidated assignment operator
    ODDistrict &operator=(const ODDistrict &s);


};


#endif

/****************************************************************************/

