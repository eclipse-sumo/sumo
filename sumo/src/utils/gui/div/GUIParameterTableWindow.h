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
// Revision 1.1  2004/11/23 10:38:29  dkrajzew
// debugging
//
// Revision 1.1  2004/10/22 12:50:47  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.6  2004/03/19 12:40:14  dkrajzew
// porting to FOX
//
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
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <vector>
#include <string>
#include <fx.h>
#include <helpers/ValueSource.h>
#include "GUIParameterTableItem.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUIGlObject;
class GUIMainWindow;


/* =========================================================================
 * class definitions
 * ======================================================================= */
class GUIParameterTableWindow : public FXMainWindow
{
    FXDECLARE(GUIParameterTableWindow)
public:
    /// Constructor
    GUIParameterTableWindow(GUIMainWindow &app,
        GUIGlObject &o, size_t noRows);

    /// Destructor
    ~GUIParameterTableWindow();

    void closeBuilding();

    void mkItem(const char *name, bool dynamic, ValueSource<double> *src);

    void mkItem(const char *name, bool dynamic, std::string value);

    void mkItem(const char *name, bool dynamic, double value);

    long onSimStep(FXObject*,FXSelector,void*);
    long onTableSelected(FXObject*,FXSelector,void*);
    long onTableDeselected(FXObject*,FXSelector,void*);
    long onRightButtonPress(FXObject*,FXSelector,void*);

protected:

protected:

    void updateTable();

    /// The object to get the information from
    GUIGlObject *myObject;

    /// The table to display the information in
    FXTable *myTable;

    /// The main application
    GUIMainWindow *myApplication;

    std::vector<GUIParameterTableItem*> myItems;

    size_t myCurrentPos;

protected:
    GUIParameterTableWindow() { }
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

