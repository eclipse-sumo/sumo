#ifndef ROEdgeVector_h
#define ROEdgeVector_h

#include <vector>
#include <string>
#include <deque>

class ROEdge;

class ROEdgeVector {
private:
    typedef std::vector<ROEdge*> EdgeVector;
    EdgeVector _edges;
public:
    ROEdgeVector();
    ~ROEdgeVector();
    void add(ROEdge *edge);
    ROEdge *getFirst() const;
    ROEdge *getLast() const;
    std::deque<std::string> getIDs() const;
    double recomputeCosts(long time) const;
    bool equals(const ROEdgeVector &vc) const;
    size_t size() const;
    ROEdgeVector getReverse() const;
    friend std::ostream &operator<<(std::ostream &os, const ROEdgeVector &ev);
};

#endif
