#ifndef QGUIToggleButton_h
#define QGUIToggleButton_h
//---------------------------------------------------------------------------//
//                        QGUIToggleButton.h -
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
// Revision 1.2  2003/02/07 10:34:15  dkrajzew
// files updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <qtoolbutton.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * QGUIToggleButton
 * Just an implementation of a button that may be toggled.
 * A Qt-button can only be set to be toggleable (not to be only a pushbutton,
 * but to keep it's state) via a protected method!?
 */
class QGUIToggleButton : public QToolButton {
    // is a q-object
    Q_OBJECT
public:
    /// constructor
    QGUIToggleButton(const QPixmap & pm, const QString & textLabel,
        const QString & grouptext, QObject * receiver,
        const char * slot, QToolBar * parent, const char * name,
        bool isOn)
        : QToolButton(pm, textLabel, grouptext, receiver, slot, parent, name)
    {
        setToggleButton(true);
        setOn(isOn);
    }

    /// destructor
    ~QGUIToggleButton() { }

    /// possibility to set the button into the "on" state from outside
    void publicSetOn(bool value)
    {
        setOn(value);
    }

};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "QGUIToggleButton.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

