#ifndef NBJoinedEdgesMap_h
#define NBJoinedEdgesMap_h

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


#include <iostream>
#include <vector>
#include <map>
#include <string>

class NBJoinedEdgesMap {
public:
    NBJoinedEdgesMap();
    ~NBJoinedEdgesMap();
    void init(const std::vector<std::string> &edgeNames);
    void appended(const std::string &to, const std::string &what);
    friend std::ostream &operator<<(std::ostream &os,
        const NBJoinedEdgesMap &map);

private:
    typedef std::vector<std::string> MappedEdgesVector;
    typedef std::map<std::string, MappedEdgesVector> JoinedEdgesMap;
    JoinedEdgesMap myMap;
};

extern NBJoinedEdgesMap gJoinedEdges;

#endif
