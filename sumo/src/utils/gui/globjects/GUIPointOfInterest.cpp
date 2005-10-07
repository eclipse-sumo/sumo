#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include "GUIPointOfInterest.h"
#include <utils/gui/div/GUIParameterTableWindow.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG

using namespace std;

GUIPointOfInterest::GUIPointOfInterest(GUIGlObjectStorage &idStorage,
                                       const std::string &id,
                                       const std::string &type,
                                       const Position2D &p,
                                       const RGBColor &c)
    : PointOfInterest(id, type, p, c),
    GUIGlObject(idStorage, string("poi:")+id)
{
}


GUIPointOfInterest::~GUIPointOfInterest()
{
}


GUIGLObjectPopupMenu *
GUIPointOfInterest::getPopUpMenu(GUIMainWindow &app,
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

    return ret;*/
}


GUIParameterTableWindow *
GUIPointOfInterest::getParameterWindow(GUIMainWindow &app,
                                       GUISUMOAbstractView &parent)
{
    GUIParameterTableWindow *ret =
        new GUIParameterTableWindow(app, *this, 7);
    // add items
    /*
    ret->mkItem("speed [m/s]", true,
        new FunctionBinding<GUITriggeredRerouter, SUMOReal>(this, &GUITriggeredRerouter::getCurrentSpeed));
        */
    // close building
    ret->closeBuilding();
    return ret;
}


GUIGlObjectType
GUIPointOfInterest::getType() const
{
    return GLO_SHAPE;
}


std::string
GUIPointOfInterest::microsimID() const
{
    throw 1;
}


bool
GUIPointOfInterest::active() const
{
    return true;
}


Boundary
GUIPointOfInterest::getCenteringBoundary() const
{
    Boundary b;
    b.add(x(), y());
    b.grow(10);
    return b;
}

