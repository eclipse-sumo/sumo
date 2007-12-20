/****************************************************************************/
/// @file    GUIParameterTableItem.h
/// @author  Daniel Krajzewicz
/// @date
/// @version $Id$
///
// A single line in the parameter window
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIParameterTableItem_h
#define GUIParameterTableItem_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <fx.h>
#include <utils/common/ValueSource.h>
#include <utils/gui/div/GUIParam_PopupMenu.h>


// ===========================================================================
// class definitions
// ===========================================================================
class GUIParameterTableItemInterface
{
public:
    virtual ~GUIParameterTableItemInterface() {}
    virtual GUIParam_PopupMenuInterface *buildPopupMenu(GUIMainWindow *, GUIParameterTableWindow *, GUIGlObject *) const = 0;
    virtual bool dynamic() const = 0;
    virtual void update() = 0;
};

/**
 * @class GUIParameterTableItem
 * This class represents a single item of a parameter table.
 * As some values may change over the simulation, this class holds the
 * information whether they change and how to ask for new values if they do
 */
template<class T>
class GUIParameterTableItem : public GUIParameterTableItemInterface
{
public:
    /// Constructor for changing values (SUMOReal-typed)
    GUIParameterTableItem(FXTable *table, size_t pos,
                          const std::string &name, bool dynamic, ValueSource<T> *src) : myAmDynamic(dynamic), myName(name), myTablePosition(pos), mySource(src),
            myValue(src->getValue()), myTable(table) {
        init(dynamic, toString<T>(src->getValue()));
    }


    /// Constructor for SUMOReal-typed, non-changing values
    GUIParameterTableItem(FXTable *table, size_t pos,
                          const std::string &name, bool dynamic, T value): myAmDynamic(dynamic), myName(name), myTablePosition(pos), mySource(0),
            myValue(value), myTable(table) {
        init(dynamic, toString<T>(value));
    }


    /// Constructor for string-typed, non-changing values
    GUIParameterTableItem(FXTable *table, size_t pos,
                          const std::string &name, bool dynamic, std::string value): myAmDynamic(dynamic), myName(name), myTablePosition(pos), mySource(0),
            myValue(0), myTable(table) {
        init(dynamic, value);
    }


    void init(bool dynamic, std::string value) {
        myTable->setItemText(myTablePosition, 0, myName.c_str());
        myTable->setItemText(myTablePosition, 1, value.c_str());
        if (dynamic) {
            myTable->setItemIcon(myTablePosition, 2,
                                 GUIIconSubSys::getIcon(ICON_YES));
        } else {
            myTable->setItemIcon(myTablePosition, 2,
                                 GUIIconSubSys::getIcon(ICON_NO));
        }
        myTable->setItemJustify(myTablePosition, 2,
                                FXTableItem::CENTER_X|FXTableItem::CENTER_Y);
    }



    /// Destructor
    ~GUIParameterTableItem() {
        delete mySource;
    }


    /// Returns the infomration whether this item may change
    bool dynamic() const {
        return myAmDynamic;
    }


    /// Returns the name of this item
    const std::string &getName() const {
        return myName;
    }


    /// Resets the value if it's dynamic
    void update() {
        if (!dynamic()||mySource==0) {
            return;
        }
        T value = mySource->getValue();
        if (value!=myValue) {
            myValue = value;
            myTable->setItemText(myTablePosition, 1,
                                 toString<T>(myValue).c_str());
        }
    }



    /// Returns a copy of the source - if the value is dynamic
    ValueSource<T> *getSourceCopy() const {
        if (mySource==0) {
            return 0;
        }
        return mySource->copy();
    }


    GUIParam_PopupMenuInterface *buildPopupMenu(GUIMainWindow *w, GUIParameterTableWindow *p, GUIGlObject *o) const {
        GUIParam_PopupMenuInterface *ret =
            new GUIParam_PopupMenu<T>(*w, *p, *o, getName(), getSourceCopy());
        if (dynamic()) {
            new FXMenuCommand(ret, "Open in new Tracker", 0, p, MID_OPENTRACKER);
        }
        return ret;
    }


private:
    /// Information whether the value may change
    bool myAmDynamic;

    /// The name of this entry
    std::string myName;

    /// The position within the table
    size_t myTablePosition;

    /** @brief The source to gain new values from
        This source is==0 if the values are not dynamic */
    ValueSource<T> *mySource;

    /// A backup of the value to avoid the redrawing when nothing has changed
    T myValue;

    FXTable *myTable;

};


#endif

/****************************************************************************/

