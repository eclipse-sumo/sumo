/***************************************************************************
 *   Copyright (C) 2006 by Florides Andreas   *
 *   florides@cs.ucy.ac.cy   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#ifndef SMALLMAPS_H
#define SMALLMAPS_H

#include <qapplication.h>
#include <qlabel.h>
#include <qcolor.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qmultilineedit.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qtextview.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

class SmallMaps : public QWidget
{
    Q_OBJECT

public:

	QLineEdit *ed1,*ed2,*ed3,*ed4,*ed5;
	QTextView *med;

    SmallMaps( QWidget *parent = 0, const char *name = 0 );
    ~SmallMaps();
private slots:
void createmap();
void createtraffic();

};



#endif
