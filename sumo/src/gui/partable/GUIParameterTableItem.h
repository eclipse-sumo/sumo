#ifndef GUIParameterTableItem_h
#define GUIParameterTableItem_h

#include <string>
#include <qlistview.h>

class DoubleValueSource;

class GUIParameterTableItem : public QListViewItem
{
public:
    GUIParameterTableItem(QListView *table, const std::string &name,
        bool dynamic, DoubleValueSource *src);
    GUIParameterTableItem(QListView *table, const std::string &name,
        bool dynamic, double value);
    ~GUIParameterTableItem();
    bool dynamic() const;
    const std::string &getName() const;
    size_t getTablePosition() const;
    void update();
    DoubleValueSource *getSourceCopy() const;
private:
    bool myAmDynamic;
    std::string myName;
    size_t myTablePosition;
    DoubleValueSource *mySource;
    double myValue;
};

#endif
