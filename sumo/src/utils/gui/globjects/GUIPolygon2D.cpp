
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)

/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <string>
#include "GUIPolygon2D.h"
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/div/GUIParameterTableWindow.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


using namespace std;


GUIPolygon2D::GUIPolygon2D(GUIGlObjectStorage &idStorage,
                           const std::string name, const std::string type,
                           const RGBColor &color,
                           const Position2DVector &Pos)
    : Polygon2D(name, type, color, Pos), GUIGlObject(idStorage, string("poly:")+name)
{
}


GUIPolygon2D::~GUIPolygon2D()
{
}



GUIGLObjectPopupMenu *
GUIPolygon2D::getPopUpMenu(GUIMainWindow &app,
                                 GUISUMOAbstractView &parent)
{
    throw 1;
    /*
    GUIGLObjectPopupMenu *ret =
        new GUITriggeredRerouterPopupMenu(app, parent, *this);
    new MFXMenuHeader(ret, app.getBoldFont(), getFullName().c_str(), 0, 0, 0);
    new FXMenuSeparator(ret);
    //
    new FXMenuCommand(ret, "Center",
        GUIIconSubSys::getIcon(ICON_RECENTERVIEW), ret, MID_CENTER);
    /*
    new FXMenuSeparator(ret);
    //
    new FXMenuCommand(ret, "Open Manipulator...",
        GUIIconSubSys::getIcon(ICON_MANIP), ret, MID_MANIP);
    //
    if(gSelected.isSelected(GLO_TRIGGER, getGlID())) {
        new FXMenuCommand(ret, "Remove From Selected",
            GUIIconSubSys::getIcon(ICON_FLAG_MINUS), ret, MID_REMOVESELECT);
    } else {
        new FXMenuCommand(ret, "Add To Selected",
            GUIIconSubSys::getIcon(ICON_FLAG_PLUS), ret, MID_ADDSELECT);
    }
    new FXMenuSeparator(ret);
    //
    new FXMenuCommand(ret, "Show Parameter",
        GUIIconSubSys::getIcon(ICON_APP_TABLE), ret, MID_SHOWPARS);

    return ret;
    */
}


GUIParameterTableWindow *
GUIPolygon2D::getParameterWindow(GUIMainWindow &app,
                                       GUISUMOAbstractView &parent)
{
    GUIParameterTableWindow *ret =
        new GUIParameterTableWindow(app, *this, 7);
    // add items
    /*
    ret->mkItem("speed [m/s]", true,
        new FunctionBinding<GUITriggeredRerouter, double>(this, &GUITriggeredRerouter::getCurrentSpeed));
        */
    // close building
    ret->closeBuilding();
    return ret;
}


GUIGlObjectType
GUIPolygon2D::getType() const
{
    return GLO_SHAPE;
}


std::string
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

