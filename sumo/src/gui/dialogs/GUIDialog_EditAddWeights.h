#ifndef GUIDialog_EditAddWeights_h
#define GUIDialog_EditAddWeights_h
//---------------------------------------------------------------------------//
//                        GUIDialog_EditAddWeights.h -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Mon, 16 Jun 2004
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
// Revision 1.3  2005/04/27 09:44:26  dkrajzew
// level3 warnings removed
//
// Revision 1.2  2004/11/23 10:00:08  dkrajzew
// new class hierarchy for windows applied
//
// Revision 1.1  2004/07/02 08:10:56  dkrajzew
// edition of breakpoints and additional weights added
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
 * @class GUIDialog_EditAddWeights
 * Instances of this class are windows that display the list of instances
 * from a given artifact like vehicles, edges or junctions and allow
 * one of their items
 */
class GUIDialog_EditAddWeights : public FXMainWindow
{
    // FOX-declarations
    FXDECLARE(GUIDialog_EditAddWeights)
public:
    /// constructor
    GUIDialog_EditAddWeights(FXWindow *parent);

    /// destructor
    ~GUIDialog_EditAddWeights();

    FXbool close(FXbool notify=FALSE);

    long onCmdLoad(FXObject*,FXSelector,void*);
    long onCmdSave(FXObject*,FXSelector,void*);
    long onCmdDeselect(FXObject*,FXSelector,void*);
    long onCmdClear(FXObject*,FXSelector,void*);
    long onCmdClose(FXObject*,FXSelector,void*);
    long onCmdEditTable(FXObject*,FXSelector,void*);
    long onUpdSave(FXObject*,FXSelector,void*);

private:
    void rebuildList();
    std::string encode2XML();

    class time_sorter {
        public:
            /// constructor
            explicit time_sorter() { }

            int operator() (const GUIAddWeight &p1, const GUIAddWeight &p2) {
                return p1.timeBeg<p2.timeBeg;
            }
        };

private:
    /// the list that holds the ids
    MFXAddEditTypedTable *myTable;

    /// the parent window
//    GUIApplicationWindow *myParent;

    ///
    bool myEntriesAreValid;

protected:
    GUIDialog_EditAddWeights() { }

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

