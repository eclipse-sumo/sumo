/****************************************************************************/
/// @file    GUIDialog_EditViewport.h
/// @author  Daniel Krajzewicz
/// @date    2005-05-04
/// @version $Id$
///
// A dialog to change the viewport
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIDialog_EditViewport_h
#define GUIDialog_EditViewport_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>
#include <utils/foxtools/FXRealSpinDial.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUISUMOAbstractView;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIDialog_EditViewport
 * @brief A dialog to change the viewport
 */
class GUIDialog_EditViewport : public FXDialogBox {
    // FOX-declarations
    FXDECLARE(GUIDialog_EditViewport)
public:
    /// @brief FOX-callback enumerations
    enum {
        MID_CHANGED = FXDialogBox::ID_LAST,
        MID_OK,
        MID_CANCEL,
        MID_LOAD,
        MID_SAVE
    };


    /** @brief Constructor
     * @param[in] parent The view to change
     * @param[in] name This dialog's caption
     * @param[in] zoom Current view's zoom
     * @param[in] xoff Current view's x-offset
     * @param[in] yoff Current view's y-offset
     * @param[in] x x-position to show the dialog at
     * @param[in] y y-position to show the dialog at
     */
    GUIDialog_EditViewport(GUISUMOAbstractView* parent,  const char* name,
                           SUMOReal zoom, SUMOReal xoff, SUMOReal yoff, int x, int y) throw();


    /// @brief Destructor
    ~GUIDialog_EditViewport() throw();



    /// @name FOX-callbacks
    /// @{

    /// Called when the user changes the viewport
    long onCmdChanged(FXObject*,FXSelector,void*);

    /// Called when the user wants to keep the viewport
    long onCmdOk(FXObject*,FXSelector,void*);

    /// Called when the user wants to restore the viewport
    long onCmdCancel(FXObject*,FXSelector,void*);

    /// Called when the user wants to load a viewport
    long onCmdLoad(FXObject*,FXSelector,void*);

    /// Called when the user wants to save a viewport
    long onCmdSave(FXObject*,FXSelector,void*);
    /// @}



    /** @brief Sets the given values into the dialog
     * @param[in] zoom Current view's zoom
     * @param[in] xoff Current view's x-offset
     * @param[in] yoff Current view's y-offset
     */
    void setValues(SUMOReal zoom, SUMOReal xoff, SUMOReal yoff) throw();


    /** @brief Resets old values
     * @param[in] zoom Current view's zoom
     * @param[in] xoff Current view's x-offset
     * @param[in] yoff Current view's y-offset
     */
    void setOldValues(SUMOReal zoom, SUMOReal xoff, SUMOReal yoff) throw();


    /** @brief Returns the information whether one of the spin dialers is grabbed
     * @return Whether the spin dialers are currently used
     */
    bool haveGrabbed() const throw();


protected:
    /// @brief The calling view
    GUISUMOAbstractView *myParent;

    /// @brief The old zoom
    SUMOReal myOldZoom;

    /// @brief The old offsets
    SUMOReal myOldXOff, myOldYOff;

    /// @brief The spin dialers used to change the view
    FXRealSpinDial *myZoom, *myXOff, *myYOff;


protected:
    /// @brief FOX needs this
    GUIDialog_EditViewport() { }

};


#endif

/****************************************************************************/

