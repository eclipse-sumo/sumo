#include <vector>
#include <string>
#include <utility>
#include <iostream>
#include <algorithm>
#include <utils/common/Named.h>
#include "NBEdge.h"
#include "NBDistrict.h"

using namespace std;

NBDistrict::NBDistrict(const std::string &id, const std::string &name,
                       bool sourceConnectorsWeighted,
                       bool sinkConnectorsWeighted)
    : Named(id), _name(name),
    _sourceConnectorsWeighted(sourceConnectorsWeighted),
    _sinkConnectorsWeighted(sinkConnectorsWeighted)
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
    /*if(_connectors.size()==0) {
        throw ProcessError(); // !!! (jemand muss abfragen, ob alles ok ist)
    }*/
    // write the head and the id of the district
    into << "   " << "<district id=\"" << _id << "\">" << endl;
    size_t i;
    // write all sources
    for(i=0; i<_sources.size(); i++) {
        // write the head and the id of the source
        into << "      " << "<dsource id=\"" << _sources[i]->getID()
            << "\" weight=\"";
        // write the weight
        if(_sourceConnectorsWeighted) {
            into << _sourceWeights[i];
        } else {
            into << (1.0/(double) _sources.size());
        }
        into << "\"/>" << endl;
    }
    // write all sinks
    for(i=0; i<_sinks.size(); i++) {
        // write the head and the id of the sink
        into << "      " << "<dsink id=\"" << _sinks[i]->getID()
            << "\" weight=\"";
        // write the weight
        if(_sinkConnectorsWeighted) {
            into << _sinkWeights[i];
        } else {
            into << (1.0/(double) _sinks.size());
        }
        into << "\"/>" << endl;
    }
    // write the tail
    into << "   " << "</district>" << endl << endl;
}

