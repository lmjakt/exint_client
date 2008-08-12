/****************************************************************************
** TDPointDrawer meta object code from reading C++ file '3dPointDrawer.h'
**
** Created: Thu May 1 14:19:01 2003
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.1.1   edited Dec 9 10:40 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "3dPointDrawer.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.1.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *TDPointDrawer::className() const
{
    return "TDPointDrawer";
}

QMetaObject *TDPointDrawer::metaObj = 0;
static QMetaObjectCleanUp cleanUp_TDPointDrawer( "TDPointDrawer", &TDPointDrawer::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString TDPointDrawer::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "TDPointDrawer", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString TDPointDrawer::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "TDPointDrawer", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* TDPointDrawer::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QGLWidget::staticMetaObject();
    static const QUParameter param_slot_0[] = {
	{ "p", &static_QUType_ptr, "vector<dpoint>", QUParameter::In }
    };
    static const QUMethod slot_0 = {"setModel", 1, param_slot_0 };
    static const QMetaData slot_tbl[] = {
	{ "setModel(vector<dpoint>)", &slot_0, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"TDPointDrawer", parentObject,
	slot_tbl, 1,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_TDPointDrawer.setMetaObject( metaObj );
    return metaObj;
}

void* TDPointDrawer::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "TDPointDrawer" ) )
	return this;
    return QGLWidget::qt_cast( clname );
}

bool TDPointDrawer::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: setModel((vector<dpoint>)(*((vector<dpoint>*)static_QUType_ptr.get(_o+1)))); break;
    default:
	return QGLWidget::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool TDPointDrawer::qt_emit( int _id, QUObject* _o )
{
    return QGLWidget::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool TDPointDrawer::qt_property( int id, int f, QVariant* v)
{
    return QGLWidget::qt_property( id, f, v);
}

bool TDPointDrawer::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
