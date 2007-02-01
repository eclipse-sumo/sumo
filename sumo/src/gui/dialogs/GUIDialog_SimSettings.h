/****************************************************************************/
/// @file    GUIDialog_SimSettings.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id: $
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIDialog_SimSettings_h
#define GUIDialog_SimSettings_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>

// ===========================================================================
// class definitions
// ===========================================================================
class GUIDialog_SimSettings : public FXDialogBox
{
    FXDECLARE(GUIDialog_SimSettings)
public:
    /// Constructor
    GUIDialog_SimSettings(FXMainWindow* parent, bool *quitOnEnd,
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
    GUIDialog_SimSettings()
    { }
};


#endif

/****************************************************************************/

