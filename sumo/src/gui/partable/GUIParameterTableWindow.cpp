//---------------------------------------------------------------------------//
//                        GUIParameterTableWindow.cpp -
//  The window that holds the table of an object's parameter
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
// Revision 1.3  2003/06/05 11:38:20  dkrajzew
// class templates applied; documentation added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <qdialog.h>
#include <qlistview.h>
#include <utils/convert/ToString.h>
#include <guisim/GUINet.h>
#include <gui/GUIApplicationWindow.h>
#include "GUIParameterTable.h"
#include "GUIParameterTableWindow.h"
#include <gui/GUIGlObject.h>

#ifndef WIN32
#include "GUIParameterTableWindow.moc"
#endif


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUIParameterTableWindow::GUIParameterTableWindow(GUIApplicationWindow *app,
                                       QWidget * parent,
                                       GUIGlObject *o )
	: QDialog( 0, 0 ), myObject(o),
    myApplication(app)
{
    setCaption(string(o->getFullName() + " Parameter").c_str() );
    myTable = new GUIParameterTable( app, o, this );
/*    connect(myTable, SIGNAL(selectionChanged() ),
	    this, SLOT( selectionChanged() ) );
    connect(myTable, SIGNAL(selectionChanged(QListViewItem*) ),
	    this, SLOT( selectionChanged(QListViewItem*) ) );
    connect(myTable, SIGNAL(clicked(QListViewItem*) ),
	    this, SLOT( clicked(QListViewItem*) ) );
    connect(myTable, SIGNAL(mySelectionChanged(QListViewItem*) ),
	    this, SLOT( mySelectionChanged(QListViewItem*) ) );*/
    myTable->addColumn( "Name" );
    myTable->addColumn( "Value" );
    myTable->setAllColumnsShowFocus( TRUE );
    myParameter = new double[o->getTableParameterNo()];
    myParameterBuffer = new double[o->getTableParameterNo()];
    myItems = new QListViewItem*[o->getTableParameterNo()];
    myTable->setBaseSize(200, 300);
    myTable->setMinimumSize(200, 300);
    setBaseSize(200, 300);
    setMinimumSize(200, 300);
    myTable->setSorting(-1);
    o->insertTableParameter(this, myTable, myParameter, myItems);
    show();
    app->addChild(this, true);
}


GUIParameterTableWindow::~GUIParameterTableWindow()
{
    delete[] myParameter;
    delete[] myParameterBuffer;
    delete[] myItems;
    myApplication->removeChild(this);
}


bool
GUIParameterTableWindow::event ( QEvent *e )
{
    if(e->type()!=QEvent::User) {
        return QDialog::event(e);
    }
//    GUINet::lockAlloc();
    myObject->fillTableParameter(myParameterBuffer);
    for(int i=0; i<myObject->getTableParameterNo(); i++) {
        // update only if changed
        if(myParameter[i]!=myParameterBuffer[i]) {
            QListViewItem *item = myItems[i];
            item->setText(1,
                toString<double>(myParameterBuffer[i]).c_str());
        }
    }
    repaint();
//    GUINet::unlockAlloc();
    return TRUE;
}


void
GUIParameterTableWindow::resizeEvent ( QResizeEvent *e )
{
//    GUINet::lockAlloc();
    myTable->resize(e->size());
//    GUINet::unlockAlloc();
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUIParameterTableWindow.icc"
//#endif

// Local Variables:
// mode:C++
// End:


