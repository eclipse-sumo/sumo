/****************************************************************************
** GUISUMOViewParent meta object code from reading C++ file 'GUISUMOViewParent.h'
**
** Created: Wed Nov 26 09:33:59 2003
**      by: The Qt MOC ($Id$)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#define Q_MOC_GUISUMOViewParent
#if !defined(Q_MOC_OUTPUT_REVISION)
#define Q_MOC_OUTPUT_REVISION 8
#elif Q_MOC_OUTPUT_REVISION != 8
#error "Moc format conflict - please regenerate all moc files"
#endif

#include "GUISUMOViewParent.h"
#include <qmetaobject.h>
#include <qapplication.h>

#if defined(Q_SPARCWORKS_FUNCP_BUG)
#define Q_AMPERSAND
#else
#define Q_AMPERSAND &
#endif


const char *GUISUMOViewParent::className() const
{
    return "GUISUMOViewParent";
}

QMetaObject *GUISUMOViewParent::metaObj = 0;

void GUISUMOViewParent::initMetaObject()
{
    if ( metaObj )
	return;
    if ( strcmp(QMainWindow::className(), "QMainWindow") != 0 )
	badSuperclassWarning("GUISUMOViewParent","QMainWindow");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION
QString GUISUMOViewParent::tr(const char* s)
{
    return ((QNonBaseApplication*)qApp)->translate("GUISUMOViewParent",s);
}

#endif // QT_NO_TRANSLATION
QMetaObject* GUISUMOViewParent::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    (void) QMainWindow::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    typedef void(GUISUMOViewParent::*m1_t0)();
    typedef void(GUISUMOViewParent::*m1_t1)();
    typedef void(GUISUMOViewParent::*m1_t2)();
    typedef void(GUISUMOViewParent::*m1_t3)();
    typedef void(GUISUMOViewParent::*m1_t4)();
    typedef void(GUISUMOViewParent::*m1_t5)();
    typedef void(GUISUMOViewParent::*m1_t6)();
    typedef void(GUISUMOViewParent::*m1_t7)();
    typedef void(GUISUMOViewParent::*m1_t8)();
    m1_t0 v1_0 = Q_AMPERSAND GUISUMOViewParent::chooseJunction;
    m1_t1 v1_1 = Q_AMPERSAND GUISUMOViewParent::chooseEdge;
    m1_t2 v1_2 = Q_AMPERSAND GUISUMOViewParent::chooseVehicle;
    m1_t3 v1_3 = Q_AMPERSAND GUISUMOViewParent::recenterView;
    m1_t4 v1_4 = Q_AMPERSAND GUISUMOViewParent::toggleShowLegend;
    m1_t5 v1_5 = Q_AMPERSAND GUISUMOViewParent::toggleAllowRotation;
    m1_t6 v1_6 = Q_AMPERSAND GUISUMOViewParent::toggleBehaviour1;
    m1_t7 v1_7 = Q_AMPERSAND GUISUMOViewParent::toggleBehaviour2;
    m1_t8 v1_8 = Q_AMPERSAND GUISUMOViewParent::toggleBehaviour3;
    QMetaData *slot_tbl = QMetaObject::new_metadata(9);
    QMetaData::Access *slot_tbl_access = QMetaObject::new_metaaccess(9);
    slot_tbl[0].name = "chooseJunction()";
    slot_tbl[0].ptr = *((QMember*)&v1_0);
    slot_tbl_access[0] = QMetaData::Public;
    slot_tbl[1].name = "chooseEdge()";
    slot_tbl[1].ptr = *((QMember*)&v1_1);
    slot_tbl_access[1] = QMetaData::Public;
    slot_tbl[2].name = "chooseVehicle()";
    slot_tbl[2].ptr = *((QMember*)&v1_2);
    slot_tbl_access[2] = QMetaData::Public;
    slot_tbl[3].name = "recenterView()";
    slot_tbl[3].ptr = *((QMember*)&v1_3);
    slot_tbl_access[3] = QMetaData::Public;
    slot_tbl[4].name = "toggleShowLegend()";
    slot_tbl[4].ptr = *((QMember*)&v1_4);
    slot_tbl_access[4] = QMetaData::Public;
    slot_tbl[5].name = "toggleAllowRotation()";
    slot_tbl[5].ptr = *((QMember*)&v1_5);
    slot_tbl_access[5] = QMetaData::Public;
    slot_tbl[6].name = "toggleBehaviour1()";
    slot_tbl[6].ptr = *((QMember*)&v1_6);
    slot_tbl_access[6] = QMetaData::Public;
    slot_tbl[7].name = "toggleBehaviour2()";
    slot_tbl[7].ptr = *((QMember*)&v1_7);
    slot_tbl_access[7] = QMetaData::Public;
    slot_tbl[8].name = "toggleBehaviour3()";
    slot_tbl[8].ptr = *((QMember*)&v1_8);
    slot_tbl_access[8] = QMetaData::Public;
    typedef void(GUISUMOViewParent::*m2_t0)(const QString&,int);
    m2_t0 v2_0 = Q_AMPERSAND GUISUMOViewParent::message;
    QMetaData *signal_tbl = QMetaObject::new_metadata(1);
    signal_tbl[0].name = "message(const QString&,int)";
    signal_tbl[0].ptr = *((QMember*)&v2_0);
    metaObj = QMetaObject::new_metaobject(
	"GUISUMOViewParent", "QMainWindow",
	slot_tbl, 9,
	signal_tbl, 1,
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

#include <qobjectdefs.h>
#include <qsignalslotimp.h>

// SIGNAL message
void GUISUMOViewParent::message( const QString& t0, int t1 )
{
    // No builtin function for signal parameter type const QString&,int
    QConnectionList *clist = receivers("message(const QString&,int)");
    if ( !clist || signalsBlocked() )
	return;
    typedef void (QObject::*RT0)();
    typedef RT0 *PRT0;
    typedef void (QObject::*RT1)(const QString&);
    typedef RT1 *PRT1;
    typedef void (QObject::*RT2)(const QString&,int);
    typedef RT2 *PRT2;
    RT0 r0;
    RT1 r1;
    RT2 r2;
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
	    case 2:
		r2 = *((PRT2)(c->member()));
		(object->*r2)(t0, t1);
		break;
	}
    }
}
