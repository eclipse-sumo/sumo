/****************************************************************************/
/// @file    GNEPoly.h
/// @author  Jakob Erdmann
/// @date    Sept 2012
/// @version $Id$
///
// A class for visualizing and editing POIS in netedit (adapted from
// GUIPolygon and NLHandler)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GNEPoly_h
#define GNEPoly_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GNENetElement.h"

// ===========================================================================
// class declarations
// ===========================================================================
class GeoConvHelper;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEPoly
 *
 * In the case the represented junction's shape is empty, the boundary
 *  is computed using the junction's position to which an offset of 1m to each
 *  side is added.
 */
class GNEPoly : public GUIPolygon, public GNEAttributeCarrier {

    /// @brief declare friend class
    //friend class GNEChange_POI

public:
    class GNEPolyHandler : public SUMOSAXHandler {
    public:
        /// @brief constructor
        GNEPolyHandler(GNENet* net);

        /// @brief
        virtual ~GNEPolyHandler();

        /// @name inherited from GenericSAXHandler
        /// @{
        /**@brief Called on the opening of a tag;
         *
         * @param[in] element ID of the currently opened element
         * @param[in] attrs Attributes within the currently opened element
         * @exception ProcessError If something fails
         * @see GenericSAXHandler::myStartElement
         */
        virtual void myStartElement(int element, const SUMOSAXAttributes& attrs);

        /**@brief Called when a closing tag occurs
         *
         * @param[in] element ID of the currently opened element
         * @exception ProcessError If something fails
         * @see GenericSAXHandler::myEndElement
         */
        virtual void myEndElement(int element);
        /// @}

        int getNumLoaded() {
            return myNumLoaded;
        }

    private:
        GNENet* myNet;

        /// @brief The coordinate transformation which was used compute the POI coordinates
        GeoConvHelper* myLocation;

        /// @brief number of loaded pois
        int myNumLoaded;

    private:
        void loadPOI(const SUMOSAXAttributes& attrs);
    };

public:
    /**@brief Constructor
     */
    GNEPoly(GNENet* net, GNEJunction* junction, const std::string& id, const std::string& type, const PositionVector& shape, bool fill,
            const RGBColor& color, SUMOReal layer,
            SUMOReal angle = 0, const std::string& imgFile = "");

    /// @brief Destructor
    virtual ~GNEPoly();

    /// @name inherited from GUIGlObject
    /// @{

    /**@brief Returns an own popup-menu
     *
     * @param[in] app The application needed to build the popup-menu
     * @param[in] parent The parent window needed to build the popup-menu
     * @return The built popup-menu
     * @see GUIGlObject::getPopUpMenu
     */
    GUIGLObjectPopupMenu* getPopUpMenu(GUIMainWindow& app,
                                       GUISUMOAbstractView& parent);


    /**@brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    void drawGL(const GUIVisualizationSettings& s) const;
    /// @}


    /// @brief draw the polygon and also little movement handles

    /**@brief change the polygon geometry
     * It is up to the Polygon to decide whether an new geometry node should be
     * generated or an existing node should be moved
     * @param[in] oldPos The origin of the mouse movement
     * @param[in] newPos The destination of the mouse movenent
     * @param[in] relative Whether newPos is absolute or relative
     * @return newPos if something was moved, oldPos if nothing was moved
     */
    Position moveGeometry(const Position& oldPos, const Position& newPos, bool relative = false);

    /// @brief replace the current shape with a rectangle
    void simplifyShape();

    /// @brief delete the geometry point closest to the given pos
    void deleteGeometryNear(const Position& pos);

    /// @brief retrieve the junction of which the shape is being edited
    GNEJunction* getEditedJunction() const {
        return myJunction;
    }


    /// @brief registers completed movement with the undoList
    //void registerMove(GNEUndoList *undoList);


    /// @name inherited from GNEAttributeCarrier
    /// @{
    std::string getAttribute(SumoXMLAttr key) const;

    /* @brief method for setting the attribute and letting the object perform additional changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList);

    bool isValid(SumoXMLAttr key, const std::string& value);
    /// @}

    /// @brief load POIs from file
    static void loadFromFile(const std::string& file, GNENet* net);

    /// @brief save POIs to file
    static void saveToFile(const std::string& file);

protected:
    /// @brief the net for querying updates
    GNENet* myNet;

    /// @brief junction of which the shape is being edited (optional)
    GNEJunction* myJunction;

private:
    /// @brief Invalidated copy constructor.
    GNEPoly(const GNEPoly&);

    /// @brief Invalidated assignment operator.
    GNEPoly& operator=(const GNEPoly&);

    void setAttribute(SumoXMLAttr key, const std::string& value);

};


#endif

/****************************************************************************/

