#ifndef NIVissimEdgePosMap_h
#define NIVissimEdgePosMap_h

#include <map>

class NIVissimEdgePosMap {
public:
    NIVissimEdgePosMap();
    ~NIVissimEdgePosMap();
    void add(int edgeid, double pos);
    void add(int edgeid, double from, double to);
    void join(NIVissimEdgePosMap &with);
private:
    typedef std::pair<double, double> Range;
    typedef std::map<int, Range> ContType;
    ContType myCont;
};




#endif
