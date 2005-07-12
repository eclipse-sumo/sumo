#ifndef GUIDialog_SimSettings_h
#define GUIDialog_SimSettings_h
//---------------------------------------------------------------------------//
//                        GUIDialog_SimSettings.h -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Fri, 29.04.2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
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
//

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#include <fx.h>

/* =========================================================================
 * class definitions
 * ======================================================================= */
class GUIDialog_SimSettings : public FXDialogBox
{
    FXDECLARE(GUIDialog_SimSettings)
public:
	/// Constructor
    GUIDialog_SimSettings( FXMainWindow* parent, bool *quitOnEnd,
        bool *surpressEnd, bool *allowFloating);
	/// Destructor
    ~GUIDialog_SimSettings();
    long onCmdOk(FXObject*,FXSelector,void*);
    long onCmdCancel(FXObject*,FXSelector,void*);
    long onCmdQuitOnEnd(FXObject*,FXSelector,void*);
    long onCmdSurpressEnd(FXObject*,FXSelector,void*);
    long onCmdAllowAggregated(FXObject*,FXSelector,void*);

public:
private:
    bool myAppQuitOnEnd;
    bool mySurpressEnd;
    bool myAllowFloating;
    bool *mySetAppQuitOnEnd;
    bool *mySetSurpressEnd;
    bool *mySetAllowFloating;

protected:
    GUIDialog_SimSettings() { }
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
