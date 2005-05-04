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
// Revision 1.1  2005/05/04 09:23:41  dkrajzew
// entries for viewport definition added; popups now popup faster
//
// Revision 1.1  2005/04/26 07:35:59  dksumo
// SUMOTime inserted; level3 warnings patched; port to fox1.4
//
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#include "GUIDialog_EditViewport.h"
#include <utils/gui/windows/GUIAppGlobals.h>
#include "GUISUMOAbstractView.h"
#include <utils/foxtools/FXRealSpinDial.h>
#include <utils/gui/windows/GUIAppEnum.h>



FXDEFMAP(GUIDialog_EditViewport) GUIDialog_EditViewportMap[]={
    FXMAPFUNC(SEL_COMMAND, MID_OK,            GUIDialog_EditViewport::onCmdOk),
    FXMAPFUNC(SEL_COMMAND, MID_CANCEL,        GUIDialog_EditViewport::onCmdCancel),
};


// Object implementation
FXIMPLEMENT(GUIDialog_EditViewport,FXDialogBox,GUIDialog_EditViewportMap, ARRAYNUMBER(GUIDialog_EditViewportMap))

/* =========================================================================
 * method definitions
 * ======================================================================= */
GUIDialog_EditViewport::GUIDialog_EditViewport(GUISUMOAbstractView* parent,
                                               const char* name,
                                               double zoom,
                                               double xoff, double yoff,
                                               int x, int y)
    : FXDialogBox( parent, name, DECOR_CLOSE|DECOR_TITLE, x, y, 0, 0),
    myParent(parent), myOldZoom(zoom), myOldXOff(xoff), myOldYOff(yoff)
{
    FXVerticalFrame *f1 =
        new FXVerticalFrame(this,
            LAYOUT_TOP|FRAME_NONE|LAYOUT_FILL_X,
            0,0,0,0, 0,0,1,1);
    //
    {
        FXHorizontalFrame *f2 =
            new FXHorizontalFrame(f1,
                LAYOUT_TOP|FRAME_NONE|LAYOUT_FILL_X,
                0,0,0,0, 0,0,1,1);
        new FXLabel(f2, "Zoom:");
        myZoom = new FXRealSpinDial(f2, 16, 0, 0,
            LAYOUT_TOP|FRAME_SUNKEN|FRAME_THICK);
        myZoom->setRange(0.0001, 100000);
        myZoom->setNumberFormat(4);
        myZoom->setValue(zoom);
    }
    {
        FXHorizontalFrame *f2 =
            new FXHorizontalFrame(f1,
                LAYOUT_TOP|FRAME_NONE|LAYOUT_FILL_X,
                0,0,0,0, 0,0,1,1);
        new FXLabel(f2, "X Offset:");
        myXOff = new FXRealSpinDial(f2, 16, 0, 0,
            LAYOUT_TOP|FRAME_SUNKEN|FRAME_THICK);
        myXOff->setRange(-1000000, 1000000);
        myXOff->setNumberFormat(4);
        myXOff->setValue(xoff);
    }
    {
        FXHorizontalFrame *f2 =
            new FXHorizontalFrame(f1,
                LAYOUT_TOP|FRAME_NONE|LAYOUT_FILL_X,
                0,0,0,0, 0,0,1,1);
        new FXLabel(f2, "Y Offset:");
        myYOff = new FXRealSpinDial(f2, 16, 0, 0,
            LAYOUT_TOP|FRAME_SUNKEN|FRAME_THICK);
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
    myParent->setViewport(myZoom->getValue(),
        myXOff->getValue(), myYOff->getValue());
    hide();
    return 1;
}


long
GUIDialog_EditViewport::onCmdCancel(FXObject*,FXSelector,void*)
{
    hide();
    return 1;
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
