/****************************************************************************
** Form implementation generated from reading ui file 'MicroscopicViewSettings.ui'
**
** Created: Mon Aug 18 17:19:20 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
//---------------------------------------------------------------------------//
//                        GUIDialog_MicroViewSettings.cpp -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 29.05.2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.3  2005/07/12 11:55:37  dkrajzew
// fonts are now drawn using polyfonts; dialogs have icons; searching for structures improved;
//
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)

/* =========================================================================
 * included modules
 * ======================================================================= */
#include "GUIDialog_MicroViewSettings.h"

/*
 *  Constructs a GUIDialog_MicroViewSettings which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUIDialog_MicroViewSettings::GUIDialog_MicroViewSettings( FXMainWindow* parent,  const char* name)
    : FXDialogBox( parent, name )
{
/*!!!!        if ( !name )
	setName( "GUIDialog_MicroViewSettings" );
    resize( 349, 325 );
    setProperty( "sizePolicy", QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, sizePolicy().hasHeightForWidth() ) );
    setProperty( "minimumSize", QSize( 349, 325 ) );
    setProperty( "maximumSize", QSize( 349, 325 ) );
    setProperty( "caption", tr( "Settings for Microscopic View " ) );

    PushButton15 = new QPushButton( this, "PushButton15" );
    PushButton15->setGeometry( QRect( 51, 291, 80, 26 ) );
    PushButton15->setProperty( "text", tr( "OK" ) );
    PushButton15->setProperty( "default", QVariant( TRUE, 0 ) );

    PushButton16 = new QPushButton( this, "PushButton16" );
    PushButton16->setGeometry( QRect( 223, 291, 80, 26 ) );
    PushButton16->setProperty( "text", tr( "Cancel" ) );

    TabWidget5 = new QTabWidget( this, "TabWidget5" );
    TabWidget5->setGeometry( QRect( 10, 10, 330, 270 ) );

    tab = new QWidget( TabWidget5, "tab" );

    CheckBox5 = new QCheckBox( tab, "CheckBox5" );
    CheckBox5->setGeometry( QRect( 10, 10, 110, 20 ) );
    CheckBox5->setProperty( "text", tr( "Allow Rotation" ) );
    QWhatsThis::add(  CheckBox5, tr( "When enabled, the network my be rotated" ) );

    TextLabel11 = new QLabel( tab, "TextLabel11" );
    TextLabel11->setGeometry( QRect( 10, 30, 70, 20 ) );
    TextLabel11->setProperty( "text", tr( "Control Type" ) );

    ComboBox1 = new QComboBox( FALSE, tab, "ComboBox1" );
    ComboBox1->insertItem( tr( "Mouse Movement Folowing" ) );
    ComboBox1->setGeometry( QRect( 80, 30, 180, 22 ) );
    TabWidget5->insertTab( tab, tr( "Controls" ) );

    tab_2 = new QWidget( TabWidget5, "tab_2" );

    CheckBox7 = new QCheckBox( tab_2, "CheckBox7" );
    CheckBox7->setGeometry( QRect( 20, 40, 78, 20 ) );
    CheckBox7->setProperty( "text", tr( "Show scale" ) );

    CheckBox6 = new QCheckBox( tab_2, "CheckBox6" );
    CheckBox6->setGeometry( QRect( 20, 20, 78, 20 ) );
    CheckBox6->setProperty( "text", tr( "Show grid" ) );
    QWhatsThis::add(  CheckBox6, tr( "Toggles whether the grid shall be displayed" ) );
    TabWidget5->insertTab( tab_2, tr( "Global" ) );

    tab_3 = new QWidget( TabWidget5, "tab_3" );

    TextLabel12 = new QLabel( tab_3, "TextLabel12" );
    TextLabel12->setGeometry( QRect( 10, 10, 90, 20 ) );
    TextLabel12->setProperty( "text", tr( "Coloring scheme" ) );

    ComboBox2 = new QComboBox( FALSE, tab_3, "ComboBox2" );
    ComboBox2->insertItem( tr( "By speed" ) );
    ComboBox2->insertItem( tr( "By route" ) );
    ComboBox2->insertItem( tr( "As defined" ) );
    ComboBox2->insertItem( tr( "New Item" ) );
    ComboBox2->insertItem( tr( "Random #1" ) );
    ComboBox2->insertItem( tr( "Random #2" ) );
    ComboBox2->insertItem( tr( "By waiting time" ) );
    ComboBox2->insertItem( tr( "Lane change #1" ) );
    ComboBox2->insertItem( tr( "Lane change #2" ) );
    ComboBox2->setGeometry( QRect( 100, 10, 160, 22 ) );
    QWhatsThis::add(  ComboBox2, tr( "Changes the way the colors are assigned to vehicles" ) );
    TabWidget5->insertTab( tab_3, tr( "Vehicle Display Options" ) );

    tab_4 = new QWidget( TabWidget5, "tab_4" );

    TextLabel13 = new QLabel( tab_4, "TextLabel13" );
    TextLabel13->setGeometry( QRect( 10, 10, 80, 20 ) );
    TextLabel13->setProperty( "text", tr( "Coloring scheme" ) );

    ComboBox3 = new QComboBox( FALSE, tab_4, "ComboBox3" );
    ComboBox3->insertItem( tr( "All black" ) );
    ComboBox3->insertItem( tr( "By allowed speed" ) );
    ComboBox3->insertItem( tr( "By purpose" ) );
    ComboBox3->insertItem( tr( "Random by type #1" ) );
    ComboBox3->setGeometry( QRect( 100, 10, 160, 22 ) );
    TabWidget5->insertTab( tab_4, tr( "Lane Display Options" ) );
    */
}

/*
 *  Destroys the object and frees any allocated resources
 */
GUIDialog_MicroViewSettings::~GUIDialog_MicroViewSettings()
{
    // no need to delete child widgets, Qt does it all for us
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

