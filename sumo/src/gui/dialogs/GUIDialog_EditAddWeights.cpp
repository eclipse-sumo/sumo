//---------------------------------------------------------------------------//
//                        GUIDialog_EditAddWeights.cpp -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Mon, 16 Jun 2004
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
// Revision 1.1  2004/07/02 08:10:56  dkrajzew
// edition of breakpoints and additional weights added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <set>
#include <guisim/GUILaneWrapper.h>
#include <gui/GUIApplicationWindow.h>
#include <gui/GUIAppEnum.h>
#include <gui/GUIGlobals.h>
#include <gui/GUIGlObject.h>
#include <guisim/GUINet.h>
#include <utils/convert/ToString.h>
#include <utils/convert/TplConvert.h>
#include "GUIDialog_EditAddWeights.h"
#include <gui/GUIGlobalSelection.h>
#include <utils/foxtools/MFXAddEditTypedTable.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/XMLHelpers.h>
#include <gui/GUISupplementaryWeightsHandler.h>
#include <utils/options/OptionsSubSys.h>
#include <utils/options/OptionsCont.h>


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
FXDEFMAP(GUIDialog_EditAddWeights) GUIDialog_EditAddWeightsMap[]=
{
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_LOAD,                   GUIDialog_EditAddWeights::onCmdLoad),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_SAVE,                   GUIDialog_EditAddWeights::onCmdSave),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_CLEAR,                  GUIDialog_EditAddWeights::onCmdClear),
    FXMAPFUNC(SEL_COMMAND,  MID_CANCEL,                         GUIDialog_EditAddWeights::onCmdClose),
    FXMAPFUNC(SEL_CHANGED,  MFXAddEditTypedTable::ID_TEXT_CHANGED,  GUIDialog_EditAddWeights::onCmdEditTable),

    FXMAPFUNC(SEL_UPDATE,   MID_CHOOSEN_SAVE,       GUIDialog_EditAddWeights::onUpdSave),
};


FXIMPLEMENT(GUIDialog_EditAddWeights, FXMainWindow, GUIDialog_EditAddWeightsMap, ARRAYNUMBER(GUIDialog_EditAddWeightsMap))


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUIDialog_EditAddWeights::GUIDialog_EditAddWeights(GUIApplicationWindow *parent)
    : FXMainWindow(gFXApp, "Additional Weights Editor", NULL, NULL, DECOR_ALL, 0, 0, 300, 300),
    myParent(parent), myEntriesAreValid(false)
{
    FXHorizontalFrame *hbox =
        new FXHorizontalFrame(this, LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0,
        0,0,0,0);

    // build the table
    myTable = new MFXAddEditTypedTable(hbox, this, MID_TABLE,
        LAYOUT_FILL_X|LAYOUT_FILL_Y);
    myTable->setVisibleRows(20);
    myTable->setVisibleColumns(6);
    myTable->setTableSize(20,6);
    myTable->setBackColor(FXRGB(255,255,255));
    myTable->setCellType(1, CT_INT);
    myTable->setNumberCellParams(1,
        OptionsSubSys::getOptions().getInt("b"),
        OptionsSubSys::getOptions().getInt("e"),
        1, 10, 100, "%.0f");
    myTable->setCellType(2, CT_INT);
    myTable->setNumberCellParams(2,
        OptionsSubSys::getOptions().getInt("b"),
        OptionsSubSys::getOptions().getInt("e"),
        1, 10, 100, "%.0f");
    myTable->setCellType(3, CT_REAL);
    myTable->setNumberCellParams(3, -100000000000000000.0, 1000000000000000.0,
        10, 100, 100000, "%.2f");
    myTable->setCellType(4, CT_REAL);
    myTable->setNumberCellParams(4, -100000000000000000.0, 1000000000000000.0,
        10, 100, 100000, "%.2f");
    myTable->setCellType(5, CT_REAL);
    myTable->setNumberCellParams(5, -100000000000000000.0, 1000000000000000.0,
        10, 100, 100000, "%.2f");
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
}


GUIDialog_EditAddWeights::~GUIDialog_EditAddWeights()
{
}


void
GUIDialog_EditAddWeights::rebuildList()
{
    myTable->clearItems();
    sort(gAddWeightsStorage.begin(), gAddWeightsStorage.end(), time_sorter());

    // set table attributes
    myTable->setTableSize(gAddWeightsStorage.size()+1, 6);
    myTable->setColumnText(0, "EdgeID");
    myTable->setColumnText(1, "TimeBeg");
    myTable->setColumnText(2, "TimeEnd");
    myTable->setColumnText(3, "Abs");
    myTable->setColumnText(4, "Add");
    myTable->setColumnText(5, "Mult");
    FXHeader *header = myTable->getColumnHeader();
    int k;
    for(k=0; k<6; k++) {
        header->setItemJustify(k, JUSTIFY_CENTER_X);
    }

    // insert into table
    size_t row = 0;
    GUIAddWeightsStorage::iterator j;
    for(j=gAddWeightsStorage.begin(); j!=gAddWeightsStorage.end(); ++j) {
        GUIAddWeight &aw = *j;
        std::string name = aw.edgeID;
        myTable->setItemText(row, 0, name.c_str());
        myTable->setItemText(row, 1,
            toString<int>((*j).timeBeg).c_str());
        myTable->setItemText(row, 2,
            toString<int>((*j).timeEnd).c_str());
        myTable->setItemText(row, 3,
            toString<float>((*j).abs).c_str());
        myTable->setItemText(row, 4,
            toString<float>((*j).add).c_str());
        myTable->setItemText(row, 5,
            toString<float>((*j).mult).c_str());
        // replace "invalid" values by empty fields
        for(k=1; k<6; k++) {
            string val = myTable->getItem(row, k)->getText().text();
            if(val==INVALID_VALUE_STR) {
                myTable->setItemText(row, k, " ");
            }
        }
        row++;
    }
    // insert dummy last field
    for(k=0; k<6; k++) {
        myTable->setItemText(row, k, " ");
    }
    //
    bool entriesAreValid = true;
    for(j=gAddWeightsStorage.begin(); j!=gAddWeightsStorage.end()&&entriesAreValid; ++j) {
        GUIAddWeight &aw = *j;
        if(aw.edgeID.find_first_not_of(" ")!=string::npos) {
            entriesAreValid = false;
        }
        if(aw.timeBeg==INVALID_VALUE) {
            entriesAreValid = false;
        }
        if(aw.timeEnd==INVALID_VALUE) {
            entriesAreValid = false;
        }
        if(aw.timeBeg>=aw.timeEnd) {
            entriesAreValid = false;
        }
    }
    myEntriesAreValid = entriesAreValid;
}


long
GUIDialog_EditAddWeights::onCmdLoad(FXObject*,FXSelector,void*)
{
    FXFileDialog opendialog(this, "Save Additional Weights");
    opendialog.setSelectMode(SELECTFILE_ANY);
    opendialog.setPatternList("*.xml");
    if(gCurrentFolder.length()!=0) {
        opendialog.setDirectory(gCurrentFolder.c_str());
    }
    if(opendialog.execute()){
        gCurrentFolder = opendialog.getDirectory().text();
        string file = string(opendialog.getFilename().text());
        GUISupplementaryWeightsHandler handler(file);
        XMLHelpers::runParser(handler, file);
        rebuildList();
    }
    return 1;
}


long
GUIDialog_EditAddWeights::onCmdSave(FXObject*,FXSelector,void*)
{
    // get the new file name
    FXFileDialog opendialog(this, "Save Additional Weights");
    opendialog.setSelectMode(SELECTFILE_ANY);
    opendialog.setPatternList("*.xml");
    if(gCurrentFolder.length()!=0) {
        opendialog.setDirectory(gCurrentFolder.c_str());
    }
    if(opendialog.execute()){
        gCurrentFolder = opendialog.getDirectory().text();
        string file = string(opendialog.getFilename().text());
        string content = encode2XML();
        ofstream strm(file.c_str());
        strm << content;
    }
    return 1;
}


std::string
GUIDialog_EditAddWeights::encode2XML()
{
    std::ostringstream strm;
    sort(gAddWeightsStorage.begin(), gAddWeightsStorage.end(), time_sorter());
    //
    strm << "<?xml version=\"1.0\" standalone=\"yes\"?>" << endl;
    strm << "<supplementary-weights>" << endl;
    for(GUIAddWeightsStorage::iterator j=gAddWeightsStorage.begin(); j!=gAddWeightsStorage.end(); ++j) {
        const GUIAddWeight &aw = (*j);
        if(aw.abs!=INVALID_VALUE||aw.add!=INVALID_VALUE||aw.mult!=INVALID_VALUE) {
            strm << "   <interval begin=\"" << aw.timeBeg << "\" end=\"" << aw.timeEnd << "\">" << endl;
            strm << "      <weight edge-id=\"" << aw.edgeID << "\" ";
            if(aw.abs!=INVALID_VALUE) {
                strm << "absolut=\"" << aw.abs << "\" ";
            }
            if(aw.add!=INVALID_VALUE) {
                strm << "add=\"" << aw.add << "\" ";
            }
            if(aw.mult!=INVALID_VALUE) {
                strm << "mult=\"" << aw.mult << "\" ";
            }
            strm << "/>" << endl;
            strm << "   </interval>" << endl;
        }
    }
    strm << "</supplementary-weights>" << endl;
    return strm.str();
}


long
GUIDialog_EditAddWeights::onCmdDeselect(FXObject*,FXSelector,void*)
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
GUIDialog_EditAddWeights::onCmdClear(FXObject*,FXSelector,void*)
{
    gAddWeightsStorage.clear();
    rebuildList();
    return 1;
}



long
GUIDialog_EditAddWeights::onCmdClose(FXObject*,FXSelector,void*)
{
    close(true);
    return 1;
}


long
GUIDialog_EditAddWeights::onUpdSave(FXObject*sender,FXSelector,void*ptr)
{
    sender->handle(this,
        myEntriesAreValid?FXSEL(SEL_COMMAND,ID_DISABLE):FXSEL(SEL_COMMAND,ID_ENABLE),
        ptr);
    return 1;
}


long
GUIDialog_EditAddWeights::onCmdEditTable(FXObject*,FXSelector,void*data)
{
    MFXEditedTableItem *i = (MFXEditedTableItem*) data;
    string value = i->item->getText().text();
    // check whether the inserted value is empty
    if(value.find_first_not_of(" ")==string::npos) {
        // replace by invalid if so
        value = INVALID_VALUE_STR;
    }
    GUIAddWeight aw;
    int row = i->row;
    if(row==gAddWeightsStorage.size()) {
        aw.edgeID = " ";
        aw.abs = INVALID_VALUE;
        aw.add = INVALID_VALUE;
        aw.mult = INVALID_VALUE;
        aw.timeBeg = 0;
        aw.timeEnd = 0;
        gAddWeightsStorage.push_back(aw);
    } else {
        aw = gAddWeightsStorage[row];
    }

    switch(i->col) {
    case 0:
        if(MSEdge::dictionary(value)==0) {
            string msg = string("The edge '") + value + string("' is not known.");
            FXMessageBox::error(this, MBOX_OK, "Invalid Edge Name",
                msg.c_str());
        } else {
            aw.edgeID = value;
        }
        break;
    case 1:
        try {
            aw.timeBeg = TplConvert<char>::_2int(value.c_str());
            if(aw.timeEnd==INVALID_VALUE) {
                aw.timeEnd = aw.timeBeg + 1;
            } else {
                if(aw.timeEnd<=aw.timeBeg) {
                    aw.timeEnd = aw.timeBeg + 1;
                    FXMessageBox::error(this, MBOX_OK, "Time Range Error",
                        "Period begin must be lower than period end");
                }
            }
        } catch (NumberFormatException) {
            string msg = string("The value must be an int, is:") + value;
            FXMessageBox::error(this, MBOX_OK, "Number format error",
                msg.c_str());
        }
        break;
    case 2:
        try {
            aw.timeEnd = TplConvert<char>::_2int(value.c_str());
            if(aw.timeBeg==INVALID_VALUE) {
                aw.timeBeg = aw.timeEnd - 1;
            } else {
                if(aw.timeEnd<=aw.timeBeg) {
                    aw.timeBeg = aw.timeEnd - 1;
                    FXMessageBox::error(this, MBOX_OK, "Time Range Error",
                        "Period begin must be lower than period end");
                }
            }
        } catch (NumberFormatException) {
            string msg = string("The value must be an int, is:") + value;
            FXMessageBox::error(this, MBOX_OK, "Number format error",
                msg.c_str());
        }
        break;
    case 3:
        try {
            aw.abs = TplConvert<char>::_2float(value.c_str());
        } catch (NumberFormatException) {
            string msg = string("The value must be a float, is:") + value;
            FXMessageBox::error(this, MBOX_OK, "Number format error",
                msg.c_str());
        }
        break;
    case 4:
        try {
            aw.add = TplConvert<char>::_2float(value.c_str());
        } catch (NumberFormatException) {
            string msg = string("The value must be a float, is:") + value;
            FXMessageBox::error(this, MBOX_OK, "Number format error",
                msg.c_str());
        }
        break;
    case 5:
        try {
            aw.mult = TplConvert<char>::_2float(value.c_str());
        } catch (NumberFormatException) {
            string msg = string("The value must be a float, is:") + value;
            FXMessageBox::error(this, MBOX_OK, "Number format error",
                msg.c_str());
        }
        break;
    default:
        throw 1;
    }
    gAddWeightsStorage[row] = aw;
    rebuildList();
    return 1;
}


FXbool
GUIDialog_EditAddWeights::close(FXbool notify)
{
    return FXMainWindow::close(notify);
}




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


