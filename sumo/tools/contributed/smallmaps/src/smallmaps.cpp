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

#include "smallmaps.h"

using namespace std;


SmallMaps::SmallMaps( QWidget *parent, const char *name )
    : QWidget( parent, name )
{
    // Make the top-level layout; a vertical box to contain all widgets
    // and sub-layouts.
    QBoxLayout *topLayout = new QVBoxLayout( this, 5 );

    // Create a menubar...
    QMenuBar *menubar = new QMenuBar( this );
    menubar->setSeparator( QMenuBar::InWindowsStyle );
    QPopupMenu* popup;
    popup = new QPopupMenu( this );
    popup->insertItem( "&Quit", qApp, SLOT(quit()) );
    menubar->insertItem( "&File", popup );

    // ...and tell the layout about it.
    topLayout->setMenuBar( menubar );

    // Make an hbox that will hold a row of buttons.
    QBoxLayout *buttons = new QHBoxLayout( topLayout );
    
	QPushButton* but1 = new QPushButton( this );
	QString s;
	s.sprintf( "Create Small Map ");
	but1->setText( s );

	// Set horizontal stretch factor to 10 to let the buttons
	// stretch horizontally. The buttons will not stretch
	// vertically, since bigWidget below will take up vertical
	// stretch.
	buttons->addWidget( but1, 10 );
	// (Actually, the result would have been the same with a
	// stretch factor of 0; if no items in a layout have non-zero
	// stretch, the space is divided equally between members.)
    
    
	QPushButton* but2 = new QPushButton( this );
	s.sprintf( "Create random Traffic ");
	but2->setText( s );
	buttons->addWidget( but2, 10 );
    


    // Make a grid that will hold a vertical table of QLabel/QLineEdit
    // pairs next to a large QMultiLineEdit.

    // Don't use hard-coded row/column numbers in QGridLayout, you'll
    // regret it when you have to change the layout.
    const int labelCol = 0;
    const int linedCol = 1;
    const int multiCol = 2;

    // Let the grid-layout have a spacing of 10 pixels between
    // widgets, overriding the default from topLayout.
    QGridLayout *grid = new QGridLayout( topLayout, 0, 0, 10 );
    int row=0;

  
	ed1 = new QLineEdit( this );
	// The line edit goes in the second column
	grid->addWidget( ed1, row, linedCol );	
	ed1->setText("frida_bidi_plain.nod.xml");
	
	// Make a label that is a buddy of the line edit
	s.sprintf( "Original Map:" );
	QLabel *label1 = new QLabel( ed1, s, this );
	// The label goes in the first column.
	grid->addWidget( label1, row, labelCol );
	row++;
    
  
	ed2 = new QLineEdit( this );
	grid->addWidget( ed2, row, linedCol );	
	s.sprintf( "\tX1:" );
	QLabel *label2 = new QLabel( ed2, s, this );
	grid->addWidget( label2, row, labelCol );
	row++;
    
  
	ed3 = new QLineEdit( this );
	grid->addWidget( ed3, row, linedCol );	
	s.sprintf( "\tX2:" );
	QLabel *label3 = new QLabel( ed3, s, this );
	grid->addWidget( label3, row, labelCol );
	row++;
    
  
	ed4 = new QLineEdit( this );
	grid->addWidget( ed4, row, linedCol );	
	s.sprintf( "\tY1:" );
	QLabel *label4 = new QLabel( ed4, s, this );
	grid->addWidget( label4, row, labelCol );
	row++;
    
  
	ed5 = new QLineEdit( this );
	grid->addWidget( ed5, row, linedCol );	
	s.sprintf( "\tY2:" );
	QLabel *label5 = new QLabel( ed5, s, this );
	grid->addWidget( label5, row, labelCol );
	row++;
    

    // The multiline edit will cover the entire vertical range of the
    // grid (rows 0 to numRows) and stay in column 2.

    med = new QTextView( this );
    grid->addMultiCellWidget( med, 0, -1, multiCol, multiCol );

    // The labels will take the space they need. Let the remaining
    // horizontal space be shared so that the multiline edit gets
    // twice as much as the line edit.
    grid->setColStretch( linedCol, 10 );
    grid->setColStretch( multiCol, 20 );

    // Add a widget at the bottom.
    QLabel* sb = new QLabel( this );
    sb->setText( "Florides Andreas 2006 UCY" );
    sb->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    // This widget will use all horizontal space, and have a fixed height.

    // we should have made a subclass and implemented sizePolicy there...
    sb->setFixedHeight( sb->sizeHint().height() );

    sb->setAlignment( AlignVCenter | AlignLeft );
    topLayout->addWidget( sb );

    topLayout->activate();

// signals and slots connections
    connect( but1, SIGNAL( clicked() ), SLOT( createmap() ) );
    connect( but2, SIGNAL( clicked() ), SLOT( createtraffic() ) );
}

SmallMaps::~SmallMaps()
{
    // All child widgets are deleted by Qt.
    // The top-level layout and all its sub-layouts are deleted by Qt.
}

void
SmallMaps::createmap()
{
char buffer[1024];
FILE* fd ;

string fileName=ed1->text();
int x1=atoi(ed2->text());
int x2=atoi(ed3->text());
int y1=atoi(ed4->text());
int y2=atoi(ed5->text());
char tempstring[500];
//declares filename
ifstream d_file;
ofstream  OUT ("smallfrida.nod.xml");
string strb,temp;
int x,y;
char str[2000];
int whereid,wherex,wherey,whereend;

d_file.open(ed1->text()); //attempts to open file

if (!d_file.is_open())
{
//if file doesn't exist; don't create a new one
med->append(tr("File"));
med->append(ed1->text());
med->append(tr("does not exist in the client's current directory\n "));

}
else
{
sprintf(tempstring, "File OK. Start parsing with x:%d-%d and y:%d-%d",x1,x2,y1,y2);
med->append(tempstring);

 while(!d_file.eof())
        {
              d_file.getline(str,2000);
		strb=str;
		whereid=strb.find("id=",0);
		if (whereid != string::npos)
		{
		wherex=strb.find("x=",0);
		wherey=strb.find("y=",0);
		whereend=strb.find(">",0);
		x = atoi(strb.substr(wherex+3,wherey-12).c_str());
		if (x>x1&&x<x2)
		{
			y = atoi(strb.substr(wherey+3,whereend-2).c_str());
			if (y>y1&&y<y2) OUT << str << endl;
		}
		
		}
		else OUT << str << endl;
        }
        d_file.close();

	fd= popen("./sumo-netconvert --xml-node-files=smallfrida.nod.xml --xml-edge-files=frida_bidi_plain.edg.xml --output-file=smallfrida.net.xml --omit-corrupt-edges", "r");
        if (fd == NULL) {
                med->append( tr("Error executing netconvert, try manualy")); //error occured
        }
while(fgets(buffer, sizeof(buffer), fd)!=NULL) {
med->append(buffer);
}
pclose (fd);

}
}

void
SmallMaps::createtraffic()
{

char buffer[1024];
FILE* fd ;

med->append( tr("Creating random routes with jtrrouter...\n") );

fd= popen("export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib/:/usr/lib/ ; sumo-jtrrouter --net=smallfrida.net.xml -R 2 --output-file=smallfrida.rou.xml -b 0 -e 100 --continue-on-unbuild ", "r");
        if (fd == NULL) {
                med->append( tr("Error executing jtrrouter, try manualy")); //error occured
        }
while(fgets(buffer, sizeof(buffer), fd)!=NULL) {
med->append(buffer);
}
pclose (fd);

}
