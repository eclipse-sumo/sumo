#ifndef NIArtemisTempSegments_h
#define NIArtemisTempSegments_h

#include <vector>
#include <map>
#include <string>
#include <utils/geom/Position2D.h>

class NIArtemisTempSegments {
public:
    static void add(const std::string &linkname, int segment,
        double x, double y);
    static void close();
private:
    class SingleGeomPoint {
    public:
        SingleGeomPoint(int segment, double x, double y);
        ~SingleGeomPoint();
        Position2D getPosition() const;
        int getSegmentNo() const;
    private:
        int mySegment;
        double myX, myY;
    };


    class info_sorter {
    public:
        /// constructor
        explicit info_sorter() { }

        int operator() (SingleGeomPoint *p1, SingleGeomPoint *p2) {
            return p1->getSegmentNo() < p2->getSegmentNo();
        }
    };


    typedef std::vector<SingleGeomPoint*> PosInfos;
    typedef std::map<std::string, PosInfos> Link2PosInfos;

    static Link2PosInfos myLinkPosInfos;
};

#endif
