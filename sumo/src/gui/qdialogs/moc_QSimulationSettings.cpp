/****************************************************************************
** QSimulationSettings meta object code from reading C++ file 'QSimulationSettings.h'
**
** Created: Tue Sep 16 15:42:50 2003
**      by: The Qt MOC ($Id$)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#define Q_MOC_QSimulationSettings
#if !defined(Q_MOC_OUTPUT_REVISION)
#define Q_MOC_OUTPUT_REVISION 8
#elif Q_MOC_OUTPUT_REVISION != 8
#error "Moc format conflict - please regenerate all moc files"
#endif

#include "QSimulationSettings.h"
#include <qmetaobject.h>
#include <qapplication.h>

#if defined(Q_SPARCWORKS_FUNCP_BUG)
#define Q_AMPERSAND
#else
#define Q_AMPERSAND &
#endif


const char *QSimulationSettings::className() const
{
    return "QSimulationSettings";
}

QMetaObject *QSimulationSettings::metaObj = 0;

void QSimulationSettings::initMetaObject()
{
    if ( metaObj )
	return;
    if ( strcmp(QDialog::className(), "QDialog") != 0 )
	badSuperclassWarning("QSimulationSettings","QDialog");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION
QString QSimulationSettings::tr(const char* s)
{
    return ((QNonBaseApplication*)qApp)->translate("QSimulationSettings",s);
}

#endif // QT_NO_TRANSLATION
QMetaObject* QSimulationSettings::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    (void) QDialog::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    typedef void(QSimulationSettings::*m1_t0)();
    typedef void(QSimulationSettings::*m1_t1)();
    typedef void(QSimulationSettings::*m1_t2)();
    typedef void(QSimulationSettings::*m1_t3)();
    typedef void(QSimulationSettings::*m1_t4)();
    typedef void(QSimulationSettings::*m1_t5)();
    typedef void(QSimulationSettings::*m1_t6)();
    typedef void(QSimulationSettings::*m1_t7)();
    typedef void(QSimulationSettings::*m1_t8)();
    m1_t0 v1_0 = Q_AMPERSAND QSimulationSettings::pressedDumpChooser;
    m1_t1 v1_1 = Q_AMPERSAND QSimulationSettings::pressedAdditionalChooser;
    m1_t2 v1_2 = Q_AMPERSAND QSimulationSettings::pressedCancel;
    m1_t3 v1_3 = Q_AMPERSAND QSimulationSettings::pressedConfigChooser;
    m1_t4 v1_4 = Q_AMPERSAND QSimulationSettings::pressedEditActions;
    m1_t5 v1_5 = Q_AMPERSAND QSimulationSettings::pressedNetworkChooser;
    m1_t6 v1_6 = Q_AMPERSAND QSimulationSettings::pressedOK;
    m1_t7 v1_7 = Q_AMPERSAND QSimulationSettings::pressedRawChooser;
    m1_t8 v1_8 = Q_AMPERSAND QSimulationSettings::pressedRoutesChooser;
    QMetaData *slot_tbl = QMetaObject::new_metadata(9);
    QMetaData::Access *slot_tbl_access = QMetaObject::new_metaaccess(9);
    slot_tbl[0].name = "pressedDumpChooser()";
    slot_tbl[0].ptr = *((QMember*)&v1_0);
    slot_tbl_access[0] = QMetaData::Public;
    slot_tbl[1].name = "pressedAdditionalChooser()";
    slot_tbl[1].ptr = *((QMember*)&v1_1);
    slot_tbl_access[1] = QMetaData::Public;
    slot_tbl[2].name = "pressedCancel()";
    slot_tbl[2].ptr = *((QMember*)&v1_2);
    slot_tbl_access[2] = QMetaData::Public;
    slot_tbl[3].name = "pressedConfigChooser()";
    slot_tbl[3].ptr = *((QMember*)&v1_3);
    slot_tbl_access[3] = QMetaData::Public;
    slot_tbl[4].name = "pressedEditActions()";
    slot_tbl[4].ptr = *((QMember*)&v1_4);
    slot_tbl_access[4] = QMetaData::Public;
    slot_tbl[5].name = "pressedNetworkChooser()";
    slot_tbl[5].ptr = *((QMember*)&v1_5);
    slot_tbl_access[5] = QMetaData::Public;
    slot_tbl[6].name = "pressedOK()";
    slot_tbl[6].ptr = *((QMember*)&v1_6);
    slot_tbl_access[6] = QMetaData::Public;
    slot_tbl[7].name = "pressedRawChooser()";
    slot_tbl[7].ptr = *((QMember*)&v1_7);
    slot_tbl_access[7] = QMetaData::Public;
    slot_tbl[8].name = "pressedRoutesChooser()";
    slot_tbl[8].ptr = *((QMember*)&v1_8);
    slot_tbl_access[8] = QMetaData::Public;
    metaObj = QMetaObject::new_metaobject(
	"QSimulationSettings", "QDialog",
	slot_tbl, 9,
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
