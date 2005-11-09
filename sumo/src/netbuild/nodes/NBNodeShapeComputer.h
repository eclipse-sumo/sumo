#ifndef NBNodeShapeComputer_h
#define NBNodeShapeComputer_h

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <fstream>
#include <utils/geom/Position2DVector.h>

class NBNode;
class NBEdge;

class NBNodeShapeComputer {
public:
    NBNodeShapeComputer(const NBNode &node, std::ofstream * const out);
    ~NBNodeShapeComputer();
    Position2DVector compute();

private:
    void addInternalGeometry();
    Position2DVector computeContinuationNodeShape(bool simpleContinuation);
    Position2DVector computeNodeShapeByCrosses();
    bool isSimpleContinuation(const NBNode &n) const;

    struct NeighborCrossDesc {
        bool myUsedOppositeDirection; // !!!
        bool myAmValid;
        SUMOReal myCrossingPosition;
        SUMOReal myCrossingAngle;

    };

private:
    const NBNode &myNode;
    std::ofstream * const myOut;

};

#endif
