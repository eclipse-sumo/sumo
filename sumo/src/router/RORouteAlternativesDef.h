#ifndef RORouteAlternativesDef_h
#define RORouteAlternativesDef_h
//---------------------------------------------------------------------------//
//                        RORouteAlternativesDef.h -
//  A route with alternative routes
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
// Revision 1.4  2003/06/18 11:36:50  dkrajzew
// a new interface which allows to choose whether to stop after a route could not be computed or not; not very sphisticated, in fact
//
// Revision 1.3  2003/03/20 16:39:17  dkrajzew
// periodical car emission implemented; windows eol removed
//
// Revision 1.2  2003/02/07 10:45:07  dkrajzew
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
#include <vector>
#include "ROEdgeVector.h"
#include "RORouteDef.h"

/* =========================================================================
 * class declarations
 * ======================================================================= */
class DijkstraRouter;
class ROEdge;

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class RORouteAlternativesDef
 * A route definition which has some alternatives, already.
 */
class RORouteAlternativesDef : public RORouteDef {
public:
    /// Constructor
    RORouteAlternativesDef(const std::string &id, size_t lastUsed,
        double gawronBeta, double gawronA);

    /// Destructor
    virtual ~RORouteAlternativesDef();

    /** @brief Adds an alternative loaded from the file
        An alternative may also be generated whicle DUA */
    virtual void addLoadedAlternative(RORoute *alternative);

    /// Returns the trip's origin edge
    ROEdge *getFrom() const;

    /// returns the trip's destination edge
    ROEdge *getTo() const;

protected:
    /// Build the next route
    RORoute *buildCurrentRoute(RORouter &router, long begin,
        bool continueOnUnbuild);

    /// Adds a build alternative
    void addAlternative(RORoute *current, long begin);

    /// Writes the currently chosen route to the stream
    void xmlOutCurrent(std::ostream &res, bool isPeriodical) const;

    /// Writes all the alternatives to the stream
    void xmlOutAlternatives(std::ostream &altres) const;

private:
    /// Searches for the route within the list of alternatives
    int findRoute(RORoute *opt) const;

    /** @brief Performs the gawron - f() function
        From "Dynamic User Equilibria..." */
    double gawronF(double pdr, double pds, double x);

    /** @brief Performs the gawron - g() function
        From "Dynamic User Equilibria..." */
    double gawronG(double a, double x);

private:
    /// Information whether a new route was generated
    bool _newRoute;

    /// Index of the route used within the last step
    int _lastUsed;

    /// Definition of the storage for alternatives
    typedef std::vector<RORoute*> AlternativesVector;

    /// The alternatives
    AlternativesVector _alternatives;

    /// gawron beta - value
    double _gawronBeta;

    /// gawron a - value
    double _gawronA;

};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "RORouteAlternativesDef.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

