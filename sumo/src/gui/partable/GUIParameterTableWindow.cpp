//---------------------------------------------------------------------------//
//                        GUIParameterTableWindow.cpp -
//  The window that holds the table of an object's parameter
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.11  2004/08/02 11:42:27  dkrajzew
// ported to fox 1.2
//
// Revision 1.10  2004/07/02 08:25:01  dkrajzew
// some design issues
//
// Revision 1.9  2004/03/19 12:40:14  dkrajzew
// porting to FOX
//
// Revision 1.8  2003/11/20 13:18:10  dkrajzew
// further work on aggregated views
//
// Revision 1.7  2003/11/12 14:09:13  dkrajzew
// clean up after recent changes; comments added
//
// Revision 1.6  2003/08/14 13:38:25  dkrajzew
// applied other window controls
//
// Revision 1.5  2003/07/30 08:48:28  dkrajzew
// new parameter table usage paradigm; undocummented yet
//
// Revision 1.4  2003/07/18 12:30:14  dkrajzew
// removed some warnings
//
// Revision 1.3  2003/06/05 11:38:20  dkrajzew
// class templates applied; documentation added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <fx.h>
#include <guisim/GUINet.h>
#include <gui/GUIApplicationWindow.h>
#include "GUIParameterTableWindow.h"
#include <gui/GUIGlObject.h>
#include <utils/convert/ToString.h>
#include <gui/partable/GUIParam_PopupMenu.h>
#include <gui/GUIAppEnum.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * FOX callback mapping
 * ======================================================================= */
FXDEFMAP(GUIParameterTableWindow) GUIParameterTableWindowMap[]=
{
    FXMAPFUNC(SEL_COMMAND,          MID_SIMSTEP,    GUIParameterTableWindow::onSimStep),
    FXMAPFUNC(SEL_SELECTED,         MID_TABLE,      GUIParameterTableWindow::onTableSelected),
    FXMAPFUNC(SEL_DESELECTED,       MID_TABLE,      GUIParameterTableWindow::onTableDeselected),
    FXMAPFUNC(SEL_RIGHTBUTTONPRESS, MID_TABLE,      GUIParameterTableWindow::onRightButtonPress),
};

FXIMPLEMENT(GUIParameterTableWindow, FXMainWindow, GUIParameterTableWindowMap, ARRAYNUMBER(GUIParameterTableWindowMap))


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUIParameterTableWindow::GUIParameterTableWindow(GUIApplicationWindow &app,
                                                 GUIGlObject &o,
                                                 size_t noRows )
    : FXMainWindow(
        app.getApp() ,string(o.getFullName() + " Parameter").c_str(),
        NULL,NULL,DECOR_ALL,20,20,300,noRows*20+60),
    myObject(&o),
    myApplication(&app), myCurrentPos(0)
{
    myTable = new FXTable( this, this, MID_TABLE, TABLE_COL_SIZABLE|TABLE_ROW_SIZABLE|LAYOUT_FILL_X|LAYOUT_FILL_Y );
    myTable->setVisibleRows(noRows+1);
    myTable->setVisibleColumns(3);
    myTable->setTableSize(noRows+1, 3);
    myTable->setBackColor(FXRGB(255,255,255));
    myTable->setColumnText(0, "Name");
    myTable->setColumnText(1, "Value");
    myTable->setColumnText(2, "Dynamic");
    myTable->getRowHeader()->setWidth(0);
    FXHeader *header = myTable->getColumnHeader();
    header->setItemJustify(0, JUSTIFY_CENTER_X);
    header->setItemSize(0, 150);
    header->setItemJustify(1, JUSTIFY_CENTER_X);
    header->setItemSize(1, 80);
    header->setItemJustify(2, JUSTIFY_CENTER_X);
    header->setItemSize(2, 60);
}


GUIParameterTableWindow::~GUIParameterTableWindow()
{
    myApplication->removeChild(this);
}


long
GUIParameterTableWindow::onSimStep(FXObject*,FXSelector,void*)
{
    updateTable();
    update();
    return 1;
}


long
GUIParameterTableWindow::onTableSelected(FXObject*,FXSelector,void*)
{
    return 1;
}


long
GUIParameterTableWindow::onTableDeselected(FXObject*,FXSelector,void*)
{
    return 1;
}


long
GUIParameterTableWindow::onRightButtonPress(FXObject*sender,
                                            FXSelector sel,
                                            void*data)
{
    // check which value entry was pressed
    myTable->onLeftBtnPress(sender, sel, data);
    int row = myTable->getCurrentRow();
    if(row==-1) {
        return 1;
    }
    GUIParameterTableItem *i = myItems[row];
    if(!i->dynamic()) {
        return 1;
    }

    GUIParam_PopupMenu *p =
        new GUIParam_PopupMenu(*myApplication, *this,
            *myObject, i->getName(), i->getSourceCopy());
    int id;
    if(i->dynamic()) {
        new FXMenuCommand(p, "Open in new Tracker", 0, p, MID_OPENTRACKER);
    }
    // set geometry
    p->setX(static_cast<FXEvent*>(data)->root_x);
    p->setY(static_cast<FXEvent*>(data)->root_y);
    p->create();
    // show
    p->show();
    return 1;
}



void
GUIParameterTableWindow::mkItem(const char *name, bool dynamic,
                                ValueSource<double> *src)
{
    GUIParameterTableItem *i = new GUIParameterTableItem(
        myTable, myCurrentPos++, name, dynamic, src);
    myItems.push_back(i);
}


void
GUIParameterTableWindow::mkItem(const char *name, bool dynamic,
                                std::string value)
{
    GUIParameterTableItem *i = new GUIParameterTableItem(
        myTable, myCurrentPos++, name, dynamic, value);
    myItems.push_back(i);
}


void
GUIParameterTableWindow::mkItem(const char *name, bool dynamic,
                                double value)
{
    GUIParameterTableItem *i = new GUIParameterTableItem(
        myTable, myCurrentPos++, name, dynamic, value);
    myItems.push_back(i);
}


void
GUIParameterTableWindow::updateTable()
{
    for(std::vector<GUIParameterTableItem*>::iterator i=myItems.begin(); i!=myItems.end(); i++) {
        (*i)->update();
    }
}


void
GUIParameterTableWindow::closeBuilding()
{
    myApplication->addChild(this, true);
    create();
    show();
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


