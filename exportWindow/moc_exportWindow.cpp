/****************************************************************************
** ExportWindow meta object code from reading C++ file 'exportWindow.h'
**
** Created: Wed Nov 28 17:00:05 2007
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.3   edited Aug 5 16:40 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "exportWindow.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *ExportWindow::className() const
{
    return "ExportWindow";
}

QMetaObject *ExportWindow::metaObj = 0;
static QMetaObjectCleanUp cleanUp_ExportWindow( "ExportWindow", &ExportWindow::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString ExportWindow::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "ExportWindow", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString ExportWindow::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "ExportWindow", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* ExportWindow::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QWidget::staticMetaObject();
    metaObj = QMetaObject::new_metaobject(
	"ExportWindow", parentObject,
	0, 0,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_ExportWindow.setMetaObject( metaObj );
    return metaObj;
}

void* ExportWindow::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "ExportWindow" ) )
	return this;
    return QWidget::qt_cast( clname );
}

bool ExportWindow::qt_invoke( int _id, QUObject* _o )
{
    return QWidget::qt_invoke(_id,_o);
}

bool ExportWindow::qt_emit( int _id, QUObject* _o )
{
    return QWidget::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool ExportWindow::qt_property( int id, int f, QVariant* v)
{
    return QWidget::qt_property( id, f, v);
}

bool ExportWindow::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
