#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include "GUIPointOfInterest.h"
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/windows/GUIAppEnum.h>

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
    GUIGlObject(idStorage, "poi:"+id)
{
}


GUIPointOfInterest::~GUIPointOfInterest()
{
}


GUIGLObjectPopupMenu *
GUIPointOfInterest::getPopUpMenu(GUIMainWindow &app,
                                 GUISUMOAbstractView &parent)
{

    GUIGLObjectPopupMenu *ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    //
    new FXMenuSeparator(ret);
    new FXMenuCommand(ret, "Rename ",
        GUIIconSubSys::getIcon(ICON_MANIP), ret, MID_RENAME);
    new FXMenuCommand(ret, "Move To ",
        GUIIconSubSys::getIcon(ICON_MANIP), ret, MID_MOVETO);
    new FXMenuSeparator(ret);
    new FXMenuCommand(ret, "Change Color ",
        GUIIconSubSys::getIcon(ICON_MANIP), ret, MID_CHANGECOL);
    new FXMenuSeparator(ret);
    new FXMenuCommand(ret, "Change Type",
        GUIIconSubSys::getIcon(ICON_MANIP), ret, MID_CHANGETYPE);
    new FXMenuSeparator(ret);
    new FXMenuCommand(ret, "Delete",
        GUIIconSubSys::getIcon(ICON_CLOSING), ret, MID_DELETE);
    new FXMenuSeparator(ret);
    //
    buildSelectionPopupEntry(ret);
    return ret;
}


GUIParameterTableWindow *
GUIPointOfInterest::getParameterWindow(GUIMainWindow &app,
                                       GUISUMOAbstractView &parent)
{
    return 0;
}


GUIGlObjectType
GUIPointOfInterest::getType() const
{
    return GLO_SHAPE;
}


const std::string &
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

