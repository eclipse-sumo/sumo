/****************************************************************************
** QGUIToggleButton meta object code from reading C++ file 'QGUIToggleButton.h'
**
** Created: Wed Nov 26 09:33:59 2003
**      by: The Qt MOC ($Id$)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#define Q_MOC_QGUIToggleButton
#if !defined(Q_MOC_OUTPUT_REVISION)
#define Q_MOC_OUTPUT_REVISION 8
#elif Q_MOC_OUTPUT_REVISION != 8
#error "Moc format conflict - please regenerate all moc files"
#endif

#include "QGUIToggleButton.h"
#include <qmetaobject.h>
#include <qapplication.h>

#if defined(Q_SPARCWORKS_FUNCP_BUG)
#define Q_AMPERSAND
#else
#define Q_AMPERSAND &
#endif


const char *QGUIToggleButton::className() const
{
    return "QGUIToggleButton";
}

QMetaObject *QGUIToggleButton::metaObj = 0;

void QGUIToggleButton::initMetaObject()
{
    if ( metaObj )
	return;
    if ( strcmp(QToolButton::className(), "QToolButton") != 0 )
	badSuperclassWarning("QGUIToggleButton","QToolButton");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION
QString QGUIToggleButton::tr(const char* s)
{
    return ((QNonBaseApplication*)qApp)->translate("QGUIToggleButton",s);
}

#endif // QT_NO_TRANSLATION
QMetaObject* QGUIToggleButton::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    (void) QToolButton::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    QMetaData::Access *slot_tbl_access = 0;
    metaObj = QMetaObject::new_metaobject(
	"QGUIToggleButton", "QToolButton",
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
