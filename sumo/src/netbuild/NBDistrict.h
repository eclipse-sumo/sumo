#ifndef NBDistrict_h
#define NBDistrict_h

#include <vector>
#include <string>
#include <iostream>
#include <utility>
#include <utils/common/Named.h>

class NBEdge;

class NBDistrict : public Named {
private:
    std::string _name;
    typedef std::vector<NBEdge*> ConnectorCont;
    typedef std::vector<double> WeightsCont;
    ConnectorCont _sources;
    WeightsCont _sourceWeights;
    ConnectorCont _sinks;
    WeightsCont _sinkWeights;
    bool _sourceConnectorsWeighted;
    bool _sinkConnectorsWeighted;
public:
    NBDistrict(const std::string &id, const std::string &name,
        bool sourceConnectorsWeighted,
        bool sinkConnectorsWeighted);
    ~NBDistrict();
    bool addSource(NBEdge *source, double weight=-1);
    bool addSink(NBEdge *sink, double weight=-1);
    void writeXML(std::ostream &into);
};

#endif

