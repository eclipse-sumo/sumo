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
// Revision 1.7  2003/11/12 14:09:13  dkrajzew
// clean up after recent changes; comments added
//
// Revision 1.6  2003/08/14 13:38:25  dkrajzew
// applied other window controls
//
// Revision 1.5  2003/07/30 08:48:28  dkrajzew
// new parameter table usage paradigm; undocummented yet
//
// Revision 1.4  2003/07/18 12:30:14  dkrajzew
// removed some warnings
//
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
#include <guisim/GUINet.h>
#include <gui/GUIApplicationWindow.h>
#include "GUIParameterTable.h"
#include "GUIParameterTableWindow.h"
#include <gui/GUIGlObject.h>
#include <utils/convert/ToString.h>
#include <gui/partable/QParamPopupMenu.h>

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
GUIParameterTableWindow::GUIParameterTableWindow(GUIApplicationWindow &app,
                                                 GUIGlObject &o )
	: QDialog( 0, 0, FALSE,
        WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu ),
    myObject(o),
    myApplication(app)
{
    setCaption(string(o.getFullName() + " Parameter").c_str() );
    myTable = new GUIParameterTable( app, *this, o, 0 );
    myTable->addColumn( "Name" );
    myTable->addColumn( "Value" );
    myTable->addColumn( "Dynamic" );
    myTable->setAllColumnsShowFocus( TRUE );
    myTable->setBaseSize(200, 300);
    myTable->setMinimumSize(200, 300);
    setBaseSize(200, 300);
    setMinimumSize(200, 300);
    myTable->setSorting(-1);
//    myParameter = new double[paramNo ];
//    myParameterBuffer = new double[paramNo ];
//    myItems = new GUIParameterTableItem*[paramNo ];
}


GUIParameterTableWindow::~GUIParameterTableWindow()
{
//    delete[] myParameter;
//    delete[] myParameterBuffer;
//    delete[] myItems;
    myApplication.removeChild(this);
}


bool
GUIParameterTableWindow::event ( QEvent *e )
{
    if(e->type()!=QEvent::User) {
        return QDialog::event(e);
    }
    updateTable();
    /*
    for(size_t i=0; i<getTableParameterNo(); i++) {
        // update only if changed
        if(myParameter[i]!=myParameterBuffer[i]) {
            QListViewItem *item = myItems[i];
            item->setText(1,
                toString<double>(myParameterBuffer[i]).c_str());
            myParameter[i] = myParameterBuffer[i];
        }
    }*/
    repaint();
    return TRUE;
}


void
GUIParameterTableWindow::resizeEvent ( QResizeEvent *e )
{
    myTable->resize(e->size());
}


void
GUIParameterTableWindow::mkItem(const char *name, bool dynamic,
        ValueSource<double> *src)
{
    GUIParameterTableItem *i = new GUIParameterTableItem(
        myTable, name, dynamic, src);
    myItems.push_back(i);
}


void
GUIParameterTableWindow::mkItem(const char *name, bool dynamic,
                                std::string value)
{
    GUIParameterTableItem *i = new GUIParameterTableItem(
        myTable, name, dynamic, value);
    myItems.push_back(i);
}


void
GUIParameterTableWindow::mkItem(const char *name, bool dynamic,
        double value)
{
    GUIParameterTableItem *i = new GUIParameterTableItem(
        myTable, name, dynamic, value);
    myItems.push_back(i);
}


void
GUIParameterTableWindow::updateTable()
{
    for(std::vector<GUIParameterTableItem*>::iterator i=myItems.begin(); i!=myItems.end(); i++) {
        (*i)->update();
    }
}


void
GUIParameterTableWindow::closeBuilding()
{
    show();
    myApplication.addChild(this, true);
}


void
GUIParameterTableWindow::buildParameterPopUp(QMouseEvent * e, GUIParameterTableItem *i)
{
    QParamPopupMenu *p =
		new QParamPopupMenu(myApplication, *(this->myTable), *this,
            myObject, i->getName(), i->getSourceCopy());
    int id;
    if(i->dynamic()) {
        id = p->insertItem("Open in new Tracker", p, SLOT(newTracker()));
        p->setItemEnabled(id, TRUE);
        id = p->insertItem("Open in Tracker...");
        p->setItemEnabled(id, FALSE);
        id = p->insertItem("Begin logging...");
        p->setItemEnabled(id, FALSE);
    }
    p->insertSeparator();
    id = p->insertItem("Show in Distribution over same");
    p->setItemEnabled(id, FALSE);
    id = p->insertItem("Set as coloring scheme ...");
    p->setItemEnabled(id, FALSE);
    // set geometry
    p->setGeometry(e->globalX(), e->globalY(),
        p->width()+e->globalX(), p->height()+e->globalY());
    // show
    p->show();
}




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUIParameterTableWindow.icc"
//#endif

// Local Variables:
// mode:C++
// End:


