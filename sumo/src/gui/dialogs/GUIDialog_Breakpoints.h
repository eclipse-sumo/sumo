#ifndef GUIDialog_Breakpoints_h
#define GUIDialog_Breakpoints_h
//---------------------------------------------------------------------------//
//                        GUIDialog_Breakpoints.h -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Thu, 17 Jun 2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
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
// Revision 1.2  2004/11/23 10:00:08  dkrajzew
// new class hierarchy for windows applied
//
// Revision 1.1  2004/07/02 08:10:56  dkrajzew
// edition of breakpoints and additional weights added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <vector>
#include <fx.h>
#include <gui/GUIAddWeightsStorage.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUIApplicationWindow;
class MFXAddEditTypedTable;


/* =========================================================================
 * class definition
 * ======================================================================= */
/**
 * @class GUIDialog_Breakpoints
 * Instances of this class are windows that display the list of instances
 * from a given artifact like vehicles, edges or junctions and allow
 * one of their items
 */
class GUIDialog_Breakpoints : public FXMainWindow
{
    // FOX-declarations
    FXDECLARE(GUIDialog_Breakpoints)
public:
    /// constructor
    GUIDialog_Breakpoints(GUIMainWindow *parent);

    /// destructor
    ~GUIDialog_Breakpoints();

    FXbool close(FXbool notify=FALSE);

    long onCmdLoad(FXObject*,FXSelector,void*);
    long onCmdSave(FXObject*,FXSelector,void*);
    long onCmdDeselect(FXObject*,FXSelector,void*);
    long onCmdClear(FXObject*,FXSelector,void*);
    long onCmdClose(FXObject*,FXSelector,void*);
    long onCmdEditTable(FXObject*,FXSelector,void*);
//    long onUpdSave(FXObject*,FXSelector,void*);

private:
    void rebuildList();
    std::string encode2TXT();

    class time_sorter {
        public:
            /// constructor
            explicit time_sorter() { }

            int operator() (const int &p1, const int &p2) {
                return p1<p2;
            }
        };

private:
    /// the list that holds the ids
    MFXAddEditTypedTable *myTable;

    /// the parent window
    GUIMainWindow *myParent;

protected:
    GUIDialog_Breakpoints() { }

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

