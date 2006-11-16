
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)

/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include "GUIPolygon2D.h"
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


using namespace std;


GUIPolygon2D::GUIPolygon2D(GUIGlObjectStorage &idStorage,
                           const std::string name, const std::string type,
                           const RGBColor &color,
                           const Position2DVector &Pos,
                           bool fill)
    : Polygon2D(name, type, color, Pos, fill),
    GUIGlObject(idStorage, "poly:"+name)
{
}


GUIPolygon2D::~GUIPolygon2D()
{
}



GUIGLObjectPopupMenu *
GUIPolygon2D::getPopUpMenu(GUIMainWindow &app,
                                 GUISUMOAbstractView &parent)
{
    GUIGLObjectPopupMenu *ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    buildNameCopyPopupEntry(ret);
//    buildSelectionPopupEntry(ret);
    return ret;
}


GUIParameterTableWindow *
GUIPolygon2D::getParameterWindow(GUIMainWindow &,
                                       GUISUMOAbstractView &)
{
    return 0;
}


GUIGlObjectType
GUIPolygon2D::getType() const
{
    return GLO_SHAPE;
}


const std::string &
GUIPolygon2D::microsimID() const
{
    throw 1;
}


bool
GUIPolygon2D::active() const
{
    return true;
}


Boundary
GUIPolygon2D::getCenteringBoundary() const
{
    Boundary b;
    b.add(myPos.getBoxBoundary());
    b.grow(10);
    return b;
}

