/****************************************************************************/
/// @file    GUIGlObject.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Base class for all objects that may be displayed within the openGL-gui
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

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

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
GUIGlObject::GUIGlObject(GUIGlObjectStorage &idStorage,
                         std::string fullName) throw()
        : myFullName(fullName)
{
    idStorage.registerObject(this);
}


GUIGlObject::GUIGlObject(GUIGlObjectStorage &idStorage,
                         std::string fullName, GLuint glID) throw()
        : myFullName(fullName)
{
    idStorage.registerObject(this, glID);
}


GUIGlObject::~GUIGlObject() throw()
{}


const std::string &
GUIGlObject::getFullName() const throw()
{
    return myFullName;
}


GLuint
GUIGlObject::getGlID() const throw()
{
    return myGlID;
}


void
GUIGlObject::setGlID(GLuint id) throw()
{
    myGlID = id;
}



void
GUIGlObject::buildPopupHeader(GUIGLObjectPopupMenu *ret,
                              GUIMainWindow &app,
                              bool addSeparator) throw()
{
    new MFXMenuHeader(ret, app.getBoldFont(), getFullName().c_str(), 0, 0, 0);
    if (addSeparator) {
        new FXMenuSeparator(ret);
    }
}


void
GUIGlObject::buildCenterPopupEntry(GUIGLObjectPopupMenu *ret,
                                   bool addSeparator) throw()
{
    new FXMenuCommand(ret, "Center",
                      GUIIconSubSys::getIcon(ICON_RECENTERVIEW), ret, MID_CENTER);
    if (addSeparator) {
        new FXMenuSeparator(ret);
    }
}


void
GUIGlObject::buildNameCopyPopupEntry(GUIGLObjectPopupMenu *ret,
                                     bool addSeparator) throw()
{
    new FXMenuCommand(ret, "Copy name to clipboard",
                      0, ret, MID_COPY_NAME);
    new FXMenuCommand(ret, "Copy typed name to clipboard",
                      0, ret, MID_COPY_TYPED_NAME);
    if (addSeparator) {
        new FXMenuSeparator(ret);
    }
}


void
GUIGlObject::buildSelectionPopupEntry(GUIGLObjectPopupMenu *ret,
                                      bool addSeparator) throw()
{
    if (gSelected.isSelected(getType(), getGlID())) {
        new FXMenuCommand(ret, "Remove From Selected",
                          GUIIconSubSys::getIcon(ICON_FLAG_MINUS), ret, MID_REMOVESELECT);
    } else {
        new FXMenuCommand(ret, "Add To Selected",
                          GUIIconSubSys::getIcon(ICON_FLAG_PLUS), ret, MID_ADDSELECT);
    }
    if (addSeparator) {
        new FXMenuSeparator(ret);
    }
}


void
GUIGlObject::buildShowParamsPopupEntry(GUIGLObjectPopupMenu *ret,
                                       bool addSeparator) throw()
{
    new FXMenuCommand(ret, "Show Parameter",
                      GUIIconSubSys::getIcon(ICON_APP_TABLE), ret, MID_SHOWPARS);
    if (addSeparator) {
        new FXMenuSeparator(ret);
    }
}


void
GUIGlObject::buildShowManipulatorPopupEntry(GUIGLObjectPopupMenu *ret,
        bool addSeparator) throw()
{
    new FXMenuCommand(ret, "Open Manipulator...",
                      GUIIconSubSys::getIcon(ICON_MANIP), ret, MID_MANIP);
    if (addSeparator) {
        new FXMenuSeparator(ret);
    }
}



/****************************************************************************/

