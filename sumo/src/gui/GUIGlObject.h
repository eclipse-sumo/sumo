#ifndef GUIGlObject_h
#define GUIGlObject_h
//---------------------------------------------------------------------------//
//                        GUIGlObject.h -
//  A class for handling of displayed objects via their numerical id
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Oct 2002
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
// $Log$
// Revision 1.4  2003/06/05 06:26:16  dkrajzew
// first tries to build under linux: warnings removed; Makefiles added
//
// Revision 1.3  2003/05/20 09:23:54  dkrajzew
// some statistics added; some debugging done
//
// Revision 1.2  2003/04/14 08:24:55  dkrajzew
// unneeded display switch and zooming option removed; new glo-objct concept implemented; comments added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include "GUIGlObjectTypes.h"
#include "TableTypes.h"


 /* =========================================================================
 * class declarations
 * ======================================================================= */
class GUIGlObjectStorage;
class QGLObjectPopupMenu;
class GUISUMOAbstractView;
class GUIParameterTableWindow;
class QListView;
class GUIApplicationWindow;
class QListViewItem;


/* =========================================================================
 * class definitions
 * ======================================================================= */
class GUIGlObject {
public:
    /// Constructor
    GUIGlObject(GUIGlObjectStorage &idStorage,
        std::string fullName);

    /// Destructor
    virtual ~GUIGlObject();

    /// Returns the full name apperaing in the tool tip
    const std::string &getFullName() const;

    /// Returns the numerical id of the object
    size_t getGlID() const;

    /// Returns an own popup-menu
    virtual QGLObjectPopupMenu *getPopUpMenu(
        GUIApplicationWindow *app, GUISUMOAbstractView *parent) = 0;

    /// Returns the type of the object as coded in GUIGlObjectType
    virtual GUIGlObjectType getType() const = 0;

    /// returns the id of the object as known to microsim
    virtual std::string microsimID() const = 0;

    void insertTableParameter(GUIParameterTableWindow *window,
        QListView *table, double *parameter,
        QListViewItem **vitems);

    virtual size_t getTableParameterNo() const;

    virtual double getTableParameter(size_t pos) const = 0;

    virtual void fillTableParameter(double *parameter) const = 0;

    /// Needed to set the id
    friend class GUIGlObjectStorage;

    virtual const char * const getTableItem(size_t pos) const = 0;

	virtual bool active() const = 0;

protected:

    virtual TableType getTableType(size_t pos) const = 0;

    virtual const char *getTableBeginValue(size_t pos) const = 0;

private:
    /// Sets the id of the object
    void setGlID(size_t id);


private:
    /// The numerical id of the object
    size_t myGlID;

    /// The name of the object
    std::string myFullName;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "GUIGlObject.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

