/****************************************************************************
** GUIViewTraffic meta object code from reading C++ file 'GUIViewTraffic.h'
**
** Created: Wed Nov 26 09:33:59 2003
**      by: The Qt MOC ($Id$)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#define Q_MOC_GUIViewTraffic
#if !defined(Q_MOC_OUTPUT_REVISION)
#define Q_MOC_OUTPUT_REVISION 8
#elif Q_MOC_OUTPUT_REVISION != 8
#error "Moc format conflict - please regenerate all moc files"
#endif

#include "GUIViewTraffic.h"
#include <qmetaobject.h>
#include <qapplication.h>

#if defined(Q_SPARCWORKS_FUNCP_BUG)
#define Q_AMPERSAND
#else
#define Q_AMPERSAND &
#endif


const char *GUIViewTraffic::className() const
{
    return "GUIViewTraffic";
}

QMetaObject *GUIViewTraffic::metaObj = 0;

void GUIViewTraffic::initMetaObject()
{
    if ( metaObj )
	return;
    if ( strcmp(GUISUMOAbstractView::className(), "GUISUMOAbstractView") != 0 )
	badSuperclassWarning("GUIViewTraffic","GUISUMOAbstractView");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION
QString GUIViewTraffic::tr(const char* s)
{
    return ((QNonBaseApplication*)qApp)->translate("GUIViewTraffic",s);
}

#endif // QT_NO_TRANSLATION
QMetaObject* GUIViewTraffic::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    (void) GUISUMOAbstractView::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    typedef void(GUIViewTraffic::*m1_t0)(int);
    typedef void(GUIViewTraffic::*m1_t1)(int);
    typedef void(GUIViewTraffic::*m1_t2)();
    m1_t0 v1_0 = Q_AMPERSAND GUIViewTraffic::changeVehicleColoringScheme;
    m1_t1 v1_1 = Q_AMPERSAND GUIViewTraffic::changeLaneColoringScheme;
    m1_t2 v1_2 = Q_AMPERSAND GUIViewTraffic::toggleFullGeometry;
    QMetaData *slot_tbl = QMetaObject::new_metadata(3);
    QMetaData::Access *slot_tbl_access = QMetaObject::new_metaaccess(3);
    slot_tbl[0].name = "changeVehicleColoringScheme(int)";
    slot_tbl[0].ptr = *((QMember*)&v1_0);
    slot_tbl_access[0] = QMetaData::Public;
    slot_tbl[1].name = "changeLaneColoringScheme(int)";
    slot_tbl[1].ptr = *((QMember*)&v1_1);
    slot_tbl_access[1] = QMetaData::Public;
    slot_tbl[2].name = "toggleFullGeometry()";
    slot_tbl[2].ptr = *((QMember*)&v1_2);
    slot_tbl_access[2] = QMetaData::Public;
    metaObj = QMetaObject::new_metaobject(
	"GUIViewTraffic", "GUISUMOAbstractView",
	slot_tbl, 3,
	0, 0,
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
