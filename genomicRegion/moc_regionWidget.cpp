/****************************************************************************
** RegionWidget meta object code from reading C++ file 'regionWidget.h'
**
** Created: Mon Oct 6 11:03:47 2003
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.1.1   edited Dec 9 10:40 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "regionWidget.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.1.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *RegionWidget::className() const
{
    return "RegionWidget";
}

QMetaObject *RegionWidget::metaObj = 0;
static QMetaObjectCleanUp cleanUp_RegionWidget( "RegionWidget", &RegionWidget::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString RegionWidget::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "RegionWidget", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString RegionWidget::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "RegionWidget", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* RegionWidget::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QWidget::staticMetaObject();
    static const QUParameter param_slot_0[] = {
	{ "me", &static_QUType_int, 0, QUParameter::In },
	{ "mall", &static_QUType_int, 0, QUParameter::In },
	{ "mm", &static_QUType_int, 0, QUParameter::In },
	{ "cr", &static_QUType_ptr, "chromRegion", QUParameter::In }
    };
    static const QUMethod slot_0 = {"setData", 4, param_slot_0 };
    static const QUParameter param_slot_1[] = {
	{ "me", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_1 = {"setMaxExp", 1, param_slot_1 };
    static const QUParameter param_slot_2[] = {
	{ "mall", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_2 = {"setMinAll", 1, param_slot_2 };
    static const QUParameter param_slot_3[] = {
	{ "mm", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_3 = {"setMinMatch", 1, param_slot_3 };
    static const QUParameter param_slot_4[] = {
	{ "ss", &static_QUType_int, 0, QUParameter::In },
	{ "se", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_4 = {"setSelectRegion", 2, param_slot_4 };
    static const QUMethod slot_5 = {"loadProbeSets", 0, 0 };
    static const QUMethod slot_6 = {"getEnsemblGenes", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "setData(int,int,int,chromRegion*)", &slot_0, QMetaData::Public },
	{ "setMaxExp(int)", &slot_1, QMetaData::Public },
	{ "setMinAll(int)", &slot_2, QMetaData::Public },
	{ "setMinMatch(int)", &slot_3, QMetaData::Public },
	{ "setSelectRegion(int,int)", &slot_4, QMetaData::Public },
	{ "loadProbeSets()", &slot_5, QMetaData::Private },
	{ "getEnsemblGenes()", &slot_6, QMetaData::Private }
    };
    static const QUParameter param_signal_0[] = {
	{ 0, &static_QUType_ptr, "vector<int>", QUParameter::In },
	{ 0, &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod signal_0 = {"pleaseLoadGenes", 2, param_signal_0 };
    static const QUParameter param_signal_1[] = {
	{ 0, &static_QUType_int, 0, QUParameter::In },
	{ 0, &static_QUType_int, 0, QUParameter::In },
	{ 0, &static_QUType_int, 0, QUParameter::In },
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod signal_1 = {"selectRegionChanged", 4, param_signal_1 };
    static const QUParameter param_signal_2[] = {
	{ 0, &static_QUType_int, 0, QUParameter::In },
	{ 0, &static_QUType_ptr, "string", QUParameter::In },
	{ 0, &static_QUType_int, 0, QUParameter::In },
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod signal_2 = {"pleaseGetEnsemblGenes", 4, param_signal_2 };
    static const QMetaData signal_tbl[] = {
	{ "pleaseLoadGenes(vector<int>,QString)", &signal_0, QMetaData::Protected },
	{ "selectRegionChanged(int,int,int,int)", &signal_1, QMetaData::Protected },
	{ "pleaseGetEnsemblGenes(int,string,int,int)", &signal_2, QMetaData::Protected }
    };
    metaObj = QMetaObject::new_metaobject(
	"RegionWidget", parentObject,
	slot_tbl, 7,
	signal_tbl, 3,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_RegionWidget.setMetaObject( metaObj );
    return metaObj;
}

void* RegionWidget::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "RegionWidget" ) )
	return this;
    return QWidget::qt_cast( clname );
}

#include <qobjectdefs.h>
#include <qsignalslotimp.h>

// SIGNAL pleaseLoadGenes
void RegionWidget::pleaseLoadGenes( vector<int> t0, QString t1 )
{
    if ( signalsBlocked() )
	return;
    QConnectionList *clist = receivers( staticMetaObject()->signalOffset() + 0 );
    if ( !clist )
	return;
    QUObject o[3];
    static_QUType_ptr.set(o+1,&t0);
    static_QUType_QString.set(o+2,t1);
    activate_signal( clist, o );
}

// SIGNAL selectRegionChanged
void RegionWidget::selectRegionChanged( int t0, int t1, int t2, int t3 )
{
    if ( signalsBlocked() )
	return;
    QConnectionList *clist = receivers( staticMetaObject()->signalOffset() + 1 );
    if ( !clist )
	return;
    QUObject o[5];
    static_QUType_int.set(o+1,t0);
    static_QUType_int.set(o+2,t1);
    static_QUType_int.set(o+3,t2);
    static_QUType_int.set(o+4,t3);
    activate_signal( clist, o );
}

// SIGNAL pleaseGetEnsemblGenes
void RegionWidget::pleaseGetEnsemblGenes( int t0, string t1, int t2, int t3 )
{
    if ( signalsBlocked() )
	return;
    QConnectionList *clist = receivers( staticMetaObject()->signalOffset() + 2 );
    if ( !clist )
	return;
    QUObject o[5];
    static_QUType_int.set(o+1,t0);
    static_QUType_ptr.set(o+2,&t1);
    static_QUType_int.set(o+3,t2);
    static_QUType_int.set(o+4,t3);
    activate_signal( clist, o );
}

bool RegionWidget::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: setData((int)static_QUType_int.get(_o+1),(int)static_QUType_int.get(_o+2),(int)static_QUType_int.get(_o+3),(chromRegion*)static_QUType_ptr.get(_o+4)); break;
    case 1: setMaxExp((int)static_QUType_int.get(_o+1)); break;
    case 2: setMinAll((int)static_QUType_int.get(_o+1)); break;
    case 3: setMinMatch((int)static_QUType_int.get(_o+1)); break;
    case 4: setSelectRegion((int)static_QUType_int.get(_o+1),(int)static_QUType_int.get(_o+2)); break;
    case 5: loadProbeSets(); break;
    case 6: getEnsemblGenes(); break;
    default:
	return QWidget::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool RegionWidget::qt_emit( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->signalOffset() ) {
    case 0: pleaseLoadGenes((vector<int>)(*((vector<int>*)static_QUType_ptr.get(_o+1))),(QString)static_QUType_QString.get(_o+2)); break;
    case 1: selectRegionChanged((int)static_QUType_int.get(_o+1),(int)static_QUType_int.get(_o+2),(int)static_QUType_int.get(_o+3),(int)static_QUType_int.get(_o+4)); break;
    case 2: pleaseGetEnsemblGenes((int)static_QUType_int.get(_o+1),(string)(*((string*)static_QUType_ptr.get(_o+2))),(int)static_QUType_int.get(_o+3),(int)static_QUType_int.get(_o+4)); break;
    default:
	return QWidget::qt_emit(_id,_o);
    }
    return TRUE;
}
#ifndef QT_NO_PROPERTIES

bool RegionWidget::qt_property( int id, int f, QVariant* v)
{
    return QWidget::qt_property( id, f, v);
}

bool RegionWidget::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
