/****************************************************************************/
/// @file    GUITriggeredRerouter.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 25.07.2005
/// @version $Id$
///
// Reroutes vehicles passing an edge (gui-version)
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
#include <microsim/trigger/MSTriggeredRerouter.h>
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
class MSEdge;
class GUIManipulator;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUITriggeredRerouter
 * @brief Reroutes vehicles passing an edge
 */
class GUITriggeredRerouter
            : public MSTriggeredRerouter,
            public GUIGlObject_AbstractAdd
{
public:
    /** constructor */
    GUITriggeredRerouter(const std::string &id,
                         const std::vector<MSEdge*> &edges, SUMOReal prob,
                         const std::string &aXMLFilename, bool off);


    /** destructor */
    ~GUITriggeredRerouter() throw();


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
     * @return The id of the rerouter
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


    GUIManipulator *openManipulator(GUIMainWindow &app,
                                    GUISUMOAbstractView &parent);

public:
class GUITriggeredRerouterPopupMenu : public GUIGLObjectPopupMenu
    {
        FXDECLARE(GUITriggeredRerouterPopupMenu)
    public:

        GUITriggeredRerouterPopupMenu(GUIMainWindow &app,
                                      GUISUMOAbstractView &parent, GUIGlObject &o);

        ~GUITriggeredRerouterPopupMenu();

        /** @brief Called if the object's manipulator shall be shown */
        long onCmdOpenManip(FXObject*,FXSelector,void*);

    protected:
        GUITriggeredRerouterPopupMenu() { }

    };


class GUIManip_TriggeredRerouter : public GUIManipulator
    {
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
        GUIManip_TriggeredRerouter(GUIMainWindow &app,
                                   const std::string &name, GUITriggeredRerouter &o,
                                   int xpos, int ypos);

        /// Destructor
        virtual ~GUIManip_TriggeredRerouter();

        long onCmdOverride(FXObject*,FXSelector,void*);
        long onCmdClose(FXObject*,FXSelector,void*);
        long onCmdUserDef(FXObject*,FXSelector,void*);
        long onUpdUserDef(FXObject*,FXSelector,void*);
        long onCmdChangeOption(FXObject*,FXSelector,void*);

    private:
        GUIMainWindow *myParent;

        FXint myChosenValue;

        FXDataTarget myChosenTarget;

        SUMOReal myUsageProbability;

        FXRealSpinDial *myUsageProbabilityDial;

        FXDataTarget myUsageProbabilityTarget;

        GUITriggeredRerouter *myObject;

    protected:
        GUIManip_TriggeredRerouter() { }

    };

private:
    /// Definition of a positions container
    typedef std::vector<Position2D> PosCont;

    /// Definition of a rotation container
    typedef std::vector<SUMOReal> RotCont;

private:
    /// The positions in full-geometry mode
    PosCont myFGPositions;

    /// The rotations in full-geometry mode
    RotCont myFGRotations;

    /// The boundary of this rerouter
    Boundary myBoundary;

};


#endif

/****************************************************************************/

