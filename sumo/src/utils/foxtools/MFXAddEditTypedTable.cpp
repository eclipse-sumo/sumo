#include <fx.h>
#include <fxkeys.h>
#include <utils/convert/TplConvert.h>
#include <utils/convert/ToString.h>
#include "MFXAddEditTypedTable.h"
/*
FXDEFMAP(MFXAddEditTypedTable) MFXAddEditTypedTableMap[]=
{
};
*/

// Object implementation
//FXIMPLEMENT(MFXAddEditTypedTable,FXTable,MFXAddEditTypedTableMap,ARRAYNUMBER(MFXAddEditTypedTableMap))
FXIMPLEMENT(MFXAddEditTypedTable,MFXEditableTable,NULL,0)


MFXAddEditTypedTable::MFXAddEditTypedTable(FXComposite *p, FXObject* tgt,
                                   FXSelector sel, FXuint opts,
                                   FXint x, FXint y, FXint w, FXint h,
                                   FXint pl,FXint pr,FXint pt,FXint pb)
    : MFXEditableTable(p, tgt, sel, opts, x, y, w, h, pl, pr, pt, pb)
{
    myNumberEditor =
        new FXRealSpinDial((FXComposite*)getParent(),1,this,
            ID_EDITOR,FRAME_NORMAL|LAYOUT_EXPLICIT);
    myNumberEditor->hide();

    myBoolEditor =
        new FXCheckButton((FXComposite*)getParent(),"",this,
            ID_EDITOR,ICON_BEFORE_TEXT|LAYOUT_LEFT|LAYOUT_SIDE_BOTTOM|CHECKBUTTON_NORMAL|FRAME_NORMAL|LAYOUT_EXPLICIT);
    myBoolEditor->hide();
}


MFXAddEditTypedTable::~MFXAddEditTypedTable()
{
}


void
MFXAddEditTypedTable::editItem(FXTableItem* item,FXint how)
{
    if(item==0) {
        editEnd();
        return;
    }
    if(myWriteProtectedCols.find(myEditedCol)!=myWriteProtectedCols.end()) {
        editEnd();
        return;
    }
    FXTableItem* it= item;
    myPreviousText = item->getText();
    FXint x = getColumnX(myEditedCol) + getRowHeader()->getWidth() + xpos;
    FXint y = getRowY(myEditedRow) + getColumnHeader()->getHeight() + ypos;
    FXIcon* icon = item->getIcon();
    if(icon) x += icon->getWidth() + 4;
    FXint vw = getViewportWidth();
    if(vertical->shown()) vw -= vertical->getWidth();
    if(vw>getColumnWidth(myEditedCol)) {
        vw = getColumnWidth(myEditedCol) + x;
    }
    switch(getCellType(myEditedCol)) {
    case CT_UNDEFINED:
    case CT_STRING:
        myEditor->setText(it->getText());
        myEditor->move(x, y);
        myEditor->resize(vw - x + 1, getRowHeight(myEditedRow) + 1);
        myEditor->show();
        myEditor->raise();
        myEditor->setFocus();
        if(how == 'I') {
            myEditor->killSelection();
            myEditor->setCursorPos(0);
        } else if(how == 'A') {
            myEditor->killSelection();
            myEditor->setCursorPos(myEditor->getText().length());
        } else myEditor->selectAll();
        break;
    case CT_REAL:
        {
            try {
                myNumberEditor->setValue(
                    TplConvert<char>::_2float(it->getText().text()));
            } catch (NumberFormatException &e) {
            } catch (EmptyData &e) {
            }
            NumberCellParams p = getNumberCellParams(myEditedCol);
            if(p.format!="undefined") {
                myNumberEditor->setFormatString((char*) p.format.c_str());
                myNumberEditor->setIncrements(p.steps1, p.steps2, p.steps3);
                myNumberEditor->setRange(p.min, p.max);
            }
            myNumberEditor->move(x, y);
            myNumberEditor->resize(vw - x + 1, getRowHeight(myEditedRow) + 1);
            myNumberEditor->show();
            myNumberEditor->raise();
            myNumberEditor->setFocus();
            myNumberEditor->selectAll();
        }
        break;
    case CT_INT:
        {
            try {
                myNumberEditor->setValue(
                    TplConvert<char>::_2int(it->getText().text()));
            } catch (NumberFormatException &e) {
            } catch (EmptyData &e) {
            }
            NumberCellParams p = getNumberCellParams(myEditedCol);
            if(p.format!="undefined") {
                myNumberEditor->setFormatString((char*) p.format.c_str());
                myNumberEditor->setIncrements(p.steps1, p.steps2, p.steps3);
                myNumberEditor->setRange(p.min, p.max);
            }
            myNumberEditor->move(x, y);
            myNumberEditor->resize(vw - x + 1, getRowHeight(myEditedRow) + 1);
            myNumberEditor->show();
            myNumberEditor->raise();
            myNumberEditor->setFocus();
            myNumberEditor->selectAll();
        }
        break;
    case CT_BOOL:
        try {
            myBoolEditor->setCheck(
                TplConvert<char>::_2bool(it->getText().text())
                ? true : false);
        } catch (NumberFormatException &e) {
        } catch (EmptyData &e) {
        }
        myBoolEditor->move(x, y);
        myBoolEditor->resize(vw - x + 1, getRowHeight(myEditedRow) + 1);
        myBoolEditor->show();
        myBoolEditor->raise();
        myBoolEditor->setFocus();
        break;
    }
    myEditedItem = it;
}


void
MFXAddEditTypedTable::editEnd()
{
    if(!myEditedItem) return;
    FXTableItem* item= myEditedItem;
    myEditedItem = NULL;
    switch(getCellType(myEditedCol)) {
    case CT_UNDEFINED:
    case CT_STRING:
        myEditor->hide();
        break;
    case CT_REAL:
    case CT_INT:
        myNumberEditor->hide();
        break;
    case CT_BOOL:
        break;
    }
    setFocus();
    FXString text;
    switch(getCellType(myEditedCol)) {
    case CT_UNDEFINED:
    case CT_STRING:
        text = myEditor->getText();
        break;
    case CT_REAL:
        text = toString(myNumberEditor->getValue()).c_str();
        break;
    case CT_INT:
        text = toString((int) myNumberEditor->getValue()).c_str();
        break;
    case CT_BOOL:
        text = myBoolEditor->getCheck()
            ? "t" : "f";
        break;
    }
    MFXEditedTableItem edited;
    edited.item = item;
    edited.row = myEditedRow;
    edited.col = myEditedCol;
    myEditedItem = 0;
    if(item->getText() == text) {
        target->handle(this,FXSEL(SEL_CHANGED, ID_TEXT_CHANGED), (void*) &edited);
        return;
    }
    if(handle(item, FXSEL(SEL_COMMAND,ID_EDITEND), &text)) {
        item->setText(text);
        handle(this, FXSEL(SEL_CHANGED,0), item);
    }
    killSelection(true);
    if(target) {
        MFXEditedTableItem edited;
        edited.item = item;
        edited.row = myEditedRow;
        edited.col = myEditedCol;
        if(!target->handle(this,FXSEL(SEL_CHANGED, ID_TEXT_CHANGED), (void*) &edited)) {
            item->setText(myPreviousText);
        } else {
            if(edited.row==getNumRows()-1) {
                insertRows(getNumRows(), 1, true);
                for(int i=0; i<getNumColumns(); i++) {
                    setItemText(getNumRows()-1, i, "");
                    setItemJustify(getNumRows()-1, i, JUSTIFY_CENTER_X);
                }
            }
        }
    }
}


void
MFXAddEditTypedTable::editCancel()
{
    if(!myEditedItem) return;
    myEditedItem = 0;
    myEditor->hide();
    setFocus();
    killSelection(true);
}


CellType
MFXAddEditTypedTable::getCellType(int pos) const
{
    if(myCellTypes.size()<=pos) {
        return CT_UNDEFINED;
    }
    return myCellTypes[pos];
}


void
MFXAddEditTypedTable::setCellType(int pos, CellType t)
{
    while(myCellTypes.size()<pos+1) {
        myCellTypes.push_back(CT_UNDEFINED);
    }
    myCellTypes[pos] = t;
}


void
MFXAddEditTypedTable::setNumberCellParams(int pos, double min, double max,
                                          double steps1,
                                          double steps2,
                                          double steps3,
                                          const std::string &format)
{
    while(myNumberCellParams.size()<pos+1) {
        NumberCellParams np;
        np.format = "undefined";
        myNumberCellParams.push_back(np);
    }
    NumberCellParams np;
    np.pos = pos;
    np.min = min;
    np.max = max;
    np.steps1 = steps1;
    np.steps2 = steps2;
    np.steps3 = steps3;
    np.format = format;
    myNumberCellParams[pos] = np;
}


MFXAddEditTypedTable::NumberCellParams
MFXAddEditTypedTable::getNumberCellParams(int pos) const
{
    if(myNumberCellParams.size()<=pos) {
        NumberCellParams np;
        np.format = "undefined";
        return np;
    }
    return myNumberCellParams[pos];
}

