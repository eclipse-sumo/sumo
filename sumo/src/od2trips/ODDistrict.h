#ifndef ODDistrict_h
#define ODDistrict_h

#include <vector>
#include <string>
#include <utility>
#include <utils/common/Named.h>

class ODDistrict : public Named {
private:
    typedef std::pair<std::string, double> WeightedName;
    typedef std::vector<WeightedName> WeightedEdgeIDCont;
    WeightedEdgeIDCont _sources;
    WeightedEdgeIDCont _sinks;
public:
    ODDistrict(const std::string &id);
    ~ODDistrict();
    void addSource(const std::string &id, double weight);
    void addSink(const std::string &id, double weight);
    std::string getRandomSource() const;
    std::string getRandomSink() const;
private:
    std::string getRandom(const WeightedEdgeIDCont &cont) const;

};

#endif

