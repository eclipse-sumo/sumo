#ifndef NBNodeShapeComputer_h
#define NBNodeShapeComputer_h

#include <utils/geom/Position2DVector.h>

class NBNode;
class NBEdge;

class NBNodeShapeComputer {
public:
    NBNodeShapeComputer(const NBNode &node);
    ~NBNodeShapeComputer();
    Position2DVector compute();

private:
    void addInternalGeometry();
    Position2DVector computeRealNodeShape();
    Position2DVector computeJoinSplitNodeShape();

    void addCCWPoint(Position2DVector &poly,
        NBEdge *e, double offset);
    void addCWPoint(Position2DVector &poly,
        NBEdge *e, double offset);

private:
    const NBNode &myNode;

};

#endif
