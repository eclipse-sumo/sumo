/****************************************************************************/
/// @file    MSJunction.h
/// @author  Christian Roessel
/// @date    Wed, 12 Dez 2001
/// @version $Id$
///
// The base class for an intersection
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSJunction_h
#define MSJunction_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <map>
#include <utils/geom/Position2D.h>
#include <utils/geom/Position2DVector.h>
#include <utils/common/UtilExceptions.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSJunction
 * @brief The base class for an intersection
 */
class MSJunction {
public:
    /// @brief Destructor.
    virtual ~MSJunction();

    /** @brief Constructor
     * @param[in] id The id of the junction
     * @param[in] position The position of the junction
     * @param[in] shape The shape of the junction
     */
    MSJunction(const std::string &id, const Position2D &position,
               const Position2DVector &shape) throw();

    /** performs some initialisation after the loading
        (e.g., link map computation) */
    virtual void postloadInit() throw(ProcessError);

    /** Clears junction's and lane's requests to prepare for the next
        iteration. */
    virtual bool clearRequests() = 0;

    /// Sets the responds
    virtual bool setAllowed() = 0;

    /** returns the junction's position */
    const Position2D &getPosition() const;

    /// Returns the id of the junction
    const std::string &getID() const;

    /** @brief Returns this junction's shape
     * @return The shape of this junction
     */
    const Position2DVector &getShape() const throw() {
        return myShape;
    }


protected:
    /// @brief The id of the junction
    std::string myID;

    /// @brief The position of the junction
    Position2D myPosition;

    /// @brief The shape of the junction
    Position2DVector myShape;


private:
    /// @brief Invalidated copy constructor.
    MSJunction(const MSJunction&);

    /// @brief Invalidated assignment operator.
    MSJunction& operator=(const MSJunction&);

};


#endif

/****************************************************************************/

