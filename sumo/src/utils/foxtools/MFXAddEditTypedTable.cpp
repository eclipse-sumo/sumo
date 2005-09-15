/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <fx.h>
#include <fxkeys.h>
#include <utils/convert/TplConvert.h>
#include <utils/convert/ToString.h>
#include "MFXAddEditTypedTable.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/*
// Map
FXDEFMAP(FXComboBox) FXComboBoxMap[]={
  FXMAPFUNC(SEL_FOCUS_UP,0,FXComboBox::onFocusUp),
  FXMAPFUNC(SEL_FOCUS_DOWN,0,FXComboBox::onFocusDown),
  FXMAPFUNC(SEL_FOCUS_SELF,0,FXComboBox::onFocusSelf),
  FXMAPFUNC(SEL_UPDATE,FXComboBox::ID_TEXT,FXComboBox::onUpdFmText),
  FXMAPFUNC(SEL_CLICKED,FXComboBox::ID_LIST,FXComboBox::onListClicked),
  FXMAPFUNC(SEL_LEFTBUTTONPRESS,FXComboBox::ID_TEXT,FXComboBox::onTextButton),
  FXMAPFUNC(SEL_CHANGED,FXComboBox::ID_TEXT,FXComboBox::onTextChanged),
  FXMAPFUNC(SEL_COMMAND,FXComboBox::ID_TEXT,FXComboBox::onTextCommand),
  FXMAPFUNC(SEL_COMMAND,FXWindow::ID_SETVALUE,FXComboBox::onFwdToText),
  FXMAPFUNC(SEL_COMMAND,FXWindow::ID_SETINTVALUE,FXComboBox::onFwdToText),
  FXMAPFUNC(SEL_COMMAND,FXWindow::ID_SETREALVALUE,FXComboBox::onFwdToText),
  FXMAPFUNC(SEL_COMMAND,FXWindow::ID_SETSTRINGVALUE,FXComboBox::onFwdToText),
  FXMAPFUNC(SEL_COMMAND,FXWindow::ID_GETINTVALUE,FXComboBox::onFwdToText),
  FXMAPFUNC(SEL_COMMAND,FXWindow::ID_GETREALVALUE,FXComboBox::onFwdToText),
  FXMAPFUNC(SEL_COMMAND,FXWindow::ID_GETSTRINGVALUE,FXComboBox::onFwdToText),
  };


// Object implementation
FXIMPLEMENT(FXComboBox,FXPacker,FXComboBoxMap,ARRAYNUMBER(FXComboBoxMap))




// Replace text of item at index
FXint FXComboBox::setItem(FXint index,const FXString& text,void* ptr){
  if(index<0 || list->getNumItems()<=index){ fxerror("%s::setItem: index out of range.\n",getClassName()); }
  list->setItem(index,text,NULL,ptr);
  if(isItemCurrent(index)){
    field->setText(text);
    }
//  recalc();
  return index;
  }


// Insert item at index
FXint FXComboBox::insertItem(FXint index,const FXString& text,void* ptr){
  if(index<0 || list->getNumItems()<index){ fxerror("%s::insertItem: index out of range.\n",getClassName()); }
  list->insertItem(index,text,NULL,ptr);
  if(isItemCurrent(index)){
    field->setText(text);
    }
//  recalc();
  return index;
  }


// Append item
FXint FXComboBox::appendItem(const FXString& text,void* ptr){
  FXint index=list->appendItem(text,NULL,ptr);
  if(isItemCurrent(getNumItems()-1)){
    field->setText(text);
    }
//  recalc();
  return index;
  }


// Prepend item
FXint FXComboBox::prependItem(const FXString& text,void* ptr){
  FXint index=list->prependItem(text,NULL,ptr);
  if(isItemCurrent(0)){
    field->setText(text);
    }
//  recalc();
  return index;
  }


// Move item from oldindex to newindex
FXint FXComboBox::moveItem(FXint newindex,FXint oldindex){
  if(newindex<0 || list->getNumItems()<=newindex || oldindex<0 || list->getNumItems()<=oldindex){ fxerror("%s::moveItem: index out of range.\n",getClassName()); }
  FXint current=list->getCurrentItem();
  list->moveItem(newindex,oldindex);
  if(current!=list->getCurrentItem()){
    current=list->getCurrentItem();
    if(0<=current){
      field->setText(list->getItemText(current));
      }
    else{
      field->setText(" ");
      }
    }
//  recalc();
  return newindex;
  }


// Remove given item
void FXComboBox::removeItem(FXint index){
  FXint current=list->getCurrentItem();
  list->removeItem(index);
  if(index==current){
    current=list->getCurrentItem();
    if(0<=current){
      field->setText(list->getItemText(current));
      }
    else{
      field->setText(FXString::null);
      }
    }
//  recalc();
  }


// Remove all items
void FXComboBox::clearItems(){
  field->setText(FXString::null);
  list->clearItems();
//  recalc();
  }


// Set item text
void FXComboBox::setItemText(FXint index,const FXString& txt){
  if(isItemCurrent(index)) setText(txt);
  list->setItemText(index,txt);
//  recalc();
  }




// Change combobox style
void FXComboBox::setComboStyle(FXuint mode){
  FXuint opts=(options&~COMBOBOX_MASK)|(mode&COMBOBOX_MASK);
  if(opts!=options){
    options=opts;
    if(options&COMBOBOX_STATIC){
      field->setEditable(FALSE);                                // Non-editable
      list->setScrollStyle(SCROLLERS_TRACK|HSCROLLING_OFF);     // No scrolling
      }
    else{
      field->setEditable(TRUE);                                 // Editable
      list->setScrollStyle(SCROLLERS_TRACK|HSCROLLER_NEVER);    // Scrollable, but no scrollbar
      }
//    recalc();
    }
  }

*/

// ---------------------------------

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

    myEnumEditor =
        new FXComboBox(this, 10, this, ID_EDITOR);
    myEnumEditor->hide();
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
        myEditor->enable();
        myEditor->setFocus();
        myEditor->grab();
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
            } catch (NumberFormatException &) {
            } catch (EmptyData &) {
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
        //myNumberEditor->setRange(0,1000);
        break;
    case CT_INT:
        {
            try {
                myNumberEditor->setValue(
                    TplConvert<char>::_2int(it->getText().text()));
            } catch (NumberFormatException &) {
            } catch (EmptyData &) {
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
        } catch (NumberFormatException &) {
        } catch (EmptyData &) {
        }
        myBoolEditor->move(x, y);
        myBoolEditor->resize(vw - x + 1, getRowHeight(myEditedRow) + 1);
        myBoolEditor->show();
        myBoolEditor->raise();
        myBoolEditor->setFocus();
        break;
    case CT_ENUM:
        {
            myEnumEditor->hide();
            myEnumEditor->clearItems();
            if(myEnums.size()>myEditedCol) {
                for(size_t i=0; i<myEnums[myEditedCol].size(); i++) {
                    myEnumEditor->appendItem(myEnums[myEditedCol][i].c_str());
                }
            }
            if(myEnumEditor->findItem(it->getText())>=0) {
                myEnumEditor->setCurrentItem(
                    myEnumEditor->findItem(it->getText()));
            } else {
                myEnumEditor->setCurrentItem(0);
            }
            myEnumEditor->setNumVisible(
                myEnums[myEditedCol].size()<10
                ? myEnums[myEditedCol].size()
                : 10);
            myEnumEditor->layout();
            y = getRowY(myEditedRow) + getColumnHeader()->getHeight() + ypos
                - getRowHeight(myEditedRow);
            myEnumEditor->move(x, y);
            myEnumEditor->resize(vw - x + 1, getRowHeight(myEditedRow) + 1);
            myEnumEditor->show();
            myEnumEditor->raise();
            myEnumEditor->setFocus();
        }
        break;
    default:
        throw 1;
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
        myBoolEditor->hide();
        break;
    case CT_ENUM:
        myEnumEditor->hide();
        break;
    default:
        throw 1;
    }
    setFocus();
    FXString text;
    switch(getCellType(myEditedCol)) {
    case CT_UNDEFINED:
    case CT_STRING:
        text = myEditor->getText();
        break;
    case CT_REAL:
        text = toString<float>((float) myNumberEditor->getValue()).c_str();
        break;
    case CT_INT:
        text = toString<int>((int) myNumberEditor->getValue()).c_str();
        break;
    case CT_BOOL:
        text = myBoolEditor->getCheck()
            ? "t" : "f";
        break;
    case CT_ENUM:
        text = myEnumEditor->getItem(myEnumEditor->getCurrentItem());//->getText();
        break;
    default:
        throw 1;
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
MFXAddEditTypedTable::getCellType(size_t pos) const
{
    if(myCellTypes.size()<=pos) {
        return CT_UNDEFINED;
    }
    return myCellTypes[pos];
}


void
MFXAddEditTypedTable::setCellType(size_t pos, CellType t)
{
    while(myCellTypes.size()<pos+1) {
        myCellTypes.push_back(CT_UNDEFINED);
    }
    myCellTypes[pos] = t;
}

void
MFXAddEditTypedTable::setNumberCellParams(size_t pos, double min, double max,
                                          double steps1,
                                          double steps2,
                                          double steps3,
                                          const std::string &format)
{
    while(myNumberCellParams.size()<=pos) {
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
MFXAddEditTypedTable::getNumberCellParams(size_t pos) const
{
    if(myNumberCellParams.size()<=pos) {
        NumberCellParams np;
        np.format = "undefined";
        return np;
    }
    return myNumberCellParams[pos];
}



void
MFXAddEditTypedTable::setEnums(size_t pos,
                               const std::vector<std::string> &params)
{
    while(myEnums.size()<=pos) {
        myEnums.push_back(std::vector<std::string>());
    }
    myEnums[pos] = params;
}


void
MFXAddEditTypedTable::addEnum(size_t pos,
                              const std::string &e)
{
    while(myEnums.size()<=pos) {
        myEnums.push_back(std::vector<std::string>());
    }
    myEnums[pos].push_back(e);
}


const std::vector<std::string> &
MFXAddEditTypedTable::getEnums(size_t pos) const
{
    return myEnums[pos];
}

