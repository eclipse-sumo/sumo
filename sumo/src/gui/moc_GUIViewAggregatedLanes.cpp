/****************************************************************************
** GUIViewAggregatedLanes meta object code from reading C++ file 'GUIViewAggregatedLanes.h'
**
** Created: Wed Nov 26 09:33:59 2003
**      by: The Qt MOC ($Id$)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#define Q_MOC_GUIViewAggregatedLanes
#if !defined(Q_MOC_OUTPUT_REVISION)
#define Q_MOC_OUTPUT_REVISION 8
#elif Q_MOC_OUTPUT_REVISION != 8
#error "Moc format conflict - please regenerate all moc files"
#endif

#include "GUIViewAggregatedLanes.h"
#include <qmetaobject.h>
#include <qapplication.h>

#if defined(Q_SPARCWORKS_FUNCP_BUG)
#define Q_AMPERSAND
#else
#define Q_AMPERSAND &
#endif


const char *GUIViewAggregatedLanes::className() const
{
    return "GUIViewAggregatedLanes";
}

QMetaObject *GUIViewAggregatedLanes::metaObj = 0;

void GUIViewAggregatedLanes::initMetaObject()
{
    if ( metaObj )
	return;
    if ( strcmp(GUISUMOAbstractView::className(), "GUISUMOAbstractView") != 0 )
	badSuperclassWarning("GUIViewAggregatedLanes","GUISUMOAbstractView");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION
QString GUIViewAggregatedLanes::tr(const char* s)
{
    return ((QNonBaseApplication*)qApp)->translate("GUIViewAggregatedLanes",s);
}

#endif // QT_NO_TRANSLATION
QMetaObject* GUIViewAggregatedLanes::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    (void) GUISUMOAbstractView::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    typedef void(GUIViewAggregatedLanes::*m1_t0)(int);
    m1_t0 v1_0 = Q_AMPERSAND GUIViewAggregatedLanes::changeLaneColoringScheme;
    QMetaData *slot_tbl = QMetaObject::new_metadata(1);
    QMetaData::Access *slot_tbl_access = QMetaObject::new_metaaccess(1);
    slot_tbl[0].name = "changeLaneColoringScheme(int)";
    slot_tbl[0].ptr = *((QMember*)&v1_0);
    slot_tbl_access[0] = QMetaData::Public;
    metaObj = QMetaObject::new_metaobject(
	"GUIViewAggregatedLanes", "GUISUMOAbstractView",
	slot_tbl, 1,
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
