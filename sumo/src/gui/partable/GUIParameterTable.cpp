#include <qnamespace.h>
#include <qlistview.h>
#include "GUIParameterTable.h"
#include "QParamPopupMenu.h"

#ifndef WIN32
#include "GUIParameterTable.moc"
#endif

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
