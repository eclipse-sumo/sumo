/****************************************************************************
** GUIParameterTable meta object code from reading C++ file 'GUIParameterTable.h'
**
** Created: Tue Sep 16 15:42:51 2003
**      by: The Qt MOC ($Id$)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#define Q_MOC_GUIParameterTable
#if !defined(Q_MOC_OUTPUT_REVISION)
#define Q_MOC_OUTPUT_REVISION 8
#elif Q_MOC_OUTPUT_REVISION != 8
#error "Moc format conflict - please regenerate all moc files"
#endif

#include "GUIParameterTable.h"
#include <qmetaobject.h>
#include <qapplication.h>

#if defined(Q_SPARCWORKS_FUNCP_BUG)
#define Q_AMPERSAND
#else
#define Q_AMPERSAND &
#endif


const char *GUIParameterTable::className() const
{
    return "GUIParameterTable";
}

QMetaObject *GUIParameterTable::metaObj = 0;

void GUIParameterTable::initMetaObject()
{
    if ( metaObj )
	return;
    if ( strcmp(QListView::className(), "QListView") != 0 )
	badSuperclassWarning("GUIParameterTable","QListView");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION
QString GUIParameterTable::tr(const char* s)
{
    return ((QNonBaseApplication*)qApp)->translate("GUIParameterTable",s);
}

#endif // QT_NO_TRANSLATION
QMetaObject* GUIParameterTable::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    (void) QListView::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    QMetaData::Access *slot_tbl_access = 0;
    typedef void(GUIParameterTable::*m2_t0)();
    typedef void(GUIParameterTable::*m2_t1)(QListViewItem*);
    m2_t0 v2_0 = Q_AMPERSAND GUIParameterTable::mySelectionChanged;
    m2_t1 v2_1 = Q_AMPERSAND GUIParameterTable::mySelectionChanged;
    QMetaData *signal_tbl = QMetaObject::new_metadata(2);
    signal_tbl[0].name = "mySelectionChanged()";
    signal_tbl[0].ptr = *((QMember*)&v2_0);
    signal_tbl[1].name = "mySelectionChanged(QListViewItem*)";
    signal_tbl[1].ptr = *((QMember*)&v2_1);
    metaObj = QMetaObject::new_metaobject(
	"GUIParameterTable", "QListView",
	0, 0,
	signal_tbl, 2,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    metaObj->set_slot_access( slot_tbl_access );
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    return metaObj;
}

// SIGNAL mySelectionChanged
void GUIParameterTable::mySelectionChanged()
{
    activate_signal( "mySelectionChanged()" );
}

#include <qobjectdefs.h>
#include <qsignalslotimp.h>

// SIGNAL mySelectionChanged
void GUIParameterTable::mySelectionChanged( QListViewItem* t0 )
{
    // No builtin function for signal parameter type QListViewItem*
    QConnectionList *clist = receivers("mySelectionChanged(QListViewItem*)");
    if ( !clist || signalsBlocked() )
	return;
    typedef void (QObject::*RT0)();
    typedef RT0 *PRT0;
    typedef void (QObject::*RT1)(QListViewItem*);
    typedef RT1 *PRT1;
    RT0 r0;
    RT1 r1;
    QConnectionListIt it(*clist);
    QConnection   *c;
    QSenderObject *object;
    while ( (c=it.current()) ) {
	++it;
	object = (QSenderObject*)c->object();
	object->setSender( this );
	switch ( c->numArgs() ) {
	    case 0:
		r0 = *((PRT0)(c->member()));
		(object->*r0)();
		break;
	    case 1:
		r1 = *((PRT1)(c->member()));
		(object->*r1)(t0);
		break;
	}
    }
}
