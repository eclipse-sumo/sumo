#ifndef NIArtemisTempEdgeLanes_h
#define NIArtemisTempEdgeLanes_h

#include <string>
#include <map>
#include <bitset>
#include <utils/common/DoubleVector.h>

class NIArtemisTempEdgeLanes {
public:
    static void add(const std::string &link, int lane, int section,
        double start, double end, const std::string &mvmt);
    static void close();
private:

    class LinkLaneDesc {
    public:
        LinkLaneDesc(int lane, int section, 
            double start, double end, const std::string &mvmt);
        ~LinkLaneDesc();
        double getStart() const;
        double getEnd() const;
        int getLane() const;
        void patchPosition(double length);
    private:
        int myLane;
        int mySection;
        double myStart;
        double myEnd;
    };

    static size_t getBeginIndex(double start, const DoubleVector &poses);
    static size_t getEndIndex(double start, const DoubleVector &poses);
    static size_t count(const std::bitset<64> &lanes);

    
    
    typedef std::vector<LinkLaneDesc*> LaneDescVector;
    typedef std::map<std::string, LaneDescVector> Link2LaneDesc;
    static Link2LaneDesc myLinkLaneDescs;

    typedef std::map<std::string, DoubleVector> Link2Positions;
    static Link2Positions myLinkLanePositions;

};

#endif
