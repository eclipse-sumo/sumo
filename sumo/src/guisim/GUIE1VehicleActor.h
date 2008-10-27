/****************************************************************************/
/// @file    GUIE1VehicleActor.h
/// @author  Daniel Krajzewicz
/// @date    Wed, 07.12.2005
/// @version $Id$
///
// GUI-version of MSBusStop
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
#ifndef GUIE1VehicleActor_h
#define GUIE1VehicleActor_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <string>
#include <utils/common/Command.h>
#include <microsim/trigger/MSE1VehicleActor.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/globjects/GUIGlObject_AbstractAdd.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/geom/HaveBoundary.h>
#include <utils/geom/Position2D.h>
#include <gui/GUIManipulator.h>
#include <utils/foxtools/FXRealSpinDial.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSNet;
class MSLane;
class GUIManipulator;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIE1VehicleActor
 * This is the gui-version of the MSE1VehicleActor-object
 */
class GUIE1VehicleActor : public MSE1VehicleActor, public GUIGlObject_AbstractAdd
{
public:
    /** constructor */
    GUIE1VehicleActor(const std::string& id, MSLane* lane,
                      SUMOReal positionInMeters, unsigned int laid,
                      unsigned int cellid, unsigned int type) throw();

    /** destructor */
    ~GUIE1VehicleActor() throw();

    /// @name inherited from GUIGlObject
    //@{

    /** @brief Returns an own popup-menu
     *
     * @param[in] app The application needed to build the popup-menu
     * @param[in] parent The parent window needed to build the popup-menu
     * @return The built popup-menu
     * @see GUIGlObject::getPopUpMenu
     */
    GUIGLObjectPopupMenu *getPopUpMenu(GUIMainWindow &app,
                                       GUISUMOAbstractView &parent) throw();


    /** @brief Returns an own parameter window
     *
     * @param[in] app The application needed to build the parameter window
     * @param[in] parent The parent window needed to build the parameter window
     * @return The built parameter window
     * @see GUIGlObject::getParameterWindow
     */
    GUIParameterTableWindow *getParameterWindow(GUIMainWindow &app,
            GUISUMOAbstractView &parent) throw();


    /** @brief Returns the id of the object as known to microsim
     *
     * @return The id of the actor
     * @see GUIGlObject::microsimID
     */
    const std::string &microsimID() const throw();


    /** @brief Returns the boundary to which the view shall be centered in order to show the object
     *
     * @return The boundary the object is within
     * @see GUIGlObject::getCenteringBoundary
     */
    Boundary getCenteringBoundary() const throw();
    //@}



    /// @name inherited from GUIGlObject_AbstractAdd
    //@{

    /** @brief Draws the object
     *
     * @param[in] scale The current scale (meters to pixel) of the view
     * @param[in] upscale The factor by which the object's size shall be multiplied
     * @see GUIGlObject_AbstractAdd::drawGL
     */
    void drawGL(const GUIVisualizationSettings &s) const throw();
    //@}


private:
    /// The position in full-geometry mode
    Position2D myFGPosition;

    /// The rotation in full-geometry mode
    SUMOReal myFGRotation;

    /// The position on the lane
    SUMOReal myPosition;

};


#endif

/****************************************************************************/

