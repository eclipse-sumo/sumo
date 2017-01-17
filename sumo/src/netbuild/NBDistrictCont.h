/****************************************************************************/
/// @file    NBDistrictCont.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// A container for districts
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
#include <string>


// ===========================================================================
// class declarations
// ===========================================================================
class NBDistrict;
class NBEdge;
class NBNodeCont;
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NBDistrictCont
 * @brief A container for districts
 *
 * A simple storage for district instances. Allows addition an retrieval of
 *  districts, filling them with sources/sinks, and some other methods which
 *  operate at all stored districts.
 *
 * @see NBDistrict
 */
class NBDistrictCont {
public:
    /// @brief Constructor
    NBDistrictCont();


    /// @brief Destructor
    ~NBDistrictCont();


    /** @brief Adds a district to the dictionary
     *
     * @param[in] district The district to add
     * @return false if the districts already was in the dictionary
     */
    bool insert(NBDistrict* const district);


    /** @brief Returns the districts with the given id
     *
     * @param[in] id The id of the district to retrieve
     * @return The district with the given id if there was one having it, 0 otherwise
     */
    NBDistrict* retrieve(const std::string& id) const;


    /** @brief Returns the pointer to the begin of the stored districts
     * @return The iterator to the beginning of stored edges
     */
    std::map<std::string, NBDistrict*>::const_iterator begin() const {
        return myDistricts.begin();
    }


    /** @brief Returns the pointer to the end of the stored districts
     * @return The iterator to the end of stored edges
     */
    std::map<std::string, NBDistrict*>::const_iterator end() const {
        return myDistricts.end();
    }


    /** @brief Returns the number of districts inside the container */
    int size() const;


    /** @brief Adds a source to the named district
     *
     * At first, the district is tried to be retrieved. If this fails, false is
     *  returned. Otherwise the retrieved districts NBDistrict::addSource-method
     *  is called.
     *
     * @see NBDistrict::addSource
     * @param[in] dist The id of the district to add the source to
     * @param[in] source An edge that shall be used as source
     * @param[in] weight An optional weight of the source
     * @return Whether the source could be added (the district exists and the suorce was not added to it before)
     */
    bool addSource(const std::string& dist, NBEdge* const source,
                   SUMOReal weight);


    /** @brief Adds a sink to the named district
     *
     * At first, the district is tried to be retrieved. If this fails, false is
     *  returned. Otherwise the retrieved districts NBDistrict::addSink-method
     *  is called.
     *
     * @see NBDistrict::addSink
     * @param[in] dist The id of the district to add the sink to
     * @param[in] source An edge that shall be used as sink
     * @param[in] weight An optional weight of the source
     * @return Whether the source could be added (the district exists and the suorce was not added to it before)
     */
    bool addSink(const std::string& dist, NBEdge* const destination,
                 SUMOReal weight);


    /** @brief Removes the given edge from the lists of sources and sinks in all stored districts
     *
     * This method simply goes through all stored districts and calls their method
     *  NBDistrict::removeFromSinksAndSources.
     *
     * @see NBDistrict::removeFromSinksAndSources
     * @param[in] e The edge to remove from sinks/sources
     */
    void removeFromSinksAndSources(NBEdge* const e);


private:
    /// @brief The type of the dictionary where a node may be found by her id
    typedef std::map<std::string, NBDistrict*> DistrictCont;

    /// @brief The instance of the dictionary
    DistrictCont myDistricts;


private:
    /** invalid copy constructor */
    NBDistrictCont(const NBDistrictCont& s);

    /** invalid assignment operator */
    NBDistrictCont& operator=(const NBDistrictCont& s);


};


#endif

/****************************************************************************/

