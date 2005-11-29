//---------------------------------------------------------------------------//
//                        GUIDialog_EditViewport.cpp -
//  A dialog to change the viewport
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Mon, 25.04.2005
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.6  2005/11/29 13:34:47  dkrajzew
// viewport debugged
//
// Revision 1.5  2005/11/09 06:46:52  dkrajzew
// some further work on viewport changer (unfinished)
//
// Revision 1.4  2005/10/07 11:46:08  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.3  2005/09/23 06:11:14  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.2  2005/09/15 12:20:19  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.1  2005/05/04 09:23:41  dkrajzew
// entries for viewport definition added; popups now popup faster
//
// Revision 1.1  2005/04/26 07:35:59  dksumo
// SUMOTime inserted; level3 warnings patched; port to fox1.4
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include "GUIDialog_EditViewport.h"
#include <utils/gui/windows/GUIAppGlobals.h>
#include "GUISUMOAbstractView.h"
#include <utils/foxtools/FXRealSpinDial.h>
#include <utils/gui/windows/GUIAppEnum.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG



FXDEFMAP(GUIDialog_EditViewport) GUIDialog_EditViewportMap[]={
    FXMAPFUNC(SEL_COMMAND, GUIDialog_EditViewport::MID_CHANGED, GUIDialog_EditViewport::onCmdChanged),
    FXMAPFUNC(SEL_COMMAND, MID_OK,                              GUIDialog_EditViewport::onCmdOk),
    FXMAPFUNC(SEL_COMMAND, MID_CANCEL,                          GUIDialog_EditViewport::onCmdCancel),
};


// Object implementation
FXIMPLEMENT(GUIDialog_EditViewport,FXDialogBox,GUIDialog_EditViewportMap, ARRAYNUMBER(GUIDialog_EditViewportMap))

/* =========================================================================
 * method definitions
 * ======================================================================= */
GUIDialog_EditViewport::GUIDialog_EditViewport(GUISUMOAbstractView* parent,
                                               const char* name,
                                               SUMOReal zoom,
                                               SUMOReal xoff, SUMOReal yoff,
                                               int x, int y)
    : FXDialogBox( parent, name, DECOR_CLOSE|DECOR_TITLE, x, y, 0, 0),
    myParent(parent), myOldZoom(zoom), myOldXOff(xoff), myOldYOff(yoff)
{
    FXVerticalFrame *f1 =
        new FXVerticalFrame(this,
            LAYOUT_TOP|FRAME_NONE|LAYOUT_FILL_X,
            0,0,0,0, 0,0,1,1);
    FXMatrix *m1 = new FXMatrix(f1, 2, MATRIX_BY_COLUMNS);
    //
    {
        new FXLabel(m1, "Zoom:", 0, LAYOUT_CENTER_Y);
        myZoom = new FXRealSpinDial(m1, 16, this, MID_CHANGED,
            LAYOUT_CENTER_Y|LAYOUT_TOP|FRAME_SUNKEN|FRAME_THICK);
        myZoom->setRange(0.0001, 100000);
        myZoom->setNumberFormat(4);
        myZoom->setValue(zoom);
    }
    {
        new FXLabel(m1, "X Offset:", 0, LAYOUT_CENTER_Y);
        myXOff = new FXRealSpinDial(m1, 16, this, MID_CHANGED,
            LAYOUT_CENTER_Y|LAYOUT_TOP|FRAME_SUNKEN|FRAME_THICK);
        myXOff->setRange(-1000000, 1000000);
        myXOff->setNumberFormat(4);
        myXOff->setValue(xoff);
    }
    {
        new FXLabel(m1, "Y Offset:", 0, LAYOUT_CENTER_Y);
        myYOff = new FXRealSpinDial(m1, 16, this, MID_CHANGED,
            LAYOUT_CENTER_Y|LAYOUT_TOP|FRAME_SUNKEN|FRAME_THICK);
        myYOff->setRange(-1000000, 1000000);
        myYOff->setNumberFormat(4);
        myYOff->setValue(yoff);
    }
    // ok/cancel
    new FXHorizontalSeparator(f1,SEPARATOR_GROOVE|LAYOUT_FILL_X);
    FXHorizontalFrame *f6 =
        new FXHorizontalFrame(f1,
            LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_FILL_X|PACK_UNIFORM_WIDTH,
            0,0,0,0, 10,10,5,0);
    FXButton *initial =
        new FXButton(f6,"&OK",NULL, this, MID_OK,
        BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_CENTER_X,
        0,0,0,0,  4, 4, 3, 3);
    new FXButton(f6,"&Cancel",NULL,this,MID_CANCEL,
        FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_CENTER_X,
        0,0,0,0,  4, 4, 3, 3);
    initial->setFocus();
}


void
GUIDialog_EditViewport::create()
{
    FXDialogBox::create();
}


GUIDialog_EditViewport::~GUIDialog_EditViewport()
{
}


long
GUIDialog_EditViewport::onCmdOk(FXObject*,FXSelector,void*)
{
    myParent->setViewport((SUMOReal) myZoom->getValue(),
        (SUMOReal) myXOff->getValue(), (SUMOReal) myYOff->getValue());
    hide();
    return 1;
}


long
GUIDialog_EditViewport::onCmdCancel(FXObject*,FXSelector,void*)
{
    myParent->setViewport(myOldZoom, myOldXOff, myOldYOff);
    hide();
    return 1;
}


long
GUIDialog_EditViewport::onCmdChanged(FXObject*,FXSelector,void*)
{
    myParent->setViewport((SUMOReal) myZoom->getValue(),
        (SUMOReal) myXOff->getValue(), (SUMOReal) myYOff->getValue());
    return 1;
}


void
GUIDialog_EditViewport::setValues(SUMOReal zoom,
                                  SUMOReal xoff, SUMOReal yoff)
{
    myZoom->setValue(zoom);
    myXOff->setValue(xoff);
    myYOff->setValue(yoff);
}


void
GUIDialog_EditViewport::setOldValues(SUMOReal zoom, SUMOReal xoff, SUMOReal yoff)
{
    myZoom->setValue(zoom);
    myXOff->setValue(xoff);
    myYOff->setValue(yoff);
    myOldZoom = zoom;
    myOldXOff = xoff;
    myOldYOff = yoff;
}


bool
GUIDialog_EditViewport::haveGrabbed() const
{
    return myZoom->getDial().grabbed()||myXOff->getDial().grabbed()||myYOff->getDial().grabbed();
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
