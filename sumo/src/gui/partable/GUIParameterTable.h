#ifndef GUIParameterTable_h
#define GUIParameterTable_h
//---------------------------------------------------------------------------//
//                        GUIParameterTable.h -
//  A qt-table holding the parameters of an gl-object
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
// Revision 1.2  2003/06/05 11:38:20  dkrajzew
// class templates applied; documentation added
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <qlistview.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUIApplicationWindow;
class GUIGlObject;


/* =========================================================================
 * class definitions
 * ======================================================================= */
class GUIParameterTable : public QListView
{
    // is a q-object
    Q_OBJECT
public:
    /// Constructor
    GUIParameterTable( GUIApplicationWindow *app,
        GUIGlObject *o, QWidget * parent = 0, const char *name = 0 );

    /// Destructor
    ~GUIParameterTable();

protected:
    /// Callback called whe the user selects an entry
    void contentsMousePressEvent( QMouseEvent * e );
//    void contentsMouseReleaseEvent( QMouseEvent * e );


signals:
    void mySelectionChanged();
    void mySelectionChanged( QListViewItem* );

private:
    /// The object which values are displayed
	GUIGlObject *myObject;

    /// The currently selected item
    QListViewItem* selected;

    /// The main application
    GUIApplicationWindow *myApplication;

};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "GUIParameterTable.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

