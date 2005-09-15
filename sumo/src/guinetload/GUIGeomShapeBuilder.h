#ifndef GUIGeomShapeBuilder_h
#define GUIGeomShapeBuilder_h

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <netload/NLGeomShapeBuilder.h>

class ShapeContainer;
class GUIGlObjectStorage;

class GUIGeomShapeBuilder : public NLGeomShapeBuilder {
public:
    GUIGeomShapeBuilder(GUIGlObjectStorage &idStorage);
    ~GUIGeomShapeBuilder();

    void polygonEnd(const Position2DVector &shape);

    void addPoint(const std::string &name, const std::string &type,
        const RGBColor &c, float x, float y);

    ShapeContainer *buildShapeContainer() const;

protected:
    GUIGlObjectStorage &myIdStorage;

};

#endif
