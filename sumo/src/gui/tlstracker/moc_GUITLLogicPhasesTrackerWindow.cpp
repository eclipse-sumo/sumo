/****************************************************************************
** GUITLLogicPhasesTrackerWindow meta object code from reading C++ file 'GUITLLogicPhasesTrackerWindow.h'
**
** Created: Wed Nov 12 14:25:21 2003
**      by: The Qt MOC ($Id$)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#define Q_MOC_GUITLLogicPhasesTrackerWindow
#if !defined(Q_MOC_OUTPUT_REVISION)
#define Q_MOC_OUTPUT_REVISION 8
#elif Q_MOC_OUTPUT_REVISION != 8
#error "Moc format conflict - please regenerate all moc files"
#endif

#include "GUITLLogicPhasesTrackerWindow.h"
#include <qmetaobject.h>
#include <qapplication.h>

#if defined(Q_SPARCWORKS_FUNCP_BUG)
#define Q_AMPERSAND
#else
#define Q_AMPERSAND &
#endif


const char *GUITLLogicPhasesTrackerWindow::className() const
{
    return "GUITLLogicPhasesTrackerWindow";
}

QMetaObject *GUITLLogicPhasesTrackerWindow::metaObj = 0;

void GUITLLogicPhasesTrackerWindow::initMetaObject()
{
    if ( metaObj )
	return;
    if ( strcmp(QMainWindow::className(), "QMainWindow") != 0 )
	badSuperclassWarning("GUITLLogicPhasesTrackerWindow","QMainWindow");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION
QString GUITLLogicPhasesTrackerWindow::tr(const char* s)
{
    return ((QNonBaseApplication*)qApp)->translate("GUITLLogicPhasesTrackerWindow",s);
}

#endif // QT_NO_TRANSLATION
QMetaObject* GUITLLogicPhasesTrackerWindow::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    (void) QMainWindow::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    QMetaData::Access *slot_tbl_access = 0;
    metaObj = QMetaObject::new_metaobject(
	"GUITLLogicPhasesTrackerWindow", "QMainWindow",
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
