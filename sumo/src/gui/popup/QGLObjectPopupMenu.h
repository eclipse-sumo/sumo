#ifndef QGLObjectPopupMenu_h
#define QGLObjectPopupMenu_h
//---------------------------------------------------------------------------//
//                        QGLObjectPopupMenu.h -
//  The popup menu which is displayed when pressing the right mouse button over
//  a gl-object
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
// $Log$
// Revision 1.2  2003/07/30 08:49:26  dkrajzew
// changed the responsibility of a GLObject
//
// Revision 1.1  2003/06/06 10:24:36  dkrajzew
// new subfolder holding popup-menus was added due to link-dependencies under linux; QGLObjectPopupMenu*-classes were moved to "popup"
//
// Revision 1.2  2003/06/05 11:37:31  dkrajzew
// class templates applied
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <qpopupmenu.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUISUMOAbstractView;
class GUIGlObject;
class GUIApplicationWindow;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class QGLObjectPopupMenu
 */
class QGLObjectPopupMenu :
        public QPopupMenu
{
    // is a q-object
    Q_OBJECT

public:
    /// Constructor
    QGLObjectPopupMenu(GUIApplicationWindow &app,
        GUISUMOAbstractView &parent, GUIGlObject &o);

    /// Destructor
    ~QGLObjectPopupMenu();

public slots:
    /// Callback called from the "center"-menu option
    void center();

    /// Callback called from the "show parameter"-menu option
    void showPars();

protected:
    /// The parent window
    GUISUMOAbstractView &myParent; // !!! needed?

    /// The object that belongs to this popup-menu
    GUIGlObject &myObject;

    /// The main application
    GUIApplicationWindow &myApplication;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "QGLObjectPopupMenu.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

