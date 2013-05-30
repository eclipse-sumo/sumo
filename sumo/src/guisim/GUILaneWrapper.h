/****************************************************************************/
/// @file    GUILaneWrapper.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Mon, 25 Nov 2002
/// @version $Id$
///
// A MSLane extended for visualisation purposes.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUILaneWrapper_h
#define GUILaneWrapper_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utility>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/settings/GUIColorer.h>
#include <utils/common/VectorHelper.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUINet;
class GUIGLObjectPopupMenu;
class MSLane;
class MSEdge;
class PositionVector;
#ifdef HAVE_OSG
namespace osg {
class Geometry;
}
#endif


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUILaneWrapper
 * @brief A MSLane extended for visualisation purposes.
 *
 * The extensions consist of information about the position of the lane and
 *  it's direction. Further, a mechanism to avoid concurrent visualisation
 *  and simulation what may cause problems when vehicles disappear is
 *  implemented using a mutex.
 */
class GUILaneWrapper : public GUIGlObject {
public:
    /** @brief Constructor
     * @param[in] lane The lane to be represented
     * @param[in] shape The shape of the lane (!!! this is a member of the lane - no need to give it additionally)
     */
    GUILaneWrapper(MSLane& lane, const PositionVector& shape, unsigned int index);


    /// @brief Destructor
    virtual ~GUILaneWrapper();



    /// @name inherited from GUIGlObject
    //@{

    /** @brief Returns an own popup-menu
     *
     * @param[in] app The application needed to build the popup-menu
     * @param[in] parent The parent window needed to build the popup-menu
     * @return The built popup-menu
     * @see GUIGlObject::getPopUpMenu
     */
    GUIGLObjectPopupMenu* getPopUpMenu(GUIMainWindow& app,
                                       GUISUMOAbstractView& parent);


    /** @brief Returns an own parameter window
     *
     * @param[in] app The application needed to build the parameter window
     * @param[in] parent The parent window needed to build the parameter window
     * @return The built parameter window
     * @see GUIGlObject::getParameterWindow
     */
    GUIParameterTableWindow* getParameterWindow(GUIMainWindow& app,
            GUISUMOAbstractView& parent);


    /** @brief Returns the boundary to which the view shall be centered in order to show the object
     *
     * @return The boundary the object is within
     * @see GUIGlObject::getCenteringBoundary
     */
    Boundary getCenteringBoundary() const;


    /** @brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    void drawGL(const GUIVisualizationSettings& s) const;
    //@}



    /** @brief Returns the represented lane
     * @return The lane represented by this wrapper
     */
    const MSLane& getLane() const {
        return myLane;
    }


    /// Returns true if the given lane id the lane this wrapper wraps the geometry of
    bool forLane(const MSLane& lane) const;



private:
    /// The assigned lane
    MSLane& myLane;

    /// The shape of the lane
    const PositionVector& myShape;

private:
    /// @brief Invalidated copy constructor.
    GUILaneWrapper(const GUILaneWrapper&);

    /// @brief Invalidated assignment operator.
    GUILaneWrapper& operator=(const GUILaneWrapper&);
};


#endif

/****************************************************************************/

