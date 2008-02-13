/****************************************************************************/
/// @file    GUIEmitter.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 21.07.2005
/// @version $Id$
///
// A vehicle emitting device (gui version)
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
#ifndef GUIEmitter_h
#define GUIEmitter_h


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
#include <microsim/trigger/MSEmitter.h>
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
 * @class GUIEmitter
 * @brief A vehicle emitting device (gui version)
 */
class GUIEmitter
            : public MSEmitter,
            public GUIGlObject_AbstractAdd
{
public:
    /** @brief Constructor
     *
     * @param[in] id The id of the emitter
     * @param[in] net The net the emitter belongs to
     * @param[in] destLane The lane the emitter is placed on
     * @param[in] pos Position of the emitter on the given lane
     * @param[in] file Name of the file to read the emission definitions from
     */
    GUIEmitter(const std::string &id, MSNet &net,
               MSLane* destLanes, SUMOReal pos,
               const std::string &file) throw();

    /** destructor */
    ~GUIEmitter() throw();

    /// @name inherited from GUIGlObject
    //@{

    /// Returns an own popup-menu
    GUIGLObjectPopupMenu *getPopUpMenu(GUIMainWindow &app,
                                       GUISUMOAbstractView &parent);

    /// Returns an own parameter window
    GUIParameterTableWindow *getParameterWindow(GUIMainWindow &app,
            GUISUMOAbstractView &parent);

    /// returns the id of the object as known to microsim
    const std::string &microsimID() const;

    /// Returns the information whether this object is still active
    bool active() const;
    //@}


    /// @name inherited from GUIAbstractAddGlObject
    //@{

    /// Draws the detector in full-geometry mode
    void drawGL(SUMOReal scale, SUMOReal upscale);

    /// Returns the detector's coordinates
    Position2D getPosition() const;
    //@}

    Boundary getBoundary() const;

    GUIManipulator *openManipulator(GUIMainWindow &app,
                                    GUISUMOAbstractView &parent);

    void setUserFlow(SUMOReal factor);
    SUMOReal getUserFlow() const;

    void setActiveChild(int index);

    void toggleDrawRoutes();

protected:
    class GUIEmitterChild_UserTriggeredChild : public MSEmitter::MSEmitterChild
    {
    public:
        GUIEmitterChild_UserTriggeredChild(
            MSEmitter_FileTriggeredChild &s, MSEmitter &parent,
            MSVehicleControl &vc, SUMOReal flow) throw();
        virtual ~GUIEmitterChild_UserTriggeredChild() throw();
        SUMOTime wrappedExecute(SUMOTime currentTime) throw(ProcessError);
        SUMOReal getUserFlow() const;

    protected:
        SUMOReal myUserFlow;
        MSVehicle *myVehicle;
        MSEmitter_FileTriggeredChild &mySource;
        bool myDescheduleVehicle;

    };

public:
class GUIEmitterPopupMenu : public GUIGLObjectPopupMenu
    {
        FXDECLARE(GUIEmitterPopupMenu)
    public:

        GUIEmitterPopupMenu(GUIMainWindow &app,
                            GUISUMOAbstractView &parent, GUIGlObject &o);

        ~GUIEmitterPopupMenu();

        /** @brief Called if the object's manipulator shall be shown */
        long onCmdOpenManip(FXObject*,FXSelector,void*);

        long onCmdDrawRoute(FXObject*,FXSelector,void*);

    protected:
        GUIEmitterPopupMenu() { }

    };

class GUIManip_TriggeredEmitter : public GUIManipulator
    {
        FXDECLARE(GUIManip_TriggeredEmitter)
    public:
        enum {
            MID_USER_DEF = FXDialogBox::ID_LAST,
            MID_PRE_DEF,
            MID_OPTION,
            MID_CLOSE,
            ID_LAST
        };
        /// Constructor
        GUIManip_TriggeredEmitter(GUIMainWindow &app,
                                  const std::string &name, GUIEmitter &o,
                                  int xpos, int ypos);

        /// Destructor
        virtual ~GUIManip_TriggeredEmitter();

        long onCmdOverride(FXObject*,FXSelector,void*);
        long onCmdClose(FXObject*,FXSelector,void*);
        long onCmdUserDef(FXObject*,FXSelector,void*);
        long onUpdUserDef(FXObject*,FXSelector,void*);
        long onCmdChangeOption(FXObject*,FXSelector,void*);

    private:
        GUIMainWindow *myParent;

        FXint myChosenValue;

        FXDataTarget myChosenTarget;

        SUMOReal myFlowFactor;

        FXDataTarget myFlowFactorTarget;

        FXRealSpinDial *myFlowFactorDial;

        GUIEmitter *myObject;

    protected:
        GUIManip_TriggeredEmitter() { }

    };

private:
    std::map<const MSEdge*, SUMOReal> getEdgeProbs() const;

private:
    /// The positions in full-geometry mode
    Position2D myFGPosition;

    /// The rotations in full-geometry mode
    SUMOReal myFGRotation;

    mutable SUMOReal myUserFlow;

    MSEmitterChild *myUserEmitChild;

    bool myDrawRoutes;

};


#endif

/****************************************************************************/

