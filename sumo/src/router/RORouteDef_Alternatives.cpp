//---------------------------------------------------------------------------//
//                        RORouteDef_Alternatives.cpp -
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
// Revision 1.6  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.5  2005/05/04 08:52:12  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added; trying to debug invalid vehicles handling
//
// Revision 1.4  2004/12/16 12:26:52  dkrajzew
// debugging
//
// Revision 1.3  2004/11/23 10:25:52  dkrajzew
// debugging
//
// Revision 1.3  2004/11/22 12:48:45  dksumo
// removed some minor warnings
//
// Revision 1.2  2004/10/29 06:18:52  dksumo
// max-alternatives options added
//
// Revision 1.1  2004/10/22 12:50:25  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.2  2004/07/02 09:39:41  dkrajzew
// debugging while working on INVENT; preparation of classes to be derived
//  for an online-routing
//
// Revision 1.1  2004/01/26 08:02:27  dkrajzew
// loaders and route-def types are now renamed in an senseful way;
//  further changes in order to make both new routers work;
//  documentation added
//
// ------------------------------------------------
// Revision 1.13  2003/11/11 08:04:46  dkrajzew
// avoiding emissions of vehicles on too short edges
//
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
// a new interface which allows to choose whether to stop after a route could
//  not be computed or not; not very sphisticated, in fact
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
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <iomanip>
#include <string>
#include <vector>
#include <cmath>
#include <math.h>
#include <cassert>
#include <limits>
#include <iostream>
#include "ROEdge.h"
#include "RORouteDef.h"
#include "RORoute.h"
#include "ROAbstractRouter.h"
#include "RORouteDef_Alternatives.h"
#include <utils/common/StdDefs.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


#ifndef WIN32
#define ISNAN isnan
#endif
#ifdef WIN32
#define ISNAN _isnan
#endif

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
RORouteDef_Alternatives::RORouteDef_Alternatives(const std::string &id,
                                                const RGBColor &color,
                                                size_t lastUsed,
                                                double gawronBeta,
                                                double gawronA,
                                                int maxRoutes)
    : RORouteDef(id, color), _lastUsed(lastUsed),
    _gawronBeta(gawronBeta), _gawronA(gawronA), myMaxRouteNumber(maxRoutes)
{
}


RORouteDef_Alternatives::~RORouteDef_Alternatives()
{
    for(AlternativesVector::iterator i=_alternatives.begin(); i!=_alternatives.end(); i++) {
        delete *i;
    }
}


void
RORouteDef_Alternatives::addLoadedAlternative(RORoute *alt)
{
    _alternatives.push_back(alt);
}



ROEdge *
RORouteDef_Alternatives::getFrom() const
{
    // check whether the item was correctly initialised
    if(_alternatives.size()==0) {
        throw 1; // !!!
    }
    return _alternatives[0]->getFirst();
}


ROEdge *
RORouteDef_Alternatives::getTo() const
{
    // check whether the item was correctly initialised
    if(_alternatives.size()==0) {
        throw 1; // !!!
    }
    return _alternatives[0]->getLast();
}


RORoute *
RORouteDef_Alternatives::buildCurrentRoute(ROAbstractRouter &router,
		SUMOTime begin, bool continueOnUnbuild, ROVehicle &veh,
		ROAbstractRouter::ROAbstractEdgeEffortRetriever * const retriever)
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
RORouteDef_Alternatives::findRoute(RORoute *opt) const
{
    for(size_t i=0; i<_alternatives.size(); i++) {
        if(_alternatives[i]->equals(opt)) {
            return i;
        }
    }
    return -1;
}


double mquiet_NaN = numeric_limits<double>::quiet_NaN();

void
RORouteDef_Alternatives::addAlternative(RORoute *current, SUMOTime begin)
{
    // add the route when it's new
    if(_lastUsed<0) {
        _alternatives.push_back(current);
        _lastUsed = _alternatives.size()-1;
    }
    // recompute the costs and (when a new route was added) the probabilities
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
        assert(_alternatives.size()!=0);
        if(_newRoute) {
            if((*i)!=current) {
                alt->setProbability(
                    alt->getProbability()
                    * double(_alternatives.size()-1)
                    / double(_alternatives.size()));
            } else {
                alt->setProbability(1.0 / double(_alternatives.size()));
            }
        }
    }
    assert(_alternatives.size()!=0);
    // compute the probabilities
    for(i=_alternatives.begin(); i!=_alternatives.end()-1; i++) {
        RORoute *pR = *i;
        for(AlternativesVector::iterator j=i+1; j!=_alternatives.end(); j++) {
            RORoute *pS = *j;
            // see [Gawron, 1998] (4.2)
            double delta =
                (pS->getCosts() - pR->getCosts()) /
                (pS->getCosts() + pR->getCosts());
            // see [Gawron, 1998] (4.3a, 4.3b)
            double newPR = gawronF(pR->getProbability(), pS->getProbability(), delta);
            double newPS = pR->getProbability() + pS->getProbability() - newPR;
            if(ISNAN(newPR)||ISNAN(newPS)) {
                newPR = pS->getCosts() > pR->getCosts()
                    ? 1 : 0;
                newPS = pS->getCosts() > pR->getCosts()
                    ? 0 : 1;
            }
            newPR = MIN2(MAX2(newPR, 0), 1);
            newPS = MIN2(MAX2(newPS, 0), 1);
            pR->setProbability(newPR);
            pS->setProbability(newPS);
        }
    }
    // remove with probability of 0 (not mentioned in Gawron)
    for(i=_alternatives.begin(); i!=_alternatives.end(); ) {
        if((*i)->getProbability()==0) {
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
        chosen = chosen - (*i)->getProbability();
        if(chosen<=0) {
            _lastUsed = pos;
            return;
        }
    }
    _lastUsed = pos;
}


double
RORouteDef_Alternatives::gawronF(double pdr, double pds, double x)
{
    if(((pdr*gawronG(_gawronA, x)+pds)==0)) {
        return std::numeric_limits<double>::max();
    }
    return (pdr*(pdr+pds)*gawronG(_gawronA, x)) /
        (pdr*gawronG(_gawronA, x)+pds);
}


double
RORouteDef_Alternatives::gawronG(double a, double x)
{
    if(((1.0-(x*x))==0)) {
        return std::numeric_limits<double>::max();
    }
    return exp((a*x)/(1.0-(x*x))); // !!! ??
}


void
RORouteDef_Alternatives::xmlOutCurrent(std::ostream &res,
                                       bool isPeriodical) const
{
    _alternatives[_lastUsed]->xmlOut(res, isPeriodical);
}


void
RORouteDef_Alternatives::xmlOutAlternatives(std::ostream &os) const
{
    os << "   <routealt id=\"" << _id << "\" last=\""
        << _lastUsed << "\">" << endl;
    for(size_t i=0; i!=_alternatives.size(); i++) {
        RORoute *alt = _alternatives[i];
        os << "      <route cost=\"" << alt->getCosts()
            << "\" probability=\"" << alt->getProbability()
            << "\">";
        alt->xmlOutEdges(os);
        os << "</route>" << endl;
    }
    os << "   </routealt>" << endl;
}


RORouteDef *
RORouteDef_Alternatives::copy(const std::string &id) const
{
    RORouteDef_Alternatives *ret = new RORouteDef_Alternatives(id,
        myColor, _lastUsed, _gawronBeta, _gawronA, myMaxRouteNumber);
    for(std::vector<RORoute*>::const_iterator i=_alternatives.begin(); i!=_alternatives.end(); i++) {
        ret->addLoadedAlternative(new RORoute(*(*i)));
    }
    return ret;
}


const ROEdgeVector &
RORouteDef_Alternatives::getCurrentEdgeVector() const
{
    assert(_lastUsed>=0&&((size_t) _lastUsed)<_alternatives.size());
    return _alternatives[_lastUsed]->getEdgeVector();
}


void
RORouteDef_Alternatives::invalidateLast()
{
    _lastUsed = -1;
}


void
RORouteDef_Alternatives::addExplicite(RORoute *current, SUMOTime begin)
{
	_alternatives.push_back(current);
    if(myMaxRouteNumber>=0) {
        while(_alternatives.size()>(size_t) myMaxRouteNumber) {
            delete *(_alternatives.begin());
            _alternatives.erase(_alternatives.begin());
        }
    }
	_lastUsed = _alternatives.size()-1;
    // recompute the costs and (when a new route was added) the probabilities
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
                alt->setProbability(
                    alt->getProbability()
                    * double(_alternatives.size()-1)
                    / double(_alternatives.size()));
            } else {
                alt->setProbability(1.0 / double(_alternatives.size()));
            }
        }
    }
    assert(_alternatives.size()!=0);
    // compute the probabilities
    for(i=_alternatives.begin(); i!=_alternatives.end()-1; i++) {
        RORoute *pR = *i;
        for(AlternativesVector::iterator j=i+1; j!=_alternatives.end(); j++) {
            RORoute *pS = *j;
            // see [Gawron, 1998] (4.2)
            double delta =
                (pS->getCosts() - pR->getCosts()) /
                (pS->getCosts() + pR->getCosts());
            // see [Gawron, 1998] (4.3a, 4.3b)
            double newPR = gawronF(pR->getProbability(), pS->getProbability(), delta);
            if(newPR>1||newPR<0) {
                cout << "Caught strange PR:" << newPR << endl;
                newPR = 1.0;
            }
            double newPS = pR->getProbability() + pS->getProbability() - newPR;
            if(newPR<0.0001) {
                newPR = 0.0001;
            }
            if(newPS<0.0001) {
                newPS = 0.0001;
            }
            pR->setProbability(newPR);
            pS->setProbability(newPS);
        }
    }
    // remove with probability of 0 (not mentioned in Gawron)
    for(i=_alternatives.begin(); i!=_alternatives.end(); ) {
        if((*i)->getProbability()==0) {
            i = _alternatives.erase(i);
        } else {
            i++;
        }
    }
}


void
RORouteDef_Alternatives::removeLast()
{
	assert(_alternatives.size()>=2);
	_alternatives.erase(_alternatives.end()-1);
	_lastUsed = _alternatives.size()-1;
	// !!! recompute probabilities
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


