//---------------------------------------------------------------------------//
//                        QGUIToggleButton.cpp -
//  An implementation of a button that may be toggled
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
// Revision 1.2  2003/06/05 11:37:31  dkrajzew
// class templates applied
//
// Revision 1.1  2003/06/05 06:26:16  dkrajzew
// first tries to build under linux: warnings removed; Makefiles added
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <qtoolbutton.h>
#include "QGUIToggleButton.h"

#ifndef WIN32
#include "QGUIToggleButton.moc"
#endif


QGUIToggleButton::QGUIToggleButton(const QPixmap & pm,
				   const QString & textLabel,
				   const QString & grouptext,
				   QObject * receiver,
				   const char * slot,
				   QToolBar * parent, const char * name,
				   bool isOn)
    : QToolButton(pm, textLabel, grouptext, receiver, slot, parent, name)
{
    setToggleButton(true);
    setOn(isOn);
}


QGUIToggleButton::~QGUIToggleButton()
{
}

void
QGUIToggleButton::publicSetOn(bool value)
{
    setOn(value);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "QGUIToggleButton.icc"
//#endif

// Local Variables:
// mode:C++
// End:


