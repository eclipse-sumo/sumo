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
                       bool sourceConnectorsWeighted,
                       bool sinkConnectorsWeighted,
                       double x, double y)
    : Named(id), _name(name),
    _sourceConnectorsWeighted(sourceConnectorsWeighted),
    _sinkConnectorsWeighted(sinkConnectorsWeighted),
    _x(x), _y(y), _posKnown(true)
{
}


NBDistrict::NBDistrict(const std::string &id, const std::string &name,
                       bool sourceConnectorsWeighted,
                       bool sinkConnectorsWeighted)
    : Named(id), _name(name),
    _sourceConnectorsWeighted(sourceConnectorsWeighted),
    _sinkConnectorsWeighted(sinkConnectorsWeighted),
    _x(0), _y(0), _posKnown(true)
{
}


NBDistrict::~NBDistrict()
{
}


bool
NBDistrict::addSource(NBEdge *source, double weight)
{
    ConnectorCont::iterator i =
        find(_sources.begin(), _sources.end(), source);
    if(i!=_sources.end()) {
        return false;
    }
    _sources.push_back(source);
    _sourceWeights.push_back(weight);
    return true;
}


bool
NBDistrict::addSink(NBEdge *sink, double weight)
{
    ConnectorCont::iterator i =
        find(_sinks.begin(), _sinks.end(), sink);
    if(i!=_sinks.end()) {
        return false;
    }
    _sinks.push_back(sink);
    _sinkWeights.push_back(weight);
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


double
NBDistrict::getXCoordinate() const
{
    return _x;
}


double
NBDistrict::getYCoordinate() const
{
    return _y;
}


void
NBDistrict::setCenter(double x, double y)
{
    _x = x;
    _y = y;
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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NBDistrict.icc"
//#endif

// Local Variables:
// mode:C++
// End:


