#ifndef NBDistribution_h
#define NBDistribution_h

#include <string>
#include <map>
#include <utils/distribution/Distribution.h>

class NBDistribution {
public:
    static bool dictionary(const std::string &type, const std::string &id,
        Distribution *d);
    static Distribution *dictionary(const std::string &type,
        const std::string &id);
private:
    typedef std::map<std::string, Distribution*> DistDict;
    typedef std::map<std::string, DistDict> TypedDistDict;
    static TypedDistDict myDict;
};

#endif
