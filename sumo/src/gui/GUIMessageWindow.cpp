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
// Revision 1.2  2003/12/04 13:23:57  dkrajzew
// made the output of warnings prettier and more visible
//
// Revision 1.1  2003/11/26 09:39:13  dkrajzew
// added a logging windows to the gui (the passing of more than a single lane
//  to come makes it necessary)
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <cassert>
#include "GUIMessageWindow.h"

#ifndef WIN32
#include "GUIMessageWindow.moc"
#endif


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUIMessageWindow::GUIMessageWindow(QWidget *parent)
    : QTextView(parent)
{
    setMinimumSize(50, 50);
}


GUIMessageWindow::~GUIMessageWindow()
{
}


void
GUIMessageWindow::appendText(GUIEvent eType, const std::string &msg)
{
    // set logger to visible
    if(!isVisible()) {
        show();
    }
    if(!isEnabled()) {
        setEnabled(true);
    }
    // build the styled message
    string mmsg = string("<nobr><font color=\"");
    switch(eType) {
    case EVENT_ERROR_OCCURED:
        // color: red
        mmsg += "#880000";
        break;
    case EVENT_WARNING_OCCURED:
        // color: yellow
        mmsg += "#e69800";
        break;
    case EVENT_MESSAGE_OCCURED:
        // color: green
        mmsg += "#008800";
        break;
    default:
        assert(false);
    }
    // continue message building
    mmsg += string("\">") + msg
        + string("</font></nobr>");
    // insert message to buffer
    append(mmsg.c_str());
    update();
}


void
GUIMessageWindow::addSeparator()
{
    string toAdd = string("<nobr><font color=\"#000088\">")
        + "----------------------------------------------------------------------------------------"
        + string("</font></nobr>");
    append(toAdd.c_str());
    update();
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
