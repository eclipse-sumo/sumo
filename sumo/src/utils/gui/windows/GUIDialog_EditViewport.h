/****************************************************************************/
/// @file    GUIDialog_EditViewport.h
/// @author  unknown_author
/// @date    unknown_date
/// @version $Id: $
///
// missing_desc
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
#ifndef GUIDialog_EditViewport_h
#define GUIDialog_EditViewport_h
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

#include <utils/foxtools/FXRealSpinDial.h>

class GUISUMOAbstractView;

class GUIDialog_EditViewport : public FXDialogBox
{
    // FOX-declarations
    FXDECLARE(GUIDialog_EditViewport)
public:
    enum {
        MID_CHANGED = FXDialogBox::ID_LAST
    };

    GUIDialog_EditViewport(GUISUMOAbstractView* parent,  const char* name,
                           SUMOReal zoom, SUMOReal xoff, SUMOReal yoff, int x, int y);
    ~GUIDialog_EditViewport();
    void create();
    long onCmdChanged(FXObject*,FXSelector,void*);
    long onCmdOk(FXObject*,FXSelector,void*);
    long onCmdCancel(FXObject*,FXSelector,void*);
    void setValues(SUMOReal zoom, SUMOReal xoff, SUMOReal yoff);
    bool haveGrabbed() const;
    void setOldValues(SUMOReal zoom, SUMOReal xoff, SUMOReal yoff);

protected:
    GUISUMOAbstractView *myParent;
    SUMOReal myOldZoom;
    SUMOReal myOldXOff, myOldYOff;
    FXRealSpinDial *myZoom, *myXOff, *myYOff;

protected:
    GUIDialog_EditViewport()
    { }

};


#endif

/****************************************************************************/

