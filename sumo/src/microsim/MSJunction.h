#ifndef MSJunction_H
#define MSJunction_H
/***************************************************************************
                          MSJunction.h  -  Base class for all kinds of
                                           junctions.
                             -------------------
    begin                : Wed, 12 Dez 2001
    copyright            : (C) 2001 by Christian Roessel
    email                : roessel@zpr.uni-koeln.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// $Log$
// Revision 1.6  2003/07/22 15:05:22  dkrajzew
// warnings removed; formatted the code
//
// Revision 1.5  2003/07/16 15:31:14  dkrajzew
// junctions are now able to build gui-wrappers (throws an exception within the gui-version, but the interface is needed)
//
// Revision 1.4  2003/07/07 08:16:28  dkrajzew
// a junction is now able to return its id
//
// Revision 1.3  2003/02/07 10:41:51  dkrajzew
// updated
//
// Revision 1.2  2002/10/16 16:42:28  dkrajzew
// complete deletion within destructors implemented; clear-operator added for container; global file include; junction extended by position information (should be revalidated later)
//
// Revision 1.1  2002/10/16 14:48:26  dkrajzew
// ROOT/sumo moved to ROOT/src
//
// Revision 1.1.1.1  2002/04/08 07:21:23  traffic
// new project name
//
// Revision 2.0  2002/02/14 14:43:15  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.14  2002/02/01 11:40:34  croessel
// Changed return-type of some void methods used in for_each-loops to
// bool in order to please MSVC++.
//
// Revision 1.13  2001/12/20 11:54:05  croessel
// using namespace std replaced by std::
//
// Revision 1.12  2001/12/12 17:46:01  croessel
// Initial commit. Part of a new junction hierarchy.
//

/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <vector>
#include <map>


 /* =========================================================================
 * class declarations
 * ======================================================================= */
class GUIJunctionWrapper;
class GUIGlObjectStorage;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 */
class MSJunction
{
public:
    /// needed for the access to the dictionary
    friend class GUIEdgeGrid;

    /// Destructor.
    virtual ~MSJunction();

    /** Use this constructor only. */
    MSJunction( std::string id, double x, double y );

    /** performs some initialisation after the loading
        (e.g., link map computation) */
    virtual void postloadInit();

    /** Clears junction's and lane's requests to prepare for the next
        iteration. */
    virtual bool clearRequests() = 0;

    /// Sets the responds
    virtual bool setAllowed() = 0;

    /** @brief Inserts MSJunction into the static dictionary
        Returns true if the key id isn't already in the dictionary.
        Otherwise returns false (the junction is not inserted then). */
    static bool dictionary( std::string id, MSJunction* junction);

    /** @brief Returns the MSEdgeControl associated to the key id if exists,
        Otherwise returns 0. */
    static MSJunction* dictionary( std::string id);

    static size_t dictSize();

    /** Returns the list of all known ids */
    static std::vector<std::string> getNames();

    /** Clears the dictionary */
    static void clear();

    /** @brief Performs some initialisation after the net was completely loaded
        This is done for all junctions within the dictionary */
    static void postloadInitContainer();

    /** returns the junction's x-position */
    double getXCoordinate() const;

    /** returns the junction's y-position */
    double getYCoordinate() const;

    /// Returns the id of the junction
    std::string id() const;

    // valid for gui-version only
    virtual GUIJunctionWrapper *buildJunctionWrapper(
        GUIGlObjectStorage &idStorage);

    friend class GUIGrid;

protected:
    /// unique ID
    std::string myID;

    /// definition of the static dictionary type
    typedef std::map< std::string, MSJunction* > DictType;

    /** Static dictionary to associate string-ids with objects. */
    static DictType myDict;

    /** the position */
    double myX, myY;

private:
    /// Default constructor.
    MSJunction();

    /// Copy constructor.
    MSJunction( const MSJunction& );

    /// Assignment operator.
    MSJunction& operator=( const MSJunction& );

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "MSJunction.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
