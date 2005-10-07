#ifndef MSRoute_h
#define MSRoute_h
//---------------------------------------------------------------------------//
//                        MSRoute.h -
//  A vehicle route
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
// Revision 1.10  2005/10/07 11:37:45  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.9  2005/09/15 11:10:46  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.8  2005/05/04 08:32:05  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.7  2004/12/16 12:24:45  dkrajzew
// debugging
//
// Revision 1.6  2004/11/23 10:20:10  dkrajzew
// new detectors and tls usage applied; debugging
//
// Revision 1.5  2004/07/02 09:26:23  dkrajzew
// classes prepared to be derived
//
// Revision 1.4  2003/03/17 14:12:19  dkrajzew
// Windows eol removed
//
// Revision 1.3  2003/03/03 14:56:24  dkrajzew
// some debugging; new detector types added; actuated traffic lights added
//
// Revision 1.2  2003/02/07 10:41:51  dkrajzew
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

#include <string>
#include <map>
#include <vector>
#include <utils/common/Named.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSEdge;
class BinaryInputDevice;


typedef std::vector<const MSEdge*> MSEdgeVector;
typedef MSEdgeVector::const_iterator MSRouteIterator;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class MSRoute : public Named {
public:
    /// Constructor
    MSRoute(const std::string &id, const MSEdgeVector &edges,
        bool multipleReferenced);

    /// Destructor
    virtual ~MSRoute();

    /// Returns the begin of the list of edges to pass
    MSRouteIterator begin() const;

    /// Returns the end of the list of edges to pass
    MSRouteIterator end() const;

    /// Returns the number of edges to pass
    size_t size() const;

    /// returns the destination edge
    const MSEdge *getLastEdge() const;

    /** @brief Returns the information whether the route is needed in the future
        This may be the case, when more than a single vehicle use the same route */
    bool inFurtherUse() const;

    bool replaceBy(const MSEdgeVector &edges, MSRouteIterator &currentEdge);

	void writeEdgeIDs(std::ostream &os) const;

    bool contains(MSEdge *edge) const;
    bool containsAnyOf(const std::vector<MSEdge*> &edgelist) const;

    const MSEdge *operator[](size_t index);

    static void dict_saveState(std::ostream &os, long what);
    void saveState(std::ostream &os, long what);
    static void dict_loadState(BinaryInputDevice &bis, long what);

    size_t posInRoute(const MSRouteIterator &currentEdge) const;

    size_t noReferences() const { // !!!
        return myReferenceNo;
    }

    void incReferenceCnt() {
        myReferenceNo++;
    }


public:
    /** @brief Adds a route to the dictionary
        Returns true if the route could be added, fals if a route with the same name already exists */
    static bool dictionary(const std::string &id, MSRoute *route);

    /** @brief Returns the named route
        Returns 0 if no route with the given name exitsts */
    static MSRoute *dictionary(const std::string &id);

    /// Returns the number of known routes
    static size_t dictSize() { return myDict.size(); }

    /// Clears the dictionary (delete all known routes, too)
    static void clear();

    /// Destroys the named route, removing it also from the dictionary
    static void erase(std::string id);

    MSRouteIterator find(const MSEdge *e) const;

    static void clearLoadedState();



private:
    /// The list of edges to pass
    MSEdgeVector _edges;

    /// Information whether the route is used by more than a single vehicle
    bool _multipleReferenced;

    size_t myReferenceNo;

private:
    /// Definition of the dictionary container
    typedef std::map<std::string, MSRoute*> RouteDict;

    /// The dictionary container
    static RouteDict myDict;

};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

