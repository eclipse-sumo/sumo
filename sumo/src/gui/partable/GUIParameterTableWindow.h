#ifndef GUIParameterTableWindow_h
#define GUIParameterTableWindow_h
//---------------------------------------------------------------------------//
//                        GUIParameterTableWindow.h -
//  The window that holds the table of an object's parameter
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
// Revision 1.5  2003/11/12 14:09:13  dkrajzew
// clean up after recent changes; comments added
//
// Revision 1.4  2003/07/30 08:48:28  dkrajzew
// new parameter table usage paradigm; undocummented yet
//
// Revision 1.3  2003/07/18 12:30:14  dkrajzew
// removed some warnings
//
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

#include <vector>
#include <string>
#include <qdialog.h>
#include <helpers/ValueSource.h>
#include "GUIParameterTableItem.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class QListViewItem;
class GUIGlObject;
class QListView;
class GUIApplicationWindow;
class QPaintEvent;
class QListViewItem;
class QEvent;
class QResizeEvent;
class GUIParameterTable;


/* =========================================================================
 * class definitions
 * ======================================================================= */
class GUIParameterTableWindow : public QDialog
{
    // is q-object
    Q_OBJECT
public:
    /// Constructor
    GUIParameterTableWindow( GUIApplicationWindow &app, GUIGlObject &o);

    /// Destructor
    ~GUIParameterTableWindow();

    virtual void buildParameterPopUp(QMouseEvent * e, GUIParameterTableItem *i);

    void closeBuilding();

    void mkItem(const char *name, bool dynamic, ValueSource<double> *src);

    void mkItem(const char *name, bool dynamic, std::string value);

    void mkItem(const char *name, bool dynamic, double value);

protected:
    /// Callback for events
    bool event ( QEvent *e );

    /// Callback for resize-events
    void resizeEvent ( QResizeEvent * );

protected:
//    virtual size_t getParameterNo() const = 0;

    void updateTable();

    /// The object to get the information from
    GUIGlObject &myObject;

    /// The table to display the information in
    GUIParameterTable *myTable;

    /// A list of current parameter
//    double *myParameter;

    /// A backup for current parameter so that only those will be reset which have changed
//    double *myParameterBuffer;

    /// The main application
    GUIApplicationWindow &myApplication;

    /// The list of parameter names
//    GUIParameterTableItem **myItems;

    std::vector<GUIParameterTableItem*> myItems;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "GUIParameterTableWindow.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

