#ifndef QGUIImageField_h
#define QGUIImageField_h
//---------------------------------------------------------------------------//
//                        QGUIImageField.h -
//  Although originally a toolbutton, this just is a holder of pictures
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
// Revision 1.1  2003/02/07 10:34:15  dkrajzew
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
 * QGUIImageField
 * Allows adding pixmaps without a function to a toolbar
 */
class QGUIImageField : public QToolButton {
    // is a q-object
    Q_OBJECT
public:
    /// constructor
    QGUIImageField(const QPixmap & pm, const QString & textLabel,
        const QString & grouptext, QToolBar * parent, const char * name)
        : QToolButton(pm, textLabel, grouptext, 0, 0, parent, name)
    {
        setAutoRaise(true);
    }

    /// destructor
    ~QGUIImageField() { }
};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "QGUIImageField.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

