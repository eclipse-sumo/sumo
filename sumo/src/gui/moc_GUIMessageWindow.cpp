/****************************************************************************
** GUIMessageWindow meta object code from reading C++ file 'GUIMessageWindow.h'
**
** Created: Wed Nov 26 11:25:07 2003
**      by: The Qt MOC ($Id$)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#define Q_MOC_GUIMessageWindow
#if !defined(Q_MOC_OUTPUT_REVISION)
#define Q_MOC_OUTPUT_REVISION 8
#elif Q_MOC_OUTPUT_REVISION != 8
#error "Moc format conflict - please regenerate all moc files"
#endif

#include "GUIMessageWindow.h"
#include <qmetaobject.h>
#include <qapplication.h>

#if defined(Q_SPARCWORKS_FUNCP_BUG)
#define Q_AMPERSAND
#else
#define Q_AMPERSAND &
#endif


const char *GUIMessageWindow::className() const
{
    return "GUIMessageWindow";
}

QMetaObject *GUIMessageWindow::metaObj = 0;

void GUIMessageWindow::initMetaObject()
{
    if ( metaObj )
	return;
    if ( strcmp(QTextView::className(), "QTextView") != 0 )
	badSuperclassWarning("GUIMessageWindow","QTextView");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION
QString GUIMessageWindow::tr(const char* s)
{
    return ((QNonBaseApplication*)qApp)->translate("GUIMessageWindow",s);
}

#endif // QT_NO_TRANSLATION
QMetaObject* GUIMessageWindow::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    (void) QTextView::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    QMetaData::Access *slot_tbl_access = 0;
    metaObj = QMetaObject::new_metaobject(
	"GUIMessageWindow", "QTextView",
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
