#ifndef GUIDialog_GLObjChooser_h
#define GUIDialog_GLObjChooser_h
//---------------------------------------------------------------------------//
//                        GUIDialog_GLObjChooser.h -
//  Class for the window that allows to choose a street, junction or vehicle
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
// Revision 1.1  2004/03/19 12:32:26  dkrajzew
// porting to FOX
//
// Revision 1.3  2003/05/20 09:23:54  dkrajzew
// some statistics added; some debugging done
//
// Revision 1.2  2003/02/07 10:34:14  dkrajzew
// files updated
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
#include <gui/GUIGlObjectTypes.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUISUMOViewParent;


/* =========================================================================
 * class definition
 * ======================================================================= */
/**
 * @class GUIDialog_GLObjChooser
 * Instances of this class are windows that display the list of instances
 * from a given artifact like vehicles, edges or junctions and allow
 * one of their items
 */
class GUIDialog_GLObjChooser : public FXMainWindow
{
    // FOX-declarations
    FXDECLARE(GUIDialog_GLObjChooser)
public:
    /// constructor
    GUIDialog_GLObjChooser(GUISUMOViewParent *parent, GUIGlObjectType type,
        std::vector<std::string> &names);

    /// destructor
    ~GUIDialog_GLObjChooser();

    /// returns the id of the chosen artifact; 0 if none was chosen
    std::string getID() const;

    FXbool close(FXbool notify=FALSE);

    long onCmdOK(FXObject*,FXSelector,void*);
    long onCmdCancel(FXObject*,FXSelector,void*);

private:
    /// the list that holds the ids
    FXList *myList;

    /// the chosen id
    std::string mySelectedID;

    /// the artifact to choose
    GUIGlObjectType myObjectType;

    /// the parent window
    GUISUMOViewParent *myParent;

protected:
    GUIDialog_GLObjChooser() { }

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

