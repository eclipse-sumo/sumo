//---------------------------------------------------------------------------//
//                        GUIChooser.cpp -
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.4  2003/05/20 09:23:54  dkrajzew
// some statistics added; some debugging done
//
// Revision 1.3  2003/03/12 16:55:16  dkrajzew
// centering of objects debugged
//
// Revision 1.2  2003/02/07 10:34:14  dkrajzew
// files updated
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
#include <qmainwindow.h>
#include <qlistbox.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qstring.h>
#include "GUISUMOViewParent.h"
#include "GUIChooser.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUIChooser::GUIChooser(GUISUMOViewParent *parent, GUIGlObjectType type,
                       std::vector<std::string> &names)
    : QMainWindow(0, (string("Chooser")).c_str(), WType_Modal),
    _type(type), _parent(parent)
{
    setFixedSize(300, 300);
    QHBoxLayout *hbox = new QHBoxLayout( this );
    hbox->setSpacing(4);
    // build the list
    buildList(hbox, names);
    // build the buttons
    buildButtons(hbox);
}


GUIChooser::~GUIChooser()
{
}


void
GUIChooser::buildList(QHBoxLayout *hbox, std::vector<std::string> &names)
{
    _list = new QListBox(this, "items", 0);
    hbox->addWidget(_list);
    for(std::vector<std::string>::iterator
        i=names.begin(); i!=names.end(); i++) {
        _list->insertItem(QString((*i).c_str()));
    }
}


void
GUIChooser::buildButtons(QHBoxLayout *hbox)
{
    // build the layout
    QGridLayout *layout = new QGridLayout( hbox, 3, 1, 4 );
//    hbox->addItem(layout);
    // build the "OK"-button
    QPushButton *button = new QPushButton(QString("OK"), this, "OK");
    connect( button, SIGNAL( clicked() ), this, SLOT( pressedOK() ) );
    layout->addWidget( button, 0, 0 );
    // build the "Cancel"-button
    button = new QPushButton(QString("Cancel"), this, "Cancel");
    connect( button, SIGNAL( clicked() ), this, SLOT( pressedCancel() ) );
    layout->addWidget( button, 1, 0 );
    // add the spacer
}


void
GUIChooser::pressedOK()
{
    if(_list->currentItem()>=0) {
        _id = string(_list->currentText().ascii());
    } else {
        _id = "";
    }
    close(TRUE);
}


void
GUIChooser::pressedCancel()
{
    _id = "";
    close(TRUE);
}


void
GUIChooser::closeEvent ( QCloseEvent *e )
{
    if(_id.length()!=0) {
        _parent->setView(_type, _id);
    }
    QMainWindow::closeEvent(e);
}


std::string
GUIChooser::getID() const
{
    return _id;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUIChooser.icc"
//#endif

// Local Variables:
// mode:C++
// End:


