//---------------------------------------------------------------------------//
//                        GUIParameterTable.cpp -
//  A qt-table holding the parameters of an gl-object
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
// Revision 1.3  2003/06/05 11:38:19  dkrajzew
// class templates applied; documentation added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <qnamespace.h>
#include <qlistview.h>
#include "GUIParameterTable.h"
#include "QParamPopupMenu.h"

#ifndef WIN32
#include "GUIParameterTable.moc"
#endif


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUIParameterTable::GUIParameterTable(GUIApplicationWindow *app,
									 GUIGlObject *o,
                                     QWidget * parent,
                                     const char *name)
	: QListView( parent, name ), myObject(o), selected(0), myApplication(app)
{
}


GUIParameterTable::~GUIParameterTable()
{
}



void
GUIParameterTable::contentsMousePressEvent( QMouseEvent * e )
{
    selected = selectedItem();
    QListView::contentsMousePressEvent( e );
    Qt::ButtonState button = e->button();
    if(button!=Qt::RightButton) {
        QListView::mousePressEvent(e);
        return;
    }
    // get the item
    QListViewItem *i = itemAt(QPoint(e->x(), e->y()));
    // build the popup
    QParamPopupMenu *p =
		new QParamPopupMenu(myApplication, this, myObject, 0);
    int id = p->insertItem("Open in new Tracker", p, SLOT(newTracker()));
    p->setItemEnabled(id, TRUE);
    id = p->insertItem("Open in Tracker...");
    p->setItemEnabled(id, FALSE);
    id = p->insertItem("Show in Distribution over same");
    p->setItemEnabled(id, FALSE);
    id = p->insertItem("Begin logging...");
    p->setItemEnabled(id, FALSE);
    p->insertSeparator();
    id = p->insertItem("Set as coloring scheme ...");
    p->setItemEnabled(id, FALSE);
    // set geometry
    p->setGeometry(e->globalX(), e->globalY(),
        p->width()+e->globalX(), p->height()+e->globalY());
    // show
    p->show();
}

/*
void
GUIParameterTable::contentsMouseReleaseEvent( QMouseEvent * e )
{
    QListView::contentsMouseReleaseEvent( e );
    if ( selectedItem() != selected ) {
        emit mySelectionChanged( selectedItem() );
        emit mySelectionChanged();
    }
}
*/


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUIParameterTable.icc"
//#endif

// Local Variables:
// mode:C++
// End:


