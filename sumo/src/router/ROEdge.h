#ifndef ROEdge_h
#define ROEdge_h

#include <string>
#include <map>
#include <vector>
#include <utils/router/ValueTimeLine.h>

class ROLane;

class ROEdge {
private:
    std::string _id;
    bool _explored;
    double _dist;
    ROEdge *_prevKnot;
    bool _inFrontList;
	double _effort;
    typedef std::map<ROLane*, ValueTimeLine*> LaneUsageCont;
    LaneUsageCont _laneCont;
    ValueTimeLine _ownValueLine;
//    std::vector<ROLane*> _lanes;
    std::vector<ROEdge*> _succeeding;
    bool _usingTimeLine;
public:
	ROEdge(const std::string &id);
	~ROEdge();
    void postloadInit();

    void setEffort(double effort);
    void addLane(ROLane *lane);
//    void computeWeight();
    void setLane(long timeBegin, long timeEnd,
        const std::string &id, float value);
    void addSucceeder(ROEdge *s);
    bool isConnectedTo(ROEdge *e);
    size_t getNoFollowing();
    ROEdge *getFollower(size_t pos);
    double getCost(long time) const;
    long getDuration(long time) const;
    // dijkstra
    void init();
    void initRootDistance();
    float getEffort() const;
    float getNextEffort(long time) const;
    void setEffort(float dist);
    bool isInFrontList() const;
    bool addConnection(ROEdge *to, float effort);
    bool isExplored() const;
    void setExplored(bool value);
    ROEdge *getPrevKnot() const;
    void setPrevKnot(ROEdge *prev);
    std::string getID() const;
protected:
    float getMyEffort(long time) const;
private:
    /// we made the copy constructor invalid
    ROEdge(const ROEdge &src);
    /// we made the assignment operator invalid
    ROEdge &operator=(const ROEdge &src);
};

#endif

