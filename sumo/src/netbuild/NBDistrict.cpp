/****************************************************************************/
/// @file    NBDistrict.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A class representing districts
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

#include <cassert>
#include <vector>
#include <string>
#include <utility>
#include <iostream>
#include <algorithm>
#include <utils/common/Named.h>
#include <utils/common/StringUtils.h>
#include "NBEdge.h"
#include "NBDistrict.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// member method definitions
// ===========================================================================
NBDistrict::NBDistrict(const std::string &id, const std::string &name,
                       SUMOReal x, SUMOReal y)
        : Named(StringUtils::convertUmlaute(id)),
        _name(StringUtils::convertUmlaute(name)),
        myPosition(x, y)
{}


NBDistrict::NBDistrict(const std::string &id, const std::string &name)
        : Named(id), _name(name),
        myPosition(0, 0)
{}


NBDistrict::~NBDistrict()
{}


bool
NBDistrict::addSource(NBEdge *source, SUMOReal weight)
{
    EdgeVector::iterator i = find(_sources.begin(), _sources.end(), source);
    if (i!=_sources.end()) {
        return false;
    }
    _sources.push_back(source);
    _sourceWeights.push_back(weight);
    assert(source->getID()!="");
    return true;
}


bool
NBDistrict::addSink(NBEdge *sink, SUMOReal weight)
{
    EdgeVector::iterator i = find(_sinks.begin(), _sinks.end(), sink);
    if (i!=_sinks.end()) {
        return false;
    }
    _sinks.push_back(sink);
    _sinkWeights.push_back(weight);
    assert(sink->getID()!="");
    return true;
}


void
NBDistrict::writeXML(std::ostream &into)
{
    DoubleVectorHelper::normalise(_sourceWeights, 1.0);
    DoubleVectorHelper::normalise(_sinkWeights, 1.0);
    // write the head and the id of the district
    into << "   " << "<district id=\"" << _id << "\">" << endl;
    size_t i;
    // write all sources
    for (i=0; i<_sources.size(); i++) {
        // write the head and the id of the source
        assert(i<_sources.size());
        into << "      " << "<dsource id=\"" << _sources[i]->getID()
        << "\" weight=\"" << _sourceWeights[i] << "\"/>" << endl;
    }
    // write all sinks
    for (i=0; i<_sinks.size(); i++) {
        // write the head and the id of the sink
        assert(i<_sinks.size());
        into << "      " << "<dsink id=\"" << _sinks[i]->getID()
        << "\" weight=\"" << _sinkWeights[i] << "\"/>" << endl;
    }
    // write the tail
    into << "   " << "</district>" << endl << endl;
}


void
NBDistrict::setCenter(SUMOReal x, SUMOReal y)
{
    myPosition = Position2D(x, y);
}


void
NBDistrict::replaceIncoming(const EdgeVector &which, NBEdge *by)
{
    // temporary structures
    EdgeVector newList;
    WeightsCont newWeights;
    SUMOReal joinedVal = 0;
    // go through the list of sinks
    EdgeVector::iterator i=_sinks.begin();
    WeightsCont::iterator j=_sinkWeights.begin();
    for (; i!=_sinks.end(); i++, j++) {
        NBEdge *tmp = (*i);
        SUMOReal val = (*j);
        if (find(which.begin(), which.end(), tmp)==which.end()) {
            // if the current edge shall not be replaced, add to the
            //  temporary list
            newList.push_back(tmp);
            newWeights.push_back(val);
        } else {
            // otherwise, skip it and add its weight to the one to be inserted
            //  instead
            joinedVal += val;
        }
    }
    // add the one to be inserted instead
    newList.push_back(by);
    newWeights.push_back(joinedVal);
    // assign to values
    _sinks = newList;
    _sinkWeights = newWeights;
}


void
NBDistrict::replaceOutgoing(const EdgeVector &which, NBEdge *by)
{
    // temporary structures
    EdgeVector newList;
    WeightsCont newWeights;
    SUMOReal joinedVal = 0;
    // go through the list of sinks
    EdgeVector::iterator i=_sources.begin();
    WeightsCont::iterator j=_sourceWeights.begin();
    for (; i!=_sources.end(); i++, j++) {
        NBEdge *tmp = (*i);
        SUMOReal val = (*j);
        if (find(which.begin(), which.end(), tmp)==which.end()) {
            // if the current edge shall not be replaced, add to the
            //  temporary list
            newList.push_back(tmp);
            newWeights.push_back(val);
        } else {
            // otherwise, skip it and add its weight to the one to be inserted
            //  instead
            joinedVal += val;
        }
    }
    // add the one to be inserted instead
    newList.push_back(by);
    newWeights.push_back(joinedVal);
    // assign to values
    _sources = newList;
    _sourceWeights = newWeights;
}


const Position2D &
NBDistrict::getPosition() const
{
    return myPosition;
}


void
NBDistrict::removeFromSinksAndSources(NBEdge *e)
{
    size_t i;
    for (i=0; i<_sinks.size(); ++i) {
        if (_sinks[i]==e) {
            _sinks.erase(_sinks.begin()+i);
            _sinkWeights.erase(_sinkWeights.begin()+i);
        }
    }
    for (i=0; i<_sources.size(); ++i) {
        if (_sources[i]==e) {
            _sources.erase(_sources.begin()+i);
            _sourceWeights.erase(_sourceWeights.begin()+i);
        }
    }
}



/****************************************************************************/

