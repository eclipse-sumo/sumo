/****************************************************************************
** SmallMaps meta object code from reading C++ file 'smallmaps.h'
**
** Created: Fri Dec 8 15:07:34 2006
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.6   edited Mar 8 17:43 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "smallmaps.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *SmallMaps::className() const
{
    return "SmallMaps";
}

QMetaObject *SmallMaps::metaObj = 0;
static QMetaObjectCleanUp cleanUp_SmallMaps( "SmallMaps", &SmallMaps::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString SmallMaps::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "SmallMaps", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString SmallMaps::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "SmallMaps", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* SmallMaps::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QWidget::staticMetaObject();
    static const QUMethod slot_0 = {"createmap", 0, 0 };
    static const QUMethod slot_1 = {"createtraffic", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "createmap()", &slot_0, QMetaData::Private },
	{ "createtraffic()", &slot_1, QMetaData::Private }
    };
    metaObj = QMetaObject::new_metaobject(
	"SmallMaps", parentObject,
	slot_tbl, 2,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_SmallMaps.setMetaObject( metaObj );
    return metaObj;
}

void* SmallMaps::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "SmallMaps" ) )
	return this;
    return QWidget::qt_cast( clname );
}

bool SmallMaps::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: createmap(); break;
    case 1: createtraffic(); break;
    default:
	return QWidget::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool SmallMaps::qt_emit( int _id, QUObject* _o )
{
    return QWidget::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool SmallMaps::qt_property( int id, int f, QVariant* v)
{
    return QWidget::qt_property( id, f, v);
}

bool SmallMaps::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
