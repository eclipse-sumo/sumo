#ifndef GUIGeomShapeBuilder_h
#define GUIGeomShapeBuilder_h

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

#include <netload/NLGeomShapeBuilder.h>

class ShapeContainer;
class GUIGlObjectStorage;

class GUIGeomShapeBuilder : public NLGeomShapeBuilder {
public:
    GUIGeomShapeBuilder(MSNet &net, GUIGlObjectStorage &idStorage);
    ~GUIGeomShapeBuilder();

    void polygonEnd(const Position2DVector &shape);

    void addPoint(const std::string &name, const std::string &type,
        const RGBColor &c, SUMOReal x, SUMOReal y,
		const std::string &lane, SUMOReal posOnLane);

protected:
    GUIGlObjectStorage &myIdStorage;

};

#endif
