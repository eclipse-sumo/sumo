/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <string>
#include <utils/gfx/RGBColor.h>
#include <utils/geom/Position2D.h>
#include <utils/geom/Position2DVector.h>
#include <utils/gui/globjects/GUIPolygon2D.h>
#include <utils/gui/globjects/GUIPointOfInterest.h>
#include <utils/shapes/ShapeContainer.h>
#include "GUIGeomShapeBuilder.h"
#include <utils/common/MsgHandler.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


GUIGeomShapeBuilder::GUIGeomShapeBuilder(GUIGlObjectStorage &idStorage)
    : myIdStorage(idStorage)
{
}


GUIGeomShapeBuilder::~GUIGeomShapeBuilder()
{
}


void
GUIGeomShapeBuilder::polygonEnd(const Position2DVector &shape)
{
    GUIPolygon2D *p =
        new GUIPolygon2D(myIdStorage, myCurrentName, myCurrentType,
            myCurrentColor, shape);
    if(!myShapeContainer->add(p)) {

        MsgHandler::getErrorInstance()->inform("A duplicate of the polygon '" + myCurrentName + "' occured.");
        delete p;
    }
}

void
GUIGeomShapeBuilder::addPoint(const std::string &name,
                             const std::string &type,
                             const RGBColor &c,
                             float x, float y)
{
    GUIPointOfInterest *p =
        new GUIPointOfInterest(myIdStorage, name, type,
            Position2D(x, y), c);
    if(!myShapeContainer->add(p)) {

        MsgHandler::getErrorInstance()->inform("A duplicate of the POI '" + name + "' occured.");
        delete p;
    }
}


ShapeContainer *
GUIGeomShapeBuilder::buildShapeContainer() const
{
    return myShapeContainer;
}
