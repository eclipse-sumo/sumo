/****************************************************************************/
/// @file    GUITriggeredRerouter.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Mon, 25.07.2005
/// @version $Id$
///
// Reroutes vehicles passing an edge (gui-version)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUITriggeredRerouter_h
#define GUITriggeredRerouter_h


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
#include <foreign/rtree/SUMORTree.h>
#include <microsim/trigger/MSTriggeredRerouter.h>
#include <utils/foxtools/FXRealSpinDial.h>
#include <utils/gui/globjects/GUIGlObject_AbstractAdd.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <gui/GUIManipulator.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUITriggeredRerouter
 * @brief Reroutes vehicles passing an edge
 * One rerouter can be active on multiple edges. To reduce drawing load we
 * instantiate GUIGlObjects for every edge
 * XXX multiple rerouters active on the same edge are problematic
 */
class GUITriggeredRerouter
    : public MSTriggeredRerouter,
      public GUIGlObject_AbstractAdd {
public:
    /** @brief Constructor
     * @param[in] id The id of the rerouter
     * @param[in] edges The edges the rerouter is placed at
     * @param[in] prob The probability to reroute vehicles
     * @param[in] aXMLFilename The file to read further definitions from
     * @param[in] off Whether the rerouter is off (not working) initially
     */
    GUITriggeredRerouter(const std::string& id,
                         const std::vector<MSEdge*>& edges, SUMOReal prob,
                         const std::string& aXMLFilename, bool off,
                         SUMORTree& rtree);


    /// @brief Destructor
    ~GUITriggeredRerouter();


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



    GUIManipulator* openManipulator(GUIMainWindow& app,
                                    GUISUMOAbstractView& parent);

public:
    class GUITriggeredRerouterEdge : public GUIGlObject {

    public:
        GUITriggeredRerouterEdge(GUIEdge* edge, GUITriggeredRerouter* parent, bool closed);

        virtual ~GUITriggeredRerouterEdge();

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

    private:
        /// Definition of a positions container
        typedef std::vector<Position> PosCont;

        /// Definition of a rotation container
        typedef std::vector<SUMOReal> RotCont;

    private:
        /// The parent rerouter to which this edge instance belongs
        GUITriggeredRerouter* myParent;

        /// The edge for which this visualization applies
        MSEdge* myEdge;

        /// whether this edge instance visualizes a closed edge
        const bool myAmClosedEdge;

        /// The positions in full-geometry mode
        PosCont myFGPositions;

        /// The rotations in full-geometry mode
        RotCont myFGRotations;

        /// The boundary of this rerouter
        Boundary myBoundary;
    };

public:
    class GUITriggeredRerouterPopupMenu : public GUIGLObjectPopupMenu {
        FXDECLARE(GUITriggeredRerouterPopupMenu)
    public:

        GUITriggeredRerouterPopupMenu(GUIMainWindow& app,
                                      GUISUMOAbstractView& parent, GUIGlObject& o);

        ~GUITriggeredRerouterPopupMenu();

        /** @brief Called if the object's manipulator shall be shown */
        long onCmdOpenManip(FXObject*, FXSelector, void*);

    protected:
        GUITriggeredRerouterPopupMenu() { }

    };


    class GUIManip_TriggeredRerouter : public GUIManipulator {
        FXDECLARE(GUIManip_TriggeredRerouter)
    public:
        enum {
            MID_USER_DEF = FXDialogBox::ID_LAST,
            MID_PRE_DEF,
            MID_OPTION,
            MID_CLOSE,
            ID_LAST
        };
        /// Constructor
        GUIManip_TriggeredRerouter(GUIMainWindow& app,
                                   const std::string& name, GUITriggeredRerouter& o,
                                   int xpos, int ypos);

        /// Destructor
        virtual ~GUIManip_TriggeredRerouter();

        long onCmdOverride(FXObject*, FXSelector, void*);
        long onCmdClose(FXObject*, FXSelector, void*);
        long onCmdUserDef(FXObject*, FXSelector, void*);
        long onUpdUserDef(FXObject*, FXSelector, void*);
        long onCmdChangeOption(FXObject*, FXSelector, void*);

    private:
        GUIMainWindow* myParent;

        FXint myChosenValue;

        FXDataTarget myChosenTarget;

        SUMOReal myUsageProbability;

        FXRealSpinDial* myUsageProbabilityDial;

        FXDataTarget myUsageProbabilityTarget;

        GUITriggeredRerouter* myObject;

    protected:
        GUIManip_TriggeredRerouter() { }

    };


private:
    /// The boundary of this rerouter
    Boundary myBoundary;

    std::vector<GUITriggeredRerouterEdge*> myEdgeVisualizations;

};


#endif

/****************************************************************************/

