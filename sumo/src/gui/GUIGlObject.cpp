#include <qlistview.h>
#include <string>
#include <stack>
#include <utils/convert/ToString.h>
#include "GUISUMOAbstractView.h"
#include "QGLObjectPopupMenu.h"
#include "GUIGlObject.h"
#include "GUIGlObjectStorage.h"



// ------------------------

GUIGlObject::GUIGlObject(GUIGlObjectStorage &idStorage,
                         std::string fullName)
    : myFullName(fullName)
{
    idStorage.registerObject(this);
}


GUIGlObject::~GUIGlObject()
{
}


const std::string &
GUIGlObject::getFullName() const
{
    return myFullName;
}


size_t
GUIGlObject::getGlID() const
{
    return myGlID;
}


void
GUIGlObject::setGlID(size_t id)
{
    myGlID = id;
}



void
GUIGlObject::insertTableParameter(GUIParameterTableWindow *window,
                                  QListView *table,
                                  double *parameter,
                                  QListViewItem **vitems)
{
    fillTableParameter(parameter);


    std::stack<QListViewItem*> stack;
    QListViewItem *lvi = new QListViewItem(
        table, "id", getFullName().c_str());
    QListViewItem *plvi = lvi;
    stack.push(lvi);
    lvi->setOpen(TRUE);
    lvi->setExpandable(FALSE);

	int i = getTableParameterNo() - 1;
    for(; i>=0; i--) {
        const char * const item = getTableItem(i);
        const TableType const type = getTableType(i);
        switch(type)
        {
        case TT_DOUBLE:
            plvi==0
                ? lvi =  new QListViewItem(
                    table,
                    item,
                    toString<double>(parameter[i]).c_str())
                :  lvi =  new QListViewItem(
                    plvi,
                    item,
                    toString<double>(parameter[i]).c_str());
                vitems[i] = lvi;
            break;
        case TT_MENU_BEGIN:
            {
                const char *beginValue = getTableBeginValue(i);
                plvi==0
                    ? lvi =  new QListViewItem(
                        table,
                        item,
                        beginValue)
                    :  lvi =  new QListViewItem(
                        plvi,
                        item,
                        beginValue);
                stack.push(lvi);
                plvi = lvi;
                lvi->setOpen( FALSE );
                vitems[i] = lvi;
            }
            break;
        case TT_MENU_END:
            lvi = stack.top();
            stack.pop();
            if(stack.size()>0) {
                plvi = stack.top();
            } else {
                plvi = 0;
            }
            vitems[i] = 0;
            break;
        default:
            throw 1;
        }
    }
}


size_t
GUIGlObject::getTableParameterNo() const
{
    for(size_t i=0; ; i++) {
        const char * const item = getTableItem(i);
        if(item==0) {
            return i;
        }
    }
    throw 1;
    return 0;
}
