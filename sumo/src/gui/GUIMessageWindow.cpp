//---------------------------------------------------------------------------//
//                        GUIMessageWindow.cpp -
//  A logging window for the gui
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 25 Nov 2003
//  copyright            : (C) 2003 by Daniel Krajzewicz
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
// Revision 1.3  2004/03/19 12:54:08  dkrajzew
// porting to FOX
//
// Revision 1.2  2003/12/04 13:23:57  dkrajzew
// made the output of warnings prettier and more visible
//
// Revision 1.1  2003/11/26 09:39:13  dkrajzew
// added a logging windows to the gui (the passing of more than a single lane
//  to come makes it necessary)
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <cassert>
#include "GUIMessageWindow.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUIMessageWindow::GUIMessageWindow(FXComposite *parent)
    : FXText(parent, 0, 0, 0, 0, 0, 0, 50)
{
    setStyled(true);
    setEditable(false);
    myStyles = new FXHiliteStyle[4];
    // set separator style
    myStyles[0].normalForeColor = FXRGB(0x00, 0x00, 0x88); //
    myStyles[0].normalBackColor = FXRGB(0xff, 0xff, 0xff);
    myStyles[0].selectForeColor = FXRGB(0xff, 0xff, 0xff);
    myStyles[0].selectBackColor = FXRGB(0x00, 0x00, 0x88); //
    myStyles[0].hiliteForeColor = FXRGB(0x00, 0x00, 0x88); //
    myStyles[0].hiliteBackColor = FXRGB(0xff, 0xff, 0xff);
    myStyles[0].activeBackColor = FXRGB(0xff, 0xff, 0xff);
    myStyles[0].style = 0;
    // set message text style
    myStyles[1].normalForeColor = FXRGB(0x00, 0x88, 0x00); //
    myStyles[1].normalBackColor = FXRGB(0xff, 0xff, 0xff);
    myStyles[1].selectForeColor = FXRGB(0xff, 0xff, 0xff);
    myStyles[1].selectBackColor = FXRGB(0x00, 0x88, 0x00); //
    myStyles[1].hiliteForeColor = FXRGB(0x00, 0x88, 0x00); //
    myStyles[1].hiliteBackColor = FXRGB(0xff, 0xff, 0xff);
    myStyles[1].activeBackColor = FXRGB(0xff, 0xff, 0xff);
    myStyles[1].style = 0;
    // set error text style
    myStyles[2].normalForeColor = FXRGB(0x88, 0x00, 0x00); //
    myStyles[2].normalBackColor = FXRGB(0xff, 0xff, 0xff);
    myStyles[2].selectForeColor = FXRGB(0xff, 0xff, 0xff);
    myStyles[2].selectBackColor = FXRGB(0x88, 0x00, 0x00); //
    myStyles[2].hiliteForeColor = FXRGB(0x88, 0x00, 0x00); //
    myStyles[2].hiliteBackColor = FXRGB(0xff, 0xff, 0xff);
    myStyles[2].activeBackColor = FXRGB(0xff, 0xff, 0xff);
    myStyles[2].style = 0;
    // set warning text style
    myStyles[3].normalForeColor = FXRGB(0xe6, 0x98, 0x00); //
    myStyles[3].normalBackColor = FXRGB(0xff, 0xff, 0xff);
    myStyles[3].selectForeColor = FXRGB(0xff, 0xff, 0xff);
    myStyles[3].selectBackColor = FXRGB(0xe6, 0x98, 0x00); //
    myStyles[3].hiliteForeColor = FXRGB(0xe6, 0x98, 0x00); //
    myStyles[3].hiliteBackColor = FXRGB(0xff, 0xff, 0xff);
    myStyles[3].activeBackColor = FXRGB(0xff, 0xff, 0xff);
    myStyles[3].style = 0;
    //
    setHiliteStyles(myStyles);
}


GUIMessageWindow::~GUIMessageWindow()
{
}


void
GUIMessageWindow::appendText(GUIEventType eType, const std::string &msg)
{
    if(!isEnabled()) {
        show();
    }
    // build the styled message
    int style = 1;
    switch(eType) {
    case EVENT_ERROR_OCCURED:
        // color: red
        style = 2;
        break;
    case EVENT_WARNING_OCCURED:
        // color: yellow
        style = 3;
        break;
    case EVENT_MESSAGE_OCCURED:
        // color: green
        style = 1;
        break;
    default:
        assert(false);
    }
    // continue message building
    // insert message to buffer
    std::string mmsg = msg + "\n";
    FXText::appendStyledText(mmsg.c_str(), mmsg.length(), style+1, true);
    update();
}


void
GUIMessageWindow::addSeparator()
{
    std::string msg = "----------------------------------------------------------------------------------------\n";
    FXText::appendStyledText(msg.c_str(), msg.length(), 1, true);
    update();
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
