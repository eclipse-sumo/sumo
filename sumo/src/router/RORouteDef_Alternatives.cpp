/****************************************************************************/
/// @file    RORouteDef_Alternatives.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A route with alternative routes
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
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

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
#include <utils/common/RandHelper.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


#ifndef WIN32
#define ISNAN isnan
#endif
#ifdef WIN32
#define ISNAN _isnan
#endif

// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
RORouteDef_Alternatives::RORouteDef_Alternatives(const std::string &id,
        const RGBColor &color,
        size_t lastUsed,
        SUMOReal gawronBeta,
        SUMOReal gawronA,
        int maxRoutes)
        : RORouteDef(id, color), _lastUsed(lastUsed),
        _gawronBeta(gawronBeta), _gawronA(gawronA), myMaxRouteNumber(maxRoutes)
{}


RORouteDef_Alternatives::~RORouteDef_Alternatives()
{
    for (AlternativesVector::iterator i=_alternatives.begin(); i!=_alternatives.end(); i++) {
        delete *i;
    }
}


void
RORouteDef_Alternatives::addLoadedAlternative(RORoute *alt)
{
    _alternatives.push_back(alt);
}



const ROEdge * const
RORouteDef_Alternatives::getFrom() const
{
    // check whether the item was correctly initialised
    if (_alternatives.size()==0) {
        throw 1; // !!!
    }
    return _alternatives[0]->getFirst();
}


const ROEdge * const
RORouteDef_Alternatives::getTo() const
{
    // check whether the item was correctly initialised
    if (_alternatives.size()==0) {
        throw 1; // !!!
    }
    return _alternatives[0]->getLast();
}


RORoute *
RORouteDef_Alternatives::buildCurrentRoute(ROAbstractRouter &router,
        SUMOTime begin, ROVehicle &veh) const
{
    // recompute duration of the last route used
    // build a new route to test whether it is better
    //  !!! after some iterations, no further routes should be build
    std::vector<const ROEdge*> edges;
    router.compute(getFrom(), getTo(), &veh, begin, edges);
    RORoute *opt = new RORoute(_id, 0, 1, edges);
    opt->setCosts(opt->recomputeCosts(&veh, begin));
    // check whether the same route was already used
    _lastUsed = findRoute(opt);
    _newRoute = true;
    // delete the route when it already existed
    if (_lastUsed>=0) {
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
    for (size_t i=0; i<_alternatives.size(); i++) {
        if (_alternatives[i]->equals(opt)) {
            return i;
        }
    }
    return -1;
}


SUMOReal mquiet_NaN = numeric_limits<SUMOReal>::quiet_NaN();

void
RORouteDef_Alternatives::addAlternative(const ROVehicle *const veh, RORoute *current, SUMOTime begin)
{
    // add the route when it's new
    if (_lastUsed<0) {
        _alternatives.push_back(current);
        _lastUsed = _alternatives.size()-1;
    }
    // recompute the costs and (when a new route was added) the probabilities
    AlternativesVector::iterator i;
    for (i=_alternatives.begin(); i!=_alternatives.end(); i++) {
        RORoute *alt = *i;
        // apply changes for old routes only
        //  (the costs for the current were computed already)
        if ((*i)!=current||!_newRoute) {
            // recompute the costs for old routes
            SUMOReal oldCosts = alt->getCosts();
            SUMOReal newCosts = alt->recomputeCosts(veh, begin);
            alt->setCosts(_gawronBeta * newCosts + ((SUMOReal) 1.0 - _gawronBeta) * oldCosts);
        }
        assert(_alternatives.size()!=0);
        if (_newRoute) {
            if ((*i)!=current) {
                alt->setProbability(
                    alt->getProbability()
                    * SUMOReal(_alternatives.size()-1)
                    / SUMOReal(_alternatives.size()));
            } else {
                alt->setProbability((SUMOReal)(1.0 / (SUMOReal) _alternatives.size()));
            }
        }
    }
    assert(_alternatives.size()!=0);
    // compute the probabilities
    for (i=_alternatives.begin(); i!=_alternatives.end()-1; i++) {
        RORoute *pR = *i;
        for (AlternativesVector::iterator j=i+1; j!=_alternatives.end(); j++) {
            RORoute *pS = *j;
            // see [Gawron, 1998] (4.2)
            SUMOReal delta =
                (pS->getCosts() - pR->getCosts()) /
                (pS->getCosts() + pR->getCosts());
            // see [Gawron, 1998] (4.3a, 4.3b)
            SUMOReal newPR = gawronF(pR->getProbability(), pS->getProbability(), delta);
            SUMOReal newPS = pR->getProbability() + pS->getProbability() - newPR;
            if (ISNAN(newPR)||ISNAN(newPS)) {
                newPR = pS->getCosts() > pR->getCosts()
                        ? (SUMOReal) 1. : 0;
                newPS = pS->getCosts() > pR->getCosts()
                        ? 0 : (SUMOReal) 1.;
            }
            newPR = MIN2((SUMOReal) MAX2(newPR, (SUMOReal) 0), (SUMOReal) 1);
            newPS = MIN2((SUMOReal) MAX2(newPS, (SUMOReal) 0), (SUMOReal) 1);
            pR->setProbability(newPR);
            pS->setProbability(newPS);
        }
    }
    // remove with probability of 0 (not mentioned in Gawron)
    for (i=_alternatives.begin(); i!=_alternatives.end();) {
        if ((*i)->getProbability()==0) {
            i = _alternatives.erase(i);
        } else {
            i++;
        }
    }
    // find the route to use
    SUMOReal chosen = randSUMO();
    size_t pos = 0;
    for (i=_alternatives.begin(); i!=_alternatives.end()-1; i++, pos++) {
        chosen = chosen - (*i)->getProbability();
        if (chosen<=0) {
            _lastUsed = pos;
            return;
        }
    }
    _lastUsed = pos;
}


SUMOReal
RORouteDef_Alternatives::gawronF(SUMOReal pdr, SUMOReal pds, SUMOReal x)
{
    if (((pdr*gawronG(_gawronA, x)+pds)==0)) {
        return std::numeric_limits<SUMOReal>::max();
    }
    return (pdr*(pdr+pds)*gawronG(_gawronA, x)) /
           (pdr*gawronG(_gawronA, x)+pds);
}


SUMOReal
RORouteDef_Alternatives::gawronG(SUMOReal a, SUMOReal x)
{
    if (((1.0-(x*x))==0)) {
        return std::numeric_limits<SUMOReal>::max();
    }
    return (SUMOReal) exp((a*x)/(1.0-(x*x))); // !!! ??
}


RORouteDef *
RORouteDef_Alternatives::copy(const std::string &id) const
{
    RORouteDef_Alternatives *ret = new RORouteDef_Alternatives(id,
                                   myColor, _lastUsed, _gawronBeta, _gawronA, myMaxRouteNumber);
    for (std::vector<RORoute*>::const_iterator i=_alternatives.begin(); i!=_alternatives.end(); i++) {
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
RORouteDef_Alternatives::addExplicite(const ROVehicle *const veh, RORoute *current, SUMOTime begin)
{
    _alternatives.push_back(current);
    if (myMaxRouteNumber>=0) {
        while (_alternatives.size()>(size_t) myMaxRouteNumber) {
            delete *(_alternatives.begin());
            _alternatives.erase(_alternatives.begin());
        }
    }
    _lastUsed = _alternatives.size()-1;
    // recompute the costs and (when a new route was added) the probabilities
    AlternativesVector::iterator i;
    for (i=_alternatives.begin(); i!=_alternatives.end(); i++) {
        RORoute *alt = *i;
        // apply changes for old routes only
        //  (the costs for the current were computed already)
        if ((*i)!=current||!_newRoute) {
            // recompute the costs for old routes
            SUMOReal oldCosts = alt->getCosts();
            SUMOReal newCosts = alt->recomputeCosts(veh, begin);
            alt->setCosts(_gawronBeta * newCosts + (SUMOReal)(1.0-_gawronBeta) * oldCosts);
        }
        if (_newRoute) {
            if ((*i)!=current) {
                alt->setProbability(
                    alt->getProbability()
                    * SUMOReal(_alternatives.size()-1)
                    / SUMOReal(_alternatives.size()));
            } else {
                alt->setProbability((SUMOReal) 1.0 / (SUMOReal) _alternatives.size());
            }
        }
    }
    assert(_alternatives.size()!=0);
    // compute the probabilities
    for (i=_alternatives.begin(); i!=_alternatives.end()-1; i++) {
        RORoute *pR = *i;
        for (AlternativesVector::iterator j=i+1; j!=_alternatives.end(); j++) {
            RORoute *pS = *j;
            // see [Gawron, 1998] (4.2)
            SUMOReal delta =
                (pS->getCosts() - pR->getCosts()) /
                (pS->getCosts() + pR->getCosts());
            // see [Gawron, 1998] (4.3a, 4.3b)
            SUMOReal newPR = gawronF(pR->getProbability(), pS->getProbability(), delta);
            if (newPR>1||newPR<0) {
                cout << "Caught strange PR:" << newPR << endl;
                newPR = 1.0;
            }
            SUMOReal newPS = pR->getProbability() + pS->getProbability() - newPR;
            if (newPR<0.0001) {
                newPR = (SUMOReal) 0.0001;
            }
            if (newPS<0.0001) {
                newPS = (SUMOReal) 0.0001;
            }
            pR->setProbability(newPR);
            pS->setProbability(newPS);
        }
    }
    // remove with probability of 0 (not mentioned in Gawron)
    for (i=_alternatives.begin(); i!=_alternatives.end();) {
        if ((*i)->getProbability()==0) {
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


int
RORouteDef_Alternatives::getLastUsedIndex() const
{
    return _lastUsed;
}


size_t
RORouteDef_Alternatives::getAlternativesSize() const
{
    return _alternatives.size();
}


const RORoute &
RORouteDef_Alternatives::getAlternative(size_t i) const
{
    return *(_alternatives[i]);
}



/****************************************************************************/

