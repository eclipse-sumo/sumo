#ifndef GUIJunctionControlBuilder_h
#define GUIJunctionControlBuilder_h

#include <netload/NLJunctionControlBuilder.h>
#include <utils/geom/Position2DVector.h>

class Position2DVector;

class GUIJunctionControlBuilder
    : public NLJunctionControlBuilder {
public:
    GUIJunctionControlBuilder();
    ~GUIJunctionControlBuilder();
    void addJunctionShape(const Position2DVector &shape);

protected:
    /** builds a junction that does not use a logic */
    virtual MSJunction *buildNoLogicJunction();

    /** builds a junction with a logic */
    virtual MSJunction *buildLogicJunction();

    /** builds a traffic light junction */
    virtual MSJunction *buildTrafficLightJunction();

private:
    /// The shape of the current junction
    Position2DVector myShape;

};

#endif
