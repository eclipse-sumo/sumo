#include <string>
#include <qlistview.h>
#include <qpixmap.h>
#include "GUIParameterTableItem.h"
#include <utils/convert/ToString.h>
#include <gui/icons/yes_no/yes.xpm>
#include <gui/icons/yes_no/no.xpm>
#include <microsim/logging/DoubleValueSource.h>

GUIParameterTableItem::GUIParameterTableItem(QListView *table,
                                             const std::string &name,
                                             bool dynamic,
                                             DoubleValueSource *src)
    : QListViewItem(table, name.c_str(),
    toString<double>(src->getValue()).c_str()),
    myAmDynamic(dynamic), myName(name), mySource(src),
    myValue(src->getValue())
{
    if(dynamic) {
        setPixmap(2, QPixmap(yes_xpm));
    } else {
        setPixmap(2, QPixmap(no_xpm));
    }
}


GUIParameterTableItem::GUIParameterTableItem(QListView *table,
                                             const std::string &name,
                                             bool dynamic,
                                             double value)
    : QListViewItem(table, name.c_str(),
    toString<double>(value).c_str()),
    myAmDynamic(dynamic), myName(name), mySource(0),
    myValue(value)
{
    if(dynamic) {
        setPixmap(2, QPixmap(yes_xpm));
    } else {
        setPixmap(2, QPixmap(no_xpm));
    }
}


GUIParameterTableItem::~GUIParameterTableItem()
{
}


bool
GUIParameterTableItem::dynamic() const
{
    return myAmDynamic;
}


const std::string &
GUIParameterTableItem::getName() const
{
    return myName;
}


void
GUIParameterTableItem::update()
{
    if(!dynamic()||mySource==0) {
        return;
    }
    double value = mySource->getValue();
    if(value!=myValue) {
        myValue = value;
        setText(1, toString<double>(value).c_str());
    }
}


DoubleValueSource *
GUIParameterTableItem::getSourceCopy() const
{
    if(mySource==0) {
        return 0;
    }
    return mySource->copy();
}


