#ifndef NIVissimAbstractEdge_h
#define NIVissimAbstractEdge_h

#include <map>
#include <utils/common/IntVector.h>
#include <utils/geom/Position2DVector.h>

class NIVissimAbstractEdge {
public:
    NIVissimAbstractEdge(int id, const Position2DVector &geom);
    virtual ~NIVissimAbstractEdge();
    Position2D getGeomPosition(double pos) const;
    Position2D getBeginPosition() const;
    Position2D getEndPosition() const;
    void splitAssigning();
    bool crossesEdge(NIVissimAbstractEdge *c) const;
    Position2D crossesEdgeAtPoint(NIVissimAbstractEdge *c) const;
    bool overlapsWith(const AbstractPoly &p, double offset=0.0) const;
    virtual void setNodeCluster(int nodeid) = 0;
    bool hasNodeCluster() const;
    double crossesAtPoint(const Position2D &p1,
        const Position2D &p2) const;

    bool hasGeom() const;
    virtual void buildGeom() = 0;
    int getID() const;
    const Position2DVector &getGeometry() const;

public:
    static bool dictionary(int id, NIVissimAbstractEdge *e);
    static NIVissimAbstractEdge *dictionary(int id);
    static void splitAndAssignToNodes();
    static IntVector getWithin(const AbstractPoly &p, double offset=0.0);
    static void clearDict();


protected:
    int myID;
    Position2DVector myGeom;
    int myNode;

private:
    typedef std::map<int, NIVissimAbstractEdge*> DictType;
    static DictType myDict;
};


#endif
