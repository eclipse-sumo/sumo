/****************************************************************************
** QParamPopupMenu meta object code from reading C++ file 'QParamPopupMenu.h'
**
** Created: Tue Sep 16 15:42:50 2003
**      by: The Qt MOC ($Id$)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#define Q_MOC_QParamPopupMenu
#if !defined(Q_MOC_OUTPUT_REVISION)
#define Q_MOC_OUTPUT_REVISION 8
#elif Q_MOC_OUTPUT_REVISION != 8
#error "Moc format conflict - please regenerate all moc files"
#endif

#include "QParamPopupMenu.h"
#include <qmetaobject.h>
#include <qapplication.h>

#if defined(Q_SPARCWORKS_FUNCP_BUG)
#define Q_AMPERSAND
#else
#define Q_AMPERSAND &
#endif


const char *QParamPopupMenu::className() const
{
    return "QParamPopupMenu";
}

QMetaObject *QParamPopupMenu::metaObj = 0;

void QParamPopupMenu::initMetaObject()
{
    if ( metaObj )
	return;
    if ( strcmp(QPopupMenu::className(), "QPopupMenu") != 0 )
	badSuperclassWarning("QParamPopupMenu","QPopupMenu");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION
QString QParamPopupMenu::tr(const char* s)
{
    return ((QNonBaseApplication*)qApp)->translate("QParamPopupMenu",s);
}

#endif // QT_NO_TRANSLATION
QMetaObject* QParamPopupMenu::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    (void) QPopupMenu::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    typedef void(QParamPopupMenu::*m1_t0)();
    m1_t0 v1_0 = Q_AMPERSAND QParamPopupMenu::newTracker;
    QMetaData *slot_tbl = QMetaObject::new_metadata(1);
    QMetaData::Access *slot_tbl_access = QMetaObject::new_metaaccess(1);
    slot_tbl[0].name = "newTracker()";
    slot_tbl[0].ptr = *((QMember*)&v1_0);
    slot_tbl_access[0] = QMetaData::Public;
    metaObj = QMetaObject::new_metaobject(
	"QParamPopupMenu", "QPopupMenu",
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
