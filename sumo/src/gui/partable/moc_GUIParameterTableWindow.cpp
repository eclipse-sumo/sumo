/****************************************************************************
** GUIParameterTableWindow meta object code from reading C++ file 'GUIParameterTableWindow.h'
**
** Created: Tue Sep 16 15:42:51 2003
**      by: The Qt MOC ($Id$)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#define Q_MOC_GUIParameterTableWindow
#if !defined(Q_MOC_OUTPUT_REVISION)
#define Q_MOC_OUTPUT_REVISION 8
#elif Q_MOC_OUTPUT_REVISION != 8
#error "Moc format conflict - please regenerate all moc files"
#endif

#include "GUIParameterTableWindow.h"
#include <qmetaobject.h>
#include <qapplication.h>

#if defined(Q_SPARCWORKS_FUNCP_BUG)
#define Q_AMPERSAND
#else
#define Q_AMPERSAND &
#endif


const char *GUIParameterTableWindow::className() const
{
    return "GUIParameterTableWindow";
}

QMetaObject *GUIParameterTableWindow::metaObj = 0;

void GUIParameterTableWindow::initMetaObject()
{
    if ( metaObj )
	return;
    if ( strcmp(QDialog::className(), "QDialog") != 0 )
	badSuperclassWarning("GUIParameterTableWindow","QDialog");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION
QString GUIParameterTableWindow::tr(const char* s)
{
    return ((QNonBaseApplication*)qApp)->translate("GUIParameterTableWindow",s);
}

#endif // QT_NO_TRANSLATION
QMetaObject* GUIParameterTableWindow::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    (void) QDialog::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    QMetaData::Access *slot_tbl_access = 0;
    metaObj = QMetaObject::new_metaobject(
	"GUIParameterTableWindow", "QDialog",
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
