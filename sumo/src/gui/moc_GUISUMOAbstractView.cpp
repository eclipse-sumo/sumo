/****************************************************************************
** GUISUMOAbstractView meta object code from reading C++ file 'GUISUMOAbstractView.h'
**
** Created: Wed Nov 26 09:33:59 2003
**      by: The Qt MOC ($Id$)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#define Q_MOC_GUISUMOAbstractView
#if !defined(Q_MOC_OUTPUT_REVISION)
#define Q_MOC_OUTPUT_REVISION 8
#elif Q_MOC_OUTPUT_REVISION != 8
#error "Moc format conflict - please regenerate all moc files"
#endif

#include "GUISUMOAbstractView.h"
#include <qmetaobject.h>
#include <qapplication.h>

#if defined(Q_SPARCWORKS_FUNCP_BUG)
#define Q_AMPERSAND
#else
#define Q_AMPERSAND &
#endif


const char *GUISUMOAbstractView::className() const
{
    return "GUISUMOAbstractView";
}

QMetaObject *GUISUMOAbstractView::metaObj = 0;

void GUISUMOAbstractView::initMetaObject()
{
    if ( metaObj )
	return;
    if ( strcmp(QGLWidget::className(), "QGLWidget") != 0 )
	badSuperclassWarning("GUISUMOAbstractView","QGLWidget");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION
QString GUISUMOAbstractView::tr(const char* s)
{
    return ((QNonBaseApplication*)qApp)->translate("GUISUMOAbstractView",s);
}

#endif // QT_NO_TRANSLATION
QMetaObject* GUISUMOAbstractView::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    (void) QGLWidget::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    typedef void(GUISUMOAbstractView::*m1_t0)();
    typedef void(GUISUMOAbstractView::*m1_t1)();
    typedef void(GUISUMOAbstractView::*m1_t2)(QMouseEvent*);
    typedef void(GUISUMOAbstractView::*m1_t3)(QMouseEvent*);
    typedef void(GUISUMOAbstractView::*m1_t4)(QMouseEvent*);
    m1_t0 v1_0 = Q_AMPERSAND GUISUMOAbstractView::toggleShowGrid;
    m1_t1 v1_1 = Q_AMPERSAND GUISUMOAbstractView::toggleToolTips;
    m1_t2 v1_2 = Q_AMPERSAND GUISUMOAbstractView::mouseMoveEvent;
    m1_t3 v1_3 = Q_AMPERSAND GUISUMOAbstractView::mousePressEvent;
    m1_t4 v1_4 = Q_AMPERSAND GUISUMOAbstractView::mouseReleaseEvent;
    QMetaData *slot_tbl = QMetaObject::new_metadata(5);
    QMetaData::Access *slot_tbl_access = QMetaObject::new_metaaccess(5);
    slot_tbl[0].name = "toggleShowGrid()";
    slot_tbl[0].ptr = *((QMember*)&v1_0);
    slot_tbl_access[0] = QMetaData::Public;
    slot_tbl[1].name = "toggleToolTips()";
    slot_tbl[1].ptr = *((QMember*)&v1_1);
    slot_tbl_access[1] = QMetaData::Public;
    slot_tbl[2].name = "mouseMoveEvent(QMouseEvent*)";
    slot_tbl[2].ptr = *((QMember*)&v1_2);
    slot_tbl_access[2] = QMetaData::Public;
    slot_tbl[3].name = "mousePressEvent(QMouseEvent*)";
    slot_tbl[3].ptr = *((QMember*)&v1_3);
    slot_tbl_access[3] = QMetaData::Public;
    slot_tbl[4].name = "mouseReleaseEvent(QMouseEvent*)";
    slot_tbl[4].ptr = *((QMember*)&v1_4);
    slot_tbl_access[4] = QMetaData::Public;
    metaObj = QMetaObject::new_metaobject(
	"GUISUMOAbstractView", "QGLWidget",
	slot_tbl, 5,
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
