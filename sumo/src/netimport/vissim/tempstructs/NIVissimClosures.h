#ifndef NIVissimClosures_h
#define NIVissimClosures_h

#include <string>
#include <map>
#include <utils/common/IntVector.h>

class NIVissimClosures {
public:
    NIVissimClosures(const std::string &id,
        int from_node, int to_node,
        IntVector &overEdges);
    ~NIVissimClosures();
    static bool dictionary(const std::string &id,
        int from_node, int to_node, IntVector &overEdges);
    static bool dictionary(const std::string &name, NIVissimClosures *o);
    static NIVissimClosures *dictionary(const std::string &name);
private:
    typedef std::map<std::string, NIVissimClosures*> DictType;
    static DictType myDict;
    const std::string myID;
    int myFromNode, myToNode;
    IntVector myOverEdges;
};

#endif

