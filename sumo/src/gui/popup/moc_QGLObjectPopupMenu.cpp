/****************************************************************************
** QGLObjectPopupMenu meta object code from reading C++ file 'QGLObjectPopupMenu.h'
**
** Created: Tue Sep 16 15:42:50 2003
**      by: The Qt MOC ($Id$)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#define Q_MOC_QGLObjectPopupMenu
#if !defined(Q_MOC_OUTPUT_REVISION)
#define Q_MOC_OUTPUT_REVISION 8
#elif Q_MOC_OUTPUT_REVISION != 8
#error "Moc format conflict - please regenerate all moc files"
#endif

#include "QGLObjectPopupMenu.h"
#include <qmetaobject.h>
#include <qapplication.h>

#if defined(Q_SPARCWORKS_FUNCP_BUG)
#define Q_AMPERSAND
#else
#define Q_AMPERSAND &
#endif


const char *QGLObjectPopupMenu::className() const
{
    return "QGLObjectPopupMenu";
}

QMetaObject *QGLObjectPopupMenu::metaObj = 0;

void QGLObjectPopupMenu::initMetaObject()
{
    if ( metaObj )
	return;
    if ( strcmp(QPopupMenu::className(), "QPopupMenu") != 0 )
	badSuperclassWarning("QGLObjectPopupMenu","QPopupMenu");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION
QString QGLObjectPopupMenu::tr(const char* s)
{
    return ((QNonBaseApplication*)qApp)->translate("QGLObjectPopupMenu",s);
}

#endif // QT_NO_TRANSLATION
QMetaObject* QGLObjectPopupMenu::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    (void) QPopupMenu::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    typedef void(QGLObjectPopupMenu::*m1_t0)();
    typedef void(QGLObjectPopupMenu::*m1_t1)();
    m1_t0 v1_0 = Q_AMPERSAND QGLObjectPopupMenu::center;
    m1_t1 v1_1 = Q_AMPERSAND QGLObjectPopupMenu::showPars;
    QMetaData *slot_tbl = QMetaObject::new_metadata(2);
    QMetaData::Access *slot_tbl_access = QMetaObject::new_metaaccess(2);
    slot_tbl[0].name = "center()";
    slot_tbl[0].ptr = *((QMember*)&v1_0);
    slot_tbl_access[0] = QMetaData::Public;
    slot_tbl[1].name = "showPars()";
    slot_tbl[1].ptr = *((QMember*)&v1_1);
    slot_tbl_access[1] = QMetaData::Public;
    metaObj = QMetaObject::new_metaobject(
	"QGLObjectPopupMenu", "QPopupMenu",
	slot_tbl, 2,
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
