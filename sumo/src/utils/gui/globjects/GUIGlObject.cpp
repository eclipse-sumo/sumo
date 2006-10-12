//---------------------------------------------------------------------------//
//                        GUIGlObject.cpp -
//  Base class for all objects that may be displayed within the openGL-gui
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.6  2006/10/12 07:57:14  dkrajzew
// added the possibility to copy an artefact's (gl-object's) name to clipboard (windows)
//
// Revision 1.5  2006/04/18 08:12:05  dkrajzew
// consolidation of interaction with gl-objects
//
// Revision 1.4  2005/10/07 11:45:32  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.3  2005/09/23 06:08:31  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.2  2005/09/15 12:19:44  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.1  2004/11/23 10:38:31  dkrajzew
// debugging
//
// Revision 1.1  2004/10/22 12:50:50  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.11  2004/03/19 12:54:07  dkrajzew
// porting to FOX
//
// Revision 1.10  2003/11/18 14:28:14  dkrajzew
// debugged and completed lane merging detectors
//
// Revision 1.9  2003/11/12 14:07:46  dkrajzew
// clean up after recent changes
//
// Revision 1.8  2003/07/30 08:52:16  dkrajzew
// further work on visualisation of all geometrical objects
//
// Revision 1.7  2003/07/22 14:56:46  dkrajzew
// changes due to new detector handling
//
// Revision 1.6  2003/06/06 10:28:45  dkrajzew
// new subfolder holding popup-menus was added due to link-dependencies
//  under linux; QGLObjectPopupMenu*-classes were moved to "popup"
//
// Revision 1.5  2003/06/05 11:37:30  dkrajzew
// class templates applied
//
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
#include <stack>
#include <utils/common/ToString.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include "GUIGlObject.h"
#include "GUIGlObjectStorage.h"
#include <utils/foxtools/MFXMenuHeader.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <gui/GUIApplicationWindow.h>
#include <utils/gui/div/GUIGlobalSelection.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUIGlObject::GUIGlObject(GUIGlObjectStorage &idStorage,
                         std::string fullName)
    : myFullName(fullName)
{
    idStorage.registerObject(this);
}


GUIGlObject::GUIGlObject(GUIGlObjectStorage &idStorage,
                         std::string fullName, size_t glID)
    : myFullName(fullName)
{
    idStorage.registerObject(this, glID);
}


GUIGlObject::~GUIGlObject()
{
}


const std::string &
GUIGlObject::getFullName() const
{
    return myFullName;
}


size_t
GUIGlObject::getGlID() const
{
    return myGlID;
}


void
GUIGlObject::setGlID(size_t id)
{
    myGlID = id;
}



void
GUIGlObject::buildPopupHeader(GUIGLObjectPopupMenu *ret,
                              GUIMainWindow &app,
                              bool addSeparator)
{
    new MFXMenuHeader(ret, app.getBoldFont(), getFullName().c_str(), 0, 0, 0);
    if(addSeparator) {
        new FXMenuSeparator(ret);
    }
}


void
GUIGlObject::buildCenterPopupEntry(GUIGLObjectPopupMenu *ret,
                                   bool addSeparator)
{
    new FXMenuCommand(ret, "Center",
        GUIIconSubSys::getIcon(ICON_RECENTERVIEW), ret, MID_CENTER);
    if(addSeparator) {
        new FXMenuSeparator(ret);
    }
}


void
GUIGlObject::buildNameCopyPopupEntry(GUIGLObjectPopupMenu *ret,
                                     bool addSeparator)
{
    new FXMenuCommand(ret, "Copy name to clipboard",
        0, ret, MID_COPY_NAME);
    new FXMenuCommand(ret, "Copy typed name to clipboard",
        0, ret, MID_COPY_TYPED_NAME);
    if(addSeparator) {
        new FXMenuSeparator(ret);
    }
}


void
GUIGlObject::buildSelectionPopupEntry(GUIGLObjectPopupMenu *ret,
                                      bool addSeparator)
{
    if(gSelected.isSelected(getType(), getGlID())) {
        new FXMenuCommand(ret, "Remove From Selected",
            GUIIconSubSys::getIcon(ICON_FLAG_MINUS), ret, MID_REMOVESELECT);
    } else {
        new FXMenuCommand(ret, "Add To Selected",
            GUIIconSubSys::getIcon(ICON_FLAG_PLUS), ret, MID_ADDSELECT);
    }
    if(addSeparator) {
        new FXMenuSeparator(ret);
    }
}


void
GUIGlObject::buildShowParamsPopupEntry(GUIGLObjectPopupMenu *ret,
                                       bool addSeparator)
{
    new FXMenuCommand(ret, "Show Parameter",
        GUIIconSubSys::getIcon(ICON_APP_TABLE), ret, MID_SHOWPARS);
    if(addSeparator) {
        new FXMenuSeparator(ret);
    }
}


void
GUIGlObject::buildShowManipulatorPopupEntry(GUIGLObjectPopupMenu *ret,
                                            bool addSeparator)
{
    new FXMenuCommand(ret, "Open Manipulator...",
        GUIIconSubSys::getIcon(ICON_MANIP), ret, MID_MANIP);
    if(addSeparator) {
        new FXMenuSeparator(ret);
    }
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


