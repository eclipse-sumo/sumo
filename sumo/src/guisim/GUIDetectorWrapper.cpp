//---------------------------------------------------------------------------//
//                        GUIDetectorWrapper.cpp -
//  The base class for detector wrapper
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
// Revision 1.10  2004/11/24 08:46:42  dkrajzew
// recent changes applied
//
// Revision 1.9  2004/07/02 08:41:00  dkrajzew
// using global selection storage
//
// Revision 1.8  2004/04/02 11:18:37  dkrajzew
// recenter view - icon added to the popup menu
//
// Revision 1.7  2004/03/19 12:57:54  dkrajzew
// porting to FOX
//
// Revision 1.6  2003/11/18 14:27:39  dkrajzew
// debugged and completed lane merging detectors
//
// Revision 1.5  2003/11/12 14:00:19  dkrajzew
// commets added; added parameter windows to all detectors
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "GUIDetectorWrapper.h"
#include <gui/GUIApplicationWindow.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/foxtools/MFXMenuHeader.h>


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUIDetectorWrapper::GUIDetectorWrapper(GUIGlObjectStorage &idStorage,
                                       std::string id)
    : GUIGlObject_AbstractAdd(idStorage, id, GLO_DETECTOR)
{
}


GUIDetectorWrapper::GUIDetectorWrapper(GUIGlObjectStorage &idStorage,
                                       std::string id,
                                       size_t glID)
    : GUIGlObject_AbstractAdd(idStorage, id, glID, GLO_DETECTOR)
{
}


GUIDetectorWrapper::~GUIDetectorWrapper()
{
}


GUIGLObjectPopupMenu *
GUIDetectorWrapper::getPopUpMenu(GUIMainWindow &app,
                                 GUISUMOAbstractView &parent)
{
    GUIGLObjectPopupMenu *ret = new GUIGLObjectPopupMenu(app, parent, *this);
    new MFXMenuHeader(ret, app.getBoldFont(), getFullName().c_str(), 0, 0, 0);
    new FXMenuSeparator(ret);
    //
    new FXMenuCommand(ret, "Center",
        GUIIconSubSys::getIcon(ICON_RECENTERVIEW), ret, MID_CENTER);
    new FXMenuSeparator(ret);
    //
    if(gSelected.isSelected(GLO_DETECTOR, getGlID())) {
        new FXMenuCommand(ret, "Remove From Selected",
            GUIIconSubSys::getIcon(ICON_FLAG_MINUS), ret, MID_REMOVESELECT);
    } else {
        new FXMenuCommand(ret, "Add To Selected",
            GUIIconSubSys::getIcon(ICON_FLAG_PLUS), ret, MID_ADDSELECT);
    }
    new FXMenuSeparator(ret);
    //
    new FXMenuCommand(ret, "Show Parameter", 0, ret, MID_SHOWPARS);
    return ret;
}


GUIGlObjectType
GUIDetectorWrapper::getType() const
{
    return GLO_DETECTOR;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
