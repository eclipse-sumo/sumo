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
// Revision 1.5  2003/11/12 14:00:19  dkrajzew
// commets added; added parameter windows to all detectors
//
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "GUIDetectorWrapper.h"
#include <gui/popup/QGLObjectPopupMenuItem.h>
#include <gui/popup/QGLObjectPopupMenu.h>
#include <gui/GUISUMOAbstractView.h>
#include <gui/partable/GUIParameterTableWindow.h>


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUIDetectorWrapper::GUIDetectorWrapper(GUIGlObjectStorage &idStorage,
                                       std::string id)
                                       : GUIGlObject(idStorage, id)
{
}


GUIDetectorWrapper::~GUIDetectorWrapper()
{
}


QGLObjectPopupMenu *
GUIDetectorWrapper::getPopUpMenu(GUIApplicationWindow &app,
                                 GUISUMOAbstractView &parent)
{
    QGLObjectPopupMenu *ret = new QGLObjectPopupMenu(app, parent, *this);
    int id = ret->insertItem(
        new QGLObjectPopupMenuItem(ret, getFullName().c_str(), true));
    ret->insertSeparator();
    // add view options
    id = ret->insertItem("Center", ret, SLOT(center()));
    ret->insertSeparator();
    id = ret->insertItem("Show Parameter", ret, SLOT(showPars()));
    ret->setItemEnabled(id, TRUE);
    // add views adding options
    ret->insertSeparator();
    id = ret->insertItem("Open ValueTracker");
    ret->setItemEnabled(id, FALSE);
    return ret;
}


GUIGlObjectType
GUIDetectorWrapper::getType() const
{
    return GLO_DETECTOR;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUIDetectorWrapper.icc"
//#endif

// Local Variables:
// mode:C++
// End:
