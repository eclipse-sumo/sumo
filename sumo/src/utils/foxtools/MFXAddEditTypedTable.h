#ifndef MFXAddEditTypedTable_h
#define MFXAddEditTypedTable_h

#include "MFXEditableTable.h"
#include <vector>
#include <utils/foxtools/FXRealSpinDial.h>

enum CellType {
    CT_UNDEFINED = -1,
    CT_REAL = 0,
    CT_STRING = 1,
    CT_INT = 2,
    CT_BOOL = 3,
    CT_ENUM = 4,
    CT_MAX
};



class MFXAddEditTypedTable : public MFXEditableTable {
    FXDECLARE(MFXAddEditTypedTable)
public:
    MFXAddEditTypedTable(FXComposite *p,FXObject* tgt=NULL,FXSelector sel=0,FXuint opts=0,FXint x=0,FXint y=0,FXint w=0,FXint h=0,FXint pl=DEFAULT_MARGIN,FXint pr=DEFAULT_MARGIN,FXint pt=DEFAULT_MARGIN,FXint pb=DEFAULT_MARGIN);
    ~MFXAddEditTypedTable();

public:
    struct NumberCellParams {
        int pos;
        double min;
        double max;
        double steps1;
        double steps2;
        double steps3;
        std::string format;
    };

    void editItem(FXTableItem* item,FXint how);
    void editEnd();
    void editCancel();
    CellType getCellType(size_t pos) const;
    void setCellType(size_t pos, CellType t);
    void setNumberCellParams(size_t pos, double min, double max,
        double steps1, double steps2, double steps3,
        const std::string &format);
    NumberCellParams getNumberCellParams(size_t pos) const;
    void setEnums(size_t pos, const std::vector<std::string> &params);
    void addEnum(size_t pos, const std::string &e);
    const std::vector<std::string> &getEnums(size_t pos) const;

protected:
    std::vector<CellType> myCellTypes;
    std::vector<NumberCellParams> myNumberCellParams;
    std::vector<std::vector<std::string> > myEnums;
    FXRealSpinDial *myNumberEditor;
    FXCheckButton *myBoolEditor;
    FXComboBox *myEnumEditor;

protected:
    MFXAddEditTypedTable() { }

};


#endif


