//---------------------------------------------------------------------------//
//                        QAboutSUMO.h -
//  The about dialog
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
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
// $Log$
// Revision 1.3  2003/06/05 11:37:31  dkrajzew
// class templates applied
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

/****************************************************************************
** Form interface generated from reading ui file 'd:\projects\sumo\sumo_about.ui'
**
** Created: Mon Mar 10 14:41:35 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef QABOUTSUMO_H
#define QABOUTSUMO_H

#include <qvariant.h>
#include <qdialog.h>
/* =========================================================================
 * class declarations
 * ======================================================================= */
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QPushButton;

class QAboutSUMO : public QDialog
{
    Q_OBJECT

public:
    QAboutSUMO( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~QAboutSUMO();

    QLabel* PixmapLabel2;
    QLabel* PixmapLabel3;
    QLabel* TextLabel5;
    QLabel* TextLabel2;
    QPushButton* PushButton1;
    QLabel* TextLabel1;
    QLabel* TextLabel3;
    QLabel* TextLabel4;

protected:
    bool event( QEvent* );
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "QAboutSUMO.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

