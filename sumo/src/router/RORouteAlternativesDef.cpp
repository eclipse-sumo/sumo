//---------------------------------------------------------------------------//
//                        RORouteAlternativesDef.cpp -
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.12  2003/10/31 08:00:33  dkrajzew
// hope to have patched false usage of RAND_MAX when using gcc
//
// Revision 1.11  2003/10/21 14:45:23  dkrajzew
// errors on numerical problems with large differences patched
//
// Revision 1.10  2003/10/15 11:55:12  dkrajzew
// false usage of rand() patched
//
// Revision 1.9  2003/07/30 09:26:33  dkrajzew
// all vehicles, routes and vehicle types may now have specific colors
//
// Revision 1.8  2003/06/24 08:17:54  dkrajzew
// some strange things happening during the computation of alternatives patched
//
// Revision 1.7  2003/06/18 11:36:50  dkrajzew
// a new interface which allows to choose whether to stop after a route could not be computed or not; not very sphisticated, in fact
//
// Revision 1.6  2003/05/20 09:48:35  dkrajzew
// debugging
//
// Revision 1.5  2003/03/20 16:39:17  dkrajzew
// periodical car emission implemented; windows eol removed
//
// Revision 1.4  2003/02/07 10:45:06  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <iomanip>
#include <string>
#include <vector>
#include <cmath>
#include <cassert>
//#include <cstdlib>
#include <iostream>
#include "ROEdge.h"
#include "RORouteDef.h"
#include "RORoute.h"
#include "RORouter.h"
//#include "RORouteAlternative.h"
#include "RORouteAlternativesDef.h"

using namespace std;

RORouteAlternativesDef::RORouteAlternativesDef(const std::string &id,
                                               const RGBColor &color,
                                               size_t lastUsed,
                                               double gawronBeta, double gawronA)
    : RORouteDef(id, color)/*, _current(-1)*/, _lastUsed(lastUsed),
    _gawronBeta(gawronBeta), _gawronA(gawronA)
{
}


RORouteAlternativesDef::~RORouteAlternativesDef()
{
    for(AlternativesVector::iterator i=_alternatives.begin(); i!=_alternatives.end(); i++) {
        delete *i;
    }
}


void
RORouteAlternativesDef::addLoadedAlternative(RORoute *alt)
{
    _alternatives.push_back(alt);
}



ROEdge *
RORouteAlternativesDef::getFrom() const
{
    // check whether the item was correctly initialised
    if(_alternatives.size()==0) {
        throw 1; // !!!
    }
    return _alternatives[0]->getFirst();
}


ROEdge *
RORouteAlternativesDef::getTo() const
{
    // check whether the item was correctly initialised
    if(_alternatives.size()==0) {
        throw 1; // !!!
    }
    return _alternatives[0]->getLast();
}


RORoute *
RORouteAlternativesDef::buildCurrentRoute(RORouter &router, long begin,
                                          bool continueOnUnbuild)
{
    // recompute duration of the last route used
    // build a new route to test whether it is better
    //  !!! after some iterations, no further routes should be build
    RORoute *opt =
        new RORoute(_id, 0, 1,
            router.compute(getFrom(), getTo(), begin, continueOnUnbuild));
    opt->setCosts(opt->recomputeCosts(begin));
    // check whether the same route was already used
    _lastUsed = findRoute(opt);
    _newRoute = true;
    // delete the route when it already existed
    if(_lastUsed>=0) {
        // this is not completely correct as the value does not
        //  come from the simulation itself but from the computing
        //  using the network !!!
//        _alternatives[_lastUsed]->setCosts(opt->getCosts());
        delete opt;
        _newRoute = false;
        return _alternatives[_lastUsed];
    }
    // return the build route
    return opt;
}


int
RORouteAlternativesDef::findRoute(RORoute *opt) const
{
    for(size_t i=0; i<_alternatives.size(); i++) {
        if(_alternatives[i]->equals(opt)) {
            return i;
        }
    }
    return -1;
}


void
RORouteAlternativesDef::addAlternative(RORoute *current, long begin)
{
    // add the route when it's new
    if(_lastUsed<0) {
        _alternatives.push_back(current);
        _lastUsed = _alternatives.size()-1;
    }
    // recompute the costs and (when a new route was added) the propabilities
    AlternativesVector::iterator i;
    for(i=_alternatives.begin(); i!=_alternatives.end(); i++) {
        RORoute *alt = *i;
        // apply changes for old routes only
        //  (the costs for the current were computed already)
        if((*i)!=current||!_newRoute) {
            // recompute the costs for old routes
            double oldCosts = alt->getCosts();
            double newCosts = alt->recomputeCosts(begin);
            alt->setCosts(_gawronBeta * newCosts + (1.0-_gawronBeta) * oldCosts);
        }
        if(_newRoute) {
            if((*i)!=current) {
                alt->setPropability(
                    alt->getPropability()
                    * double(_alternatives.size()-1)
                    / double(_alternatives.size()));
            } else {
                alt->setPropability(1.0 / double(_alternatives.size()));
            }
        }
    }
    assert(_alternatives.size()!=0);
    // compute the propabilities
    for(i=_alternatives.begin(); i!=_alternatives.end()-1; i++) {
        RORoute *pR = *i;
        for(AlternativesVector::iterator j=i+1; j!=_alternatives.end(); j++) {
            RORoute *pS = *j;
            // see [Gawron, 1998] (4.2)
            double delta =
                (pS->getCosts() - pR->getCosts()) /
                (pS->getCosts() + pR->getCosts());
            // see [Gawron, 1998] (4.3a, 4.3b)
            double newPR = gawronF(pR->getPropability(), pS->getPropability(), delta);
            double newPS = pR->getPropability() + pS->getPropability() - newPR;
            if(newPR<0.0001) {
                newPR = 0.0001;
            }
            if(newPS<0.0001) {
                newPS = 0.0001;
            }
            pR->setPropability(newPR);
            pS->setPropability(newPS);
        }
    }
    // remove with propability of 0 (not mentioned in Gawron)
    for(i=_alternatives.begin(); i!=_alternatives.end(); ) {
        if((*i)->getPropability()==0) {
            i = _alternatives.erase(i);
        } else {
            i++;
        }
    }
    // find the route to use
    double chosen = (double)rand() /
        ( static_cast<double>(RAND_MAX) + 1);
    size_t pos = 0;
    for(i=_alternatives.begin(); i!=_alternatives.end()-1; i++, pos++) {
        chosen = chosen - (*i)->getPropability();
        if(chosen<=0) {
            _lastUsed = pos;
            return;
        }
    }
    _lastUsed = pos;
}


double
RORouteAlternativesDef::gawronF(double pdr, double pds, double x)
{
    return (pdr*(pdr+pds)*gawronG(_gawronA, x)) /
        (pdr*gawronG(_gawronA, x)+pds);
}


double
RORouteAlternativesDef::gawronG(double a, double x)
{
    return exp((a*x)/(1.0-(x*x))); // !!! ??
}


void
RORouteAlternativesDef::xmlOutCurrent(std::ostream &res, bool isPeriodical) const
{
    _alternatives[_lastUsed]->xmlOut(res, isPeriodical);
}


void
RORouteAlternativesDef::xmlOutAlternatives(std::ostream &os) const
{
    os << "   <routealt id=\"" << _id << "\" last=\""
        << _lastUsed << "\">" << endl;
    for(size_t i=0; i!=_alternatives.size(); i++) {
        RORoute *alt = _alternatives[i];
        os << "      <route cost=\"" << alt->getCosts()
            << "\" propability=\"" << alt->getPropability()
            << "\">";
        alt->xmlOutEdges(os);
        os << "</route>" << endl;
    }
    os << "   </routealt>" << endl;
}




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "RORouteAlternativesDef.icc"
//#endif

// Local Variables:
// mode:C++
// End:


