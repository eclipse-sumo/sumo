#ifndef GUIParam_PopupMenu_h
#define GUIParam_PopupMenu_h
/***************************************************************************
                          GUIParam_PopupMenu.cpp
    The popup-menu which appears hen pressing right mouse button over a
     parameter table
                             -------------------
    project              : SUMO - Simulation of Urban MObility
    begin                : Mai 2003
    copyright            : (C) 2003 by Daniel Krajzewicz
    organisation         : IVF/DLR http://ivf.dlr.de
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.2  2005/09/15 12:18:59  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.1  2004/11/23 10:38:29  dkrajzew
// debugging
//
// Revision 1.1  2004/10/22 12:50:47  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.1  2004/03/19 12:40:14  dkrajzew
// porting to FOX
//
// Revision 1.3  2003/11/12 14:09:13  dkrajzew
// clean up after recent changes; comments added
//
// Revision 1.2  2003/07/30 08:48:28  dkrajzew
// new parameter table usage paradigm; undocummented yet
//
// Revision 1.1  2003/05/20 09:23:58  dkrajzew
// some statistics added; some debugging done
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <fx.h>
#include <utils/helpers/ValueSource.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
//class GUIParameterTable;
class GUIParameterTableWindow;
class GUIGlObject;
class GUIMainWindow;


/* =========================================================================
 * class declarataions
 * ======================================================================= */
/**
 * @class GUIParam_PopupMenu
 * A popup menu holding the context of a parameter table entry
 */
class GUIParam_PopupMenu : public FXMenuPane
{
    FXDECLARE(GUIParam_PopupMenu)
public:
    /// Constructor
    GUIParam_PopupMenu(GUIMainWindow &app,
        /*GUIParameterTable &parent, */GUIParameterTableWindow &parentWindow,
        GUIGlObject &o, const std::string &varName, ValueSource<double> *src);

    /// Destructor
    ~GUIParam_PopupMenu();

    long onCmdOpenTracker(FXObject*,FXSelector,void*);

private:
    /// The object the table displays
    GUIGlObject *myObject;

//    GUIParameterTable *myParent;

    GUIParameterTableWindow *myParentWindow;

    /** @brief The main application window
        holder of some needed values */
    GUIMainWindow *myApplication;

    std::string myVarName;


    ValueSource<double> *mySource;

protected:
    GUIParam_PopupMenu() { }

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

