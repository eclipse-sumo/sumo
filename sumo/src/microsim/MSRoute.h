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
// Revision 1.2  2003/02/07 10:41:51  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <map>
#include <vector>
#include <utils/common/Named.h>

/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSEdge;

typedef std::vector<MSEdge*> MSEdgeVector;
typedef MSEdgeVector::const_iterator MSRouteIterator;

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class MSRoute : public Named {
private:
	typedef std::map<std::string, MSRoute*> RouteDict;
	static RouteDict myDict;
	MSEdgeVector _edges;
    bool _multipleReferenced;
public:
	MSRoute(const std::string &id,
		const MSEdgeVector &edges,
        bool multipleReferenced);
	~MSRoute();
	MSRouteIterator begin() const;
	MSRouteIterator end() const;
	size_t size() const;
	MSEdge *getLastEdge() const;
	static bool dictionary(const std::string &id, MSRoute *route);
	static MSRoute *dictionary(const std::string &id);
    static size_t dictSize() { return myDict.size(); }
	static void clear();
    static void remove(const std::string &id);
    bool inFurtherUse() const;

};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "MSRoute.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

