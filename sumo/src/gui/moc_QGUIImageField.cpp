/****************************************************************************
** QGUIImageField meta object code from reading C++ file 'QGUIImageField.h'
**
** Created: Tue Sep 16 15:42:50 2003
**      by: The Qt MOC ($Id$)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#define Q_MOC_QGUIImageField
#if !defined(Q_MOC_OUTPUT_REVISION)
#define Q_MOC_OUTPUT_REVISION 8
#elif Q_MOC_OUTPUT_REVISION != 8
#error "Moc format conflict - please regenerate all moc files"
#endif

#include "QGUIImageField.h"
#include <qmetaobject.h>
#include <qapplication.h>

#if defined(Q_SPARCWORKS_FUNCP_BUG)
#define Q_AMPERSAND
#else
#define Q_AMPERSAND &
#endif


const char *QGUIImageField::className() const
{
    return "QGUIImageField";
}

QMetaObject *QGUIImageField::metaObj = 0;

void QGUIImageField::initMetaObject()
{
    if ( metaObj )
	return;
    if ( strcmp(QToolButton::className(), "QToolButton") != 0 )
	badSuperclassWarning("QGUIImageField","QToolButton");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION
QString QGUIImageField::tr(const char* s)
{
    return ((QNonBaseApplication*)qApp)->translate("QGUIImageField",s);
}

#endif // QT_NO_TRANSLATION
QMetaObject* QGUIImageField::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    (void) QToolButton::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    QMetaData::Access *slot_tbl_access = 0;
    metaObj = QMetaObject::new_metaobject(
	"QGUIImageField", "QToolButton",
	0, 0,
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
