//---------------------------------------------------------------------------//
//                        NBDistrict.cpp -
//  A class representing districts
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
// Revision 1.11  2003/11/11 08:33:54  dkrajzew
// consequent position2D instead of two doubles added
//
// Revision 1.10  2003/09/05 15:16:57  dkrajzew
// umlaute conversion; node geometry computation; internal links computation
//
// Revision 1.9  2003/05/20 09:33:47  dkrajzew
// false computation of yielding on lane ends debugged; some debugging on tl-import; further work on vissim-import
//
// Revision 1.8  2003/04/16 10:03:47  dkrajzew
// further work on Vissim-import
//
// Revision 1.7  2003/04/02 11:48:43  dkrajzew
// debug statements removed
//
// Revision 1.6  2003/04/01 15:15:49  dkrajzew
// further work on vissim-import
//
// Revision 1.5  2003/03/26 12:00:07  dkrajzew
// debugging for Vissim and Visum-imports
//
// Revision 1.4  2003/03/17 14:22:33  dkrajzew
// further debug and windows eol removed
//
// Revision 1.3  2003/03/03 14:58:53  dkrajzew
// debugging; handling of imported traffic light definitions
//
// Revision 1.2  2003/02/07 10:43:43  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

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


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
NBDistrict::NBDistrict(const std::string &id, const std::string &name,
                       double x, double y)
    : Named(StringUtils::convertUmlaute(id)),
    _name(StringUtils::convertUmlaute(name)),
    myPosition(x, y), _posKnown(true)
{
}


NBDistrict::NBDistrict(const std::string &id, const std::string &name)
    : Named(id), _name(name),
    myPosition(0, 0), _posKnown(true) // !!! (true???)
{
}


NBDistrict::~NBDistrict()
{
}


bool
NBDistrict::addSource(NBEdge *source, double weight)
{
    EdgeVector::iterator i =
        find(_sources.begin(), _sources.end(), source);
    if(i!=_sources.end()) {
        return false;
    }
    _sources.push_back(source);
    _sourceWeights.push_back(weight);
    assert(source->getID()!="");
    return true;
}


bool
NBDistrict::addSink(NBEdge *sink, double weight)
{
    EdgeVector::iterator i =
        find(_sinks.begin(), _sinks.end(), sink);
    if(i!=_sinks.end()) {
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
//    assert(_sourceWeights.size()>0);
//    assert(_sinkWeights.size()>0);
    DoubleVectorHelper::normalise(_sourceWeights, 1.0);
    DoubleVectorHelper::normalise(_sinkWeights, 1.0);
//    assert(DoubleVectorHelper::sum(_sourceWeights)==1);
//    assert(DoubleVectorHelper::sum(_sinkWeights)==1);
    // write the head and the id of the district
    into << "   " << "<district id=\"" << _id << "\">" << endl;
    size_t i;
    // write all sources
    for(i=0; i<_sources.size(); i++) {
        // write the head and the id of the source
        assert(i<_sources.size());
        into << "      " << "<dsource id=\"" << _sources[i]->getID()
            << "\" weight=\"" << _sourceWeights[i] << "\"/>" << endl;
    }
    // write all sinks
    for(i=0; i<_sinks.size(); i++) {
        // write the head and the id of the sink
        assert(i<_sinks.size());
        into << "      " << "<dsink id=\"" << _sinks[i]->getID()
            << "\" weight=\"" << _sinkWeights[i] << "\"/>" << endl;
    }
    // write the tail
    into << "   " << "</district>" << endl << endl;
}


void
NBDistrict::setCenter(double x, double y)
{
    myPosition = Position2D(x, y);
}


void
NBDistrict::replaceIncoming(const EdgeVector &which, NBEdge *by)
{
    // temporary structures
    EdgeVector newList;
    WeightsCont newWeights;
    double joinedVal = 0;
    // go through the list of sinks
    EdgeVector::iterator i=_sinks.begin();
    WeightsCont::iterator j=_sinkWeights.begin();
    for(; i!=_sinks.end(); i++, j++) {
        NBEdge *tmp = (*i);
        double val = (*j);
        if(find(which.begin(), which.end(), tmp)==which.end()) {
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
    double joinedVal = 0;
    // go through the list of sinks
    EdgeVector::iterator i=_sources.begin();
    WeightsCont::iterator j=_sourceWeights.begin();
    for(; i!=_sources.end(); i++, j++) {
        NBEdge *tmp = (*i);
        double val = (*j);
        if(find(which.begin(), which.end(), tmp)==which.end()) {
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




/*void
NBDistrict::normalise(DoubleVector &dv, size_t num)
{
    // normalise sources
    if(_sourceConnectorsWeighted) {
        DoubleVectorHelper::normalise(dv);
    } else {
        dv = DoubleVector(1.0 / (double) num);
    }
}
*/


const Position2D &
NBDistrict::getPosition() const
{
    return myPosition;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NBDistrict.icc"
//#endif

// Local Variables:
// mode:C++
// End:


