#ifndef QGLObjectPopupMenuItem_h
#define QGLObjectPopupMenuItem_h
//---------------------------------------------------------------------------//
//                        QGLObjectPopupMenuItem.h -
//  A single entry within the popup-menu
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

#include <string>
#include <qfont.h>
#include <qmenudata.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
class QGLObjectPopupMenuItem :
    public QCustomMenuItem
{
public:
    /// Constructor
    QGLObjectPopupMenuItem(QPopupMenu *parent, const std::string &name,
        bool bold=false);

    /// destructor
    ~QGLObjectPopupMenuItem();

    /// draws the menu item
    void paint ( QPainter * p, const QColorGroup & cg, bool act, bool enabled, int x, int y, int w, int h ) ;

    /// returns the size of the item
    QSize sizeHint ();

private:
    /// The displayed name
    std::string myName;

    /// Information about the style of the text
    QFont myFont;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "QGLObjectPopupMenuItem.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

