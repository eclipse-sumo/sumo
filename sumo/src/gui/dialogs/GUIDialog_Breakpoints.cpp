//---------------------------------------------------------------------------//
//                        GUIDialog_Breakpoints.cpp -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Thu, 17 Jun 2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
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
// Revision 1.10  2005/11/14 09:46:37  dkrajzew
// debugging editable tables
//
// Revision 1.9  2005/11/09 06:30:13  dkrajzew
// subwindows are now deleted on (re)loading the simulation
//
// Revision 1.8  2005/10/07 11:36:48  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.7  2005/09/22 13:30:40  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.6  2005/09/15 11:05:28  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.5  2005/07/12 11:55:37  dkrajzew
// fonts are now drawn using polyfonts; dialogs have icons; searching for structures improved;
//
// Revision 1.4  2005/04/27 09:44:26  dkrajzew
// level3 warnings removed
//
// Revision 1.3  2004/11/23 10:00:08  dkrajzew
// new class hierarchy for windows applied
//
// Revision 1.2  2004/08/02 11:28:57  dkrajzew
// ported to fox 1.2
//
// Revision 1.1  2004/07/02 08:10:56  dkrajzew
// edition of breakpoints and additional weights added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <set>
#include <guisim/GUILaneWrapper.h>
#include <gui/GUIApplicationWindow.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <guisim/GUINet.h>
#include <utils/common/ToString.h>
#include <utils/importio/LineReader.h>
#include <utils/common/TplConvert.h>
#include "GUIDialog_Breakpoints.h"
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/foxtools/MFXAddEditTypedTable.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/MsgHandler.h>
#include <gui/GUISupplementaryWeightsHandler.h>
#include <utils/options/OptionsSubSys.h>
#include <utils/options/OptionsCont.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/windows/GUIAppGlobals.h>
#include <utils/gui/windows/GUIAppGlobals.h>
#include <utils/gui/images/GUIIconSubSys.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * definitions
 * ======================================================================= */
#define INVALID_VALUE -1
#define INVALID_VALUE_STR "-1"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * FOX callback mapping
 * ======================================================================= */
FXDEFMAP(GUIDialog_Breakpoints) GUIDialog_BreakpointsMap[]=
{
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_LOAD,                   GUIDialog_Breakpoints::onCmdLoad),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_SAVE,                   GUIDialog_Breakpoints::onCmdSave),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_CLEAR,                  GUIDialog_Breakpoints::onCmdClear),
    FXMAPFUNC(SEL_COMMAND,  MID_CANCEL,                         GUIDialog_Breakpoints::onCmdClose),
    FXMAPFUNC(SEL_CHANGED,  MFXAddEditTypedTable::ID_TEXT_CHANGED,  GUIDialog_Breakpoints::onCmdEditTable),

//    FXMAPFUNC(SEL_UPDATE,   MID_CHOOSEN_SAVE,       GUIDialog_Breakpoints::onUpdSave),
};


FXIMPLEMENT(GUIDialog_Breakpoints, FXMainWindow, GUIDialog_BreakpointsMap, ARRAYNUMBER(GUIDialog_BreakpointsMap))


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUIDialog_Breakpoints::GUIDialog_Breakpoints(GUIMainWindow *parent)
    : FXMainWindow(gFXApp, "Breakpoints Editor", NULL, NULL, DECOR_ALL, 20,20,300, 300),
    myParent(parent)
{
    FXHorizontalFrame *hbox =
        new FXHorizontalFrame(this, LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0,
        0,0,0,0);

    // build the table
    myTable = new MFXAddEditTypedTable(hbox, this, MID_TABLE,
        LAYOUT_FILL_X|LAYOUT_FILL_Y);
    myTable->setVisibleRows(20);
    myTable->setVisibleColumns(1);
    myTable->setTableSize(20,1);
    myTable->setBackColor(FXRGB(255,255,255));
    myTable->setCellType(0, CT_INT);
    myTable->setNumberCellParams(0,
        OptionsSubSys::getOptions().getInt("b"),
        OptionsSubSys::getOptions().getInt("e"),
        1, 10, 100, "%.0f");
    myTable->getRowHeader()->setWidth(0);
    rebuildList();
    // build the layout
    FXVerticalFrame *layout = new FXVerticalFrame( hbox, LAYOUT_TOP,0,0,0,0,
        4,4,4,4);
    // "Load"
    new FXButton(layout, "Load\t\t", 0, this, MID_CHOOSEN_LOAD,
        ICON_BEFORE_TEXT|LAYOUT_FILL_X|FRAME_THICK|FRAME_RAISED,
        0, 0, 0, 0, 4, 4, 3, 3);
    // "Save"
    new FXButton(layout, "Save\t\t", 0, this, MID_CHOOSEN_SAVE,
        ICON_BEFORE_TEXT|LAYOUT_FILL_X|FRAME_THICK|FRAME_RAISED,
        0, 0, 0, 0, 4, 4, 3, 3);

    new FXHorizontalSeparator(layout,SEPARATOR_GROOVE|LAYOUT_FILL_X);

    // "Deselect Chosen"
    /*
    new FXButton(layout, "Deselect Chosen\t\t", 0, this, MID_CHOOSEN_DESELECT,
        ICON_BEFORE_TEXT|LAYOUT_FILL_X|FRAME_THICK|FRAME_RAISED,
        0, 0, 0, 0, 4, 4, 3, 3);
        */
    // "Clear List"
    new FXButton(layout, "Clear\t\t", 0, this, MID_CHOOSEN_CLEAR,
        ICON_BEFORE_TEXT|LAYOUT_FILL_X|FRAME_THICK|FRAME_RAISED,
        0, 0, 0, 0, 4, 4, 3, 3);

    new FXHorizontalSeparator(layout,SEPARATOR_GROOVE|LAYOUT_FILL_X);

    // "Close"
    new FXButton(layout, "Close\t\t", 0, this, MID_CANCEL,
        ICON_BEFORE_TEXT|LAYOUT_FILL_X|FRAME_THICK|FRAME_RAISED,
        0, 0, 0, 0, 4, 4, 3, 3);

    setIcon( GUIIconSubSys::getIcon(ICON_APP_BREAKPOINTS) );
    myParent->addChild(this);
}


GUIDialog_Breakpoints::~GUIDialog_Breakpoints()
{
    myParent->removeChild(this);
}


void
GUIDialog_Breakpoints::rebuildList()
{
    myTable->clearItems();
    sort(gBreakpoints.begin(), gBreakpoints.end(), time_sorter());

    // set table attributes
    myTable->setTableSize(gBreakpoints.size()+1, 1);
    myTable->setColumnText(0, "Time");
    FXHeader *header = myTable->getColumnHeader();
    header->setHeight(getApp()->getNormalFont()->getFontHeight()+getApp()->getNormalFont()->getFontAscent());
    int k;
    for(k=0; k<1; k++) {
        header->setItemJustify(k, JUSTIFY_CENTER_X);
    }

    // insert into table
    size_t row = 0;
    std::vector<int>::iterator j;
    for(j=gBreakpoints.begin(); j!=gBreakpoints.end(); ++j) {
        myTable->setItemText(row, 0,
            toString<int>(*j).c_str());
        row++;
    }
    // insert dummy last field
    for(k=0; k<1; k++) {
        myTable->setItemText(row, k, " ");
    }
}


long
GUIDialog_Breakpoints::onCmdLoad(FXObject*,FXSelector,void*)
{
    FXFileDialog opendialog(this, "Save Breakpoints");
    opendialog.setSelectMode(SELECTFILE_ANY);
    opendialog.setPatternList("*.txt");
    if(gCurrentFolder.length()!=0) {
        opendialog.setDirectory(gCurrentFolder.c_str());
    }
    if(opendialog.execute()){
        gCurrentFolder = opendialog.getDirectory().text();
        string file = string(opendialog.getFilename().text());
        ifstream strm(file.c_str());
        while(strm.good()) {
            std::string val;
            strm >> val;
            try {
                int value = TplConvert<char>::_2int(val.c_str());
                gBreakpoints.push_back(value);
            } catch (NumberFormatException&) {
                MsgHandler::getErrorInstance()->inform(
                    string(" A breakpoint-value must be an int, is:") + val);
            } catch (EmptyData&) {
            }
        }
        rebuildList();
    }
    return 1;
}


long
GUIDialog_Breakpoints::onCmdSave(FXObject*,FXSelector,void*)
{
    // get the new file name
    FXFileDialog opendialog(this, "Save Breakpoints");
    opendialog.setSelectMode(SELECTFILE_ANY);
    opendialog.setPatternList("*.txt");
    if(gCurrentFolder.length()!=0) {
        opendialog.setDirectory(gCurrentFolder.c_str());
    }
    if(opendialog.execute()){
        gCurrentFolder = opendialog.getDirectory().text();
        string file = string(opendialog.getFilename().text());
        string content = encode2TXT();
        ofstream strm(file.c_str());
        strm << content;
    }
    return 1;
}


std::string
GUIDialog_Breakpoints::encode2TXT()
{
    std::ostringstream strm;
    sort(gBreakpoints.begin(), gBreakpoints.end(), time_sorter());
    //
    for(std::vector<int>::iterator j=gBreakpoints.begin(); j!=gBreakpoints.end(); ++j) {
        if((*j)!=INVALID_VALUE) {
            strm << (*j) << endl;
        }
    }
    return strm.str();
}


long
GUIDialog_Breakpoints::onCmdDeselect(FXObject*,FXSelector,void*)
{
    /*
    size_t no = myList->getNumItems();
    vector<size_t> selected;
    size_t i;
    // remove items from list
    std::vector<size_t> chosen = gSelected.getAllSelected();
    std::vector<size_t>::iterator j = chosen.begin();
    for(i=0; i<no; i++) {
        if(myList->getItem(i)->isSelected()) {
            gSelected.deselect(-1, *j);
            selected.push_back(i);
        } else {
            j++;
        }
    }
    // rebuild list
    rebuildList();
    myParent->update();
    */
    return 1;
}



long
GUIDialog_Breakpoints::onCmdClear(FXObject*,FXSelector,void*)
{
    gBreakpoints.clear();
    rebuildList();
    return 1;
}



long
GUIDialog_Breakpoints::onCmdClose(FXObject*,FXSelector,void*)
{
    close(true);
    return 1;
}

/*
long
GUIDialog_Breakpoints::onUpdSave(FXObject*sender,FXSelector,void*ptr)
{
    sender->handle(this,
        myEntriesAreValid?FXSEL(SEL_COMMAND,ID_DISABLE):FXSEL(SEL_COMMAND,ID_ENABLE),
        ptr);
    return 1;
}

*/
long
GUIDialog_Breakpoints::onCmdEditTable(FXObject*,FXSelector,void*data)
{
    MFXEditedTableItem *i = (MFXEditedTableItem*) data;
    string value = i->item->getText().text();
    // check whether the inserted value is empty
    if(value.find_first_not_of(" ")==string::npos) {
        // replace by invalid if so
        value = INVALID_VALUE_STR;
    }
    int row = i->row;
    if(row==gBreakpoints.size()) {
        gBreakpoints.push_back(INVALID_VALUE);
    }

    switch(i->col) {
    case 0:
        try {
            gBreakpoints[row] = TplConvert<char>::_2int(value.c_str());
        } catch (NumberFormatException) {
            string msg = string("The value must be an int, is:") + value;
            FXMessageBox::error(this, MBOX_OK, "Number format error",
                msg.c_str());
        }
        break;
    default:
        throw 1;
    }
    rebuildList();
    return 1;
}


FXbool
GUIDialog_Breakpoints::close(FXbool notify)
{
    return FXMainWindow::close(notify);
}




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


