/****************************************************************************/
/// @file    MSJunction.h
/// @author  Christian Roessel
/// @date    Wed, 12 Dez 2001
/// @version $Id$
///
// junctions.
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


// ===========================================================================
// class declarations
// ===========================================================================
class GUIJunctionWrapper;
class GUIGlObjectStorage;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSJunction
 */
class MSJunction
{
public:
    /// Destructor.
    virtual ~MSJunction();

    /** Use this constructor only. */
    MSJunction(std::string id, const Position2D &position);

    /** performs some initialisation after the loading
        (e.g., link map computation) */
    virtual void postloadInit();

    /** Clears junction's and lane's requests to prepare for the next
        iteration. */
    virtual bool clearRequests() = 0;

    /// Sets the responds
    virtual bool setAllowed() = 0;

    /** returns the junction's position */
    const Position2D &getPosition() const;

    /// Returns the id of the junction
    const std::string &getID() const;

    // valid for gui-version only
    virtual GUIJunctionWrapper *buildJunctionWrapper(
        GUIGlObjectStorage &idStorage);


protected:
    /// unique ID
    std::string myID;

    /** the position */
    Position2D myPosition;

private:
    /// Default constructor.
    MSJunction();

    /// Copy constructor.
    MSJunction(const MSJunction&);

    /// Assignment operator.
    MSJunction& operator=(const MSJunction&);

};


#endif

/****************************************************************************/

