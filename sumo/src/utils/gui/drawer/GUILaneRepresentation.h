#ifndef GUILaneRepresentation_h
#define GUILaneRepresentation_h

#include <utils/gui/globjects/GUIGlObject.h>

class GUILaneRepresentation : public GUIGlObject {
public:
    /// Constructor
    GUILaneRepresentation(GUIGlObjectStorage &idStorage, std::string fullName)
        : GUIGlObject(idStorage, fullName) {}

    /// Constructor for objects joining gl-objects
    GUILaneRepresentation(GUIGlObjectStorage &idStorage,
            std::string fullName, size_t glID)
        : GUIGlObject(idStorage, fullName, glID) {}

    virtual ~GUILaneRepresentation() { }

    /** returns the begin position of the lane */
    virtual const Position2D &getBegin() const = 0;

    /** returns the end position of the lane */
    virtual const Position2D &getEnd() const = 0;

    /** returns the direction of the lane (!!! describe better) */
    virtual const Position2D &getDirection() const = 0;

    /** returns the rotation of the lane (!!! describe better) */
    virtual double getRotation() const = 0;

    /** returns the "visualisation length"; this length may differ to the
        real length */
    virtual double visLength() const = 0;

    virtual const Position2DVector &getShape() const = 0;
    virtual const DoubleVector &getShapeRotations() const = 0;
    virtual const DoubleVector &getShapeLengths() const = 0;

    virtual void selectSucessors() { }

};

#endif

