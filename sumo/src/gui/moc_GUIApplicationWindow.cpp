/****************************************************************************
** GUIApplicationWindow meta object code from reading C++ file 'GUIApplicationWindow.h'
**
** Created: Wed Nov 26 11:25:07 2003
**      by: The Qt MOC ($Id$)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#define Q_MOC_GUIApplicationWindow
#if !defined(Q_MOC_OUTPUT_REVISION)
#define Q_MOC_OUTPUT_REVISION 8
#elif Q_MOC_OUTPUT_REVISION != 8
#error "Moc format conflict - please regenerate all moc files"
#endif

#include "GUIApplicationWindow.h"
#include <qmetaobject.h>
#include <qapplication.h>

#if defined(Q_SPARCWORKS_FUNCP_BUG)
#define Q_AMPERSAND
#else
#define Q_AMPERSAND &
#endif


const char *GUIApplicationWindow::className() const
{
    return "GUIApplicationWindow";
}

QMetaObject *GUIApplicationWindow::metaObj = 0;

void GUIApplicationWindow::initMetaObject()
{
    if ( metaObj )
	return;
    if ( strcmp(QMainWindow::className(), "QMainWindow") != 0 )
	badSuperclassWarning("GUIApplicationWindow","QMainWindow");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION
QString GUIApplicationWindow::tr(const char* s)
{
    return ((QNonBaseApplication*)qApp)->translate("GUIApplicationWindow",s);
}

#endif // QT_NO_TRANSLATION
QMetaObject* GUIApplicationWindow::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    (void) QMainWindow::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    typedef void(GUIApplicationWindow::*m1_t0)();
    typedef void(GUIApplicationWindow::*m1_t1)();
    typedef void(GUIApplicationWindow::*m1_t2)();
    typedef void(GUIApplicationWindow::*m1_t3)();
    typedef void(GUIApplicationWindow::*m1_t4)();
    typedef void(GUIApplicationWindow::*m1_t5)();
    typedef void(GUIApplicationWindow::*m1_t6)();
    typedef void(GUIApplicationWindow::*m1_t7)();
    typedef void(GUIApplicationWindow::*m1_t8)();
    typedef void(GUIApplicationWindow::*m1_t9)();
    typedef void(GUIApplicationWindow::*m1_t10)();
    typedef void(GUIApplicationWindow::*m1_t11)();
    typedef void(GUIApplicationWindow::*m1_t12)();
    typedef void(GUIApplicationWindow::*m1_t13)(int);
    typedef void(GUIApplicationWindow::*m1_t14)(int);
    typedef void(GUIApplicationWindow::*m1_t15)(int);
    typedef void(GUIApplicationWindow::*m1_t16)();
    m1_t0 v1_0 = Q_AMPERSAND GUIApplicationWindow::load;
    m1_t1 v1_1 = Q_AMPERSAND GUIApplicationWindow::closeAllWindows;
    m1_t2 v1_2 = Q_AMPERSAND GUIApplicationWindow::start;
    m1_t3 v1_3 = Q_AMPERSAND GUIApplicationWindow::stop;
    m1_t4 v1_4 = Q_AMPERSAND GUIApplicationWindow::singleStep;
    m1_t5 v1_5 = Q_AMPERSAND GUIApplicationWindow::openNewMicroscopicWindow;
    m1_t6 v1_6 = Q_AMPERSAND GUIApplicationWindow::openNewLaneAggregatedWindow;
    m1_t7 v1_7 = Q_AMPERSAND GUIApplicationWindow::about;
    m1_t8 v1_8 = Q_AMPERSAND GUIApplicationWindow::aboutQt;
    m1_t9 v1_9 = Q_AMPERSAND GUIApplicationWindow::appSettings;
    m1_t10 v1_10 = Q_AMPERSAND GUIApplicationWindow::simSettings;
    m1_t11 v1_11 = Q_AMPERSAND GUIApplicationWindow::windowsMenuAboutToShow;
    m1_t12 v1_12 = Q_AMPERSAND GUIApplicationWindow::settingsMenuAboutToShow;
    m1_t13 v1_13 = Q_AMPERSAND GUIApplicationWindow::windowsMenuActivated;
    m1_t14 v1_14 = Q_AMPERSAND GUIApplicationWindow::windowSetings;
    m1_t15 v1_15 = Q_AMPERSAND GUIApplicationWindow::setSimulationDelay;
    m1_t16 v1_16 = Q_AMPERSAND GUIApplicationWindow::showLog;
    QMetaData *slot_tbl = QMetaObject::new_metadata(17);
    QMetaData::Access *slot_tbl_access = QMetaObject::new_metaaccess(17);
    slot_tbl[0].name = "load()";
    slot_tbl[0].ptr = *((QMember*)&v1_0);
    slot_tbl_access[0] = QMetaData::Private;
    slot_tbl[1].name = "closeAllWindows()";
    slot_tbl[1].ptr = *((QMember*)&v1_1);
    slot_tbl_access[1] = QMetaData::Private;
    slot_tbl[2].name = "start()";
    slot_tbl[2].ptr = *((QMember*)&v1_2);
    slot_tbl_access[2] = QMetaData::Private;
    slot_tbl[3].name = "stop()";
    slot_tbl[3].ptr = *((QMember*)&v1_3);
    slot_tbl_access[3] = QMetaData::Private;
    slot_tbl[4].name = "singleStep()";
    slot_tbl[4].ptr = *((QMember*)&v1_4);
    slot_tbl_access[4] = QMetaData::Private;
    slot_tbl[5].name = "openNewMicroscopicWindow()";
    slot_tbl[5].ptr = *((QMember*)&v1_5);
    slot_tbl_access[5] = QMetaData::Private;
    slot_tbl[6].name = "openNewLaneAggregatedWindow()";
    slot_tbl[6].ptr = *((QMember*)&v1_6);
    slot_tbl_access[6] = QMetaData::Private;
    slot_tbl[7].name = "about()";
    slot_tbl[7].ptr = *((QMember*)&v1_7);
    slot_tbl_access[7] = QMetaData::Private;
    slot_tbl[8].name = "aboutQt()";
    slot_tbl[8].ptr = *((QMember*)&v1_8);
    slot_tbl_access[8] = QMetaData::Private;
    slot_tbl[9].name = "appSettings()";
    slot_tbl[9].ptr = *((QMember*)&v1_9);
    slot_tbl_access[9] = QMetaData::Private;
    slot_tbl[10].name = "simSettings()";
    slot_tbl[10].ptr = *((QMember*)&v1_10);
    slot_tbl_access[10] = QMetaData::Private;
    slot_tbl[11].name = "windowsMenuAboutToShow()";
    slot_tbl[11].ptr = *((QMember*)&v1_11);
    slot_tbl_access[11] = QMetaData::Private;
    slot_tbl[12].name = "settingsMenuAboutToShow()";
    slot_tbl[12].ptr = *((QMember*)&v1_12);
    slot_tbl_access[12] = QMetaData::Private;
    slot_tbl[13].name = "windowsMenuActivated(int)";
    slot_tbl[13].ptr = *((QMember*)&v1_13);
    slot_tbl_access[13] = QMetaData::Private;
    slot_tbl[14].name = "windowSetings(int)";
    slot_tbl[14].ptr = *((QMember*)&v1_14);
    slot_tbl_access[14] = QMetaData::Private;
    slot_tbl[15].name = "setSimulationDelay(int)";
    slot_tbl[15].ptr = *((QMember*)&v1_15);
    slot_tbl_access[15] = QMetaData::Public;
    slot_tbl[16].name = "showLog()";
    slot_tbl[16].ptr = *((QMember*)&v1_16);
    slot_tbl_access[16] = QMetaData::Public;
    metaObj = QMetaObject::new_metaobject(
	"GUIApplicationWindow", "QMainWindow",
	slot_tbl, 17,
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
