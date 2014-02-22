/****************************************************************************/
/// @file    ODDistrictCont.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A container for districts
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef ODDistrictCont_h
#define ODDistrictCont_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "ODDistrict.h"
#include <utils/common/NamedObjectCont.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ODDistrictCont
 * @brief A container for districts
 *
 * Besides the inherited methods for adding/removing districts, this container
 *  allows to retrieve a random source or sink from a named district.
 */
class ODDistrictCont : public NamedObjectCont<ODDistrict*> {
public:
    /// Constructor
    ODDistrictCont();


    /// Destructor
    ~ODDistrictCont();


    /** @brief Returns the id of a random source from the named district
     *
     * At first, the named district is retrieved. If this fails, an
     *  InvalidArgument-exception is thrown. Otherwise, a source (edge)
     *  is chosen randomly from this  district using this district's
     *  getRandomSource-method which throws an OutOfBoundsException-exception
     *  if this district does not contain a source.
     *
     * @param[in] name The id of the district to get a random source from
     * @return The id of a randomly chosen source
     * @exception InvalidArgument If the named district is not known
     * @exception OutOfBoundsException If the named district has no sources
     * @see ODDistrict::getRandomSource
     */
    std::string getRandomSourceFromDistrict(const std::string& name) const;


    /** @brief Returns the id of a random sink from the named district
     *
     * At first, the named district is retrieved. If this fails, an
     *  InvalidArgument-exception is thrown. Otherwise, a sink (edge)
     *  is chosen randomly from this district using this district's
     *  getRandomSink-method which throws an OutOfBoundsException-exception
     *  if this district does not contain a sink.
     *
     * @param[in] name The id of the district to get a random sink from
     * @return The id of a randomly chosen sink
     * @exception InvalidArgument If the named district is not known
     * @exception OutOfBoundsException If the named district has no sinks
     * @see ODDistrict::getRandomSink
     */
    std::string getRandomSinkFromDistrict(const std::string& name) const;

    // @brief load districts from FILE
    void loadDistricts(std::string districtfile);

private:
    /// @brief invalidated copy constructor
    ODDistrictCont(const ODDistrictCont& s);

    /// @brief invalidated assignment operator
    ODDistrictCont& operator=(const ODDistrictCont& s);


};


#endif

/****************************************************************************/

