/****************************************************************************
** Form implementation generated from reading ui file 'ApplicationSettings.ui'
**
** Created: Mon Aug 18 17:19:20 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "QApplicationSettings.h"

#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a QApplicationSettings which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
QApplicationSettings::QApplicationSettings( QWidget* parent, bool &quitOnEnd)
    : QDialog( parent, 0,  TRUE, 0),
    myAppQuitOnEnd(quitOnEnd)
{
	setName( "QApplicationSettings" );
    resize( 222, 82 );
    setProperty( "sizePolicy", QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, sizePolicy().hasHeightForWidth() ) );
    setProperty( "minimumSize", QSize( 222, 82 ) );
    setProperty( "maximumSize", QSize( 222, 82 ) );
    setProperty( "baseSize", QSize( 41, 48 ) );
    setProperty( "caption", tr( "Application Settings" ) );
    setProperty( "sizeGripEnabled", QVariant( FALSE, 0 ) );

    CheckBox1 = new QCheckBox( this, "CheckBox1" );
    CheckBox1->setGeometry( QRect( 10, 10, 130, 20 ) );
    CheckBox1->setProperty( "text", tr( "Quit on simulation end" ) );
    QWhatsThis::add(  CheckBox1, tr( "If checked, the application will quit after the simulation has ended" ) );

    PushButton2 = new QPushButton( this, "PushButton2" );
    PushButton2->setGeometry( QRect( 120, 50, 93, 26 ) );
    PushButton2->setProperty( "text", tr( "Cancel" ) );

    PushButton1 = new QPushButton( this, "PushButton1" );
    PushButton1->setGeometry( QRect( 10, 50, 93, 26 ) );
    PushButton1->setProperty( "text", tr( "OK" ) );
    PushButton1->setProperty( "default", QVariant( TRUE, 0 ) );

    // signals and slots connections
    connect( PushButton1, SIGNAL( clicked() ), this, SLOT( pressedOK() ) );
    connect( PushButton2, SIGNAL( clicked() ), this, SLOT( pressedCancel() ) );

    //
    CheckBox1->setChecked(myAppQuitOnEnd);
}

/*
 *  Destroys the object and frees any allocated resources
 */
QApplicationSettings::~QApplicationSettings()
{
    // no need to delete child widgets, Qt does it all for us
}

void QApplicationSettings::pressedCancel()
{
    close();
}

void QApplicationSettings::pressedOK()
{
    myAppQuitOnEnd = CheckBox1->isChecked();
    close();
}

