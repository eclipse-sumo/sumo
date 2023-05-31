/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GUIOSGView.h
/// @author  Daniel Krajzewicz
/// @author  Mirko Barthauer
/// @date    19.01.2012
///
// An OSG-based 3D view on the simulation
/****************************************************************************/
#pragma once
#include <config.h>

#ifdef HAVE_OSG

#include "GUIOSGHeader.h"
#include "GUIOSGManipulator.h"

#include <string>
#include <microsim/traffic_lights/MSTLLogicControl.h>
#include <utils/geom/Boundary.h>
#include <utils/geom/Position.h>
#include <utils/common/RGBColor.h>
#include <utils/geom/PositionVector.h>
#include <gui/GUISUMOViewParent.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>


// ===========================================================================
// class declarations
// ===========================================================================

class GUINet;
class GUISUMOViewParent;
class GUIVehicle;
class GUILaneWrapper;
class MSRoute;
class MSTransportable;
class MSVehicle;

namespace osgGA {
class CameraManipulator;
}

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIOSGView
 * @brief An OSG-based 3D view on the simulation
 */
class GUIOSGView : public GUISUMOAbstractView {
    FXDECLARE(GUIOSGView)

public:
    friend class GUIOSGPerspectiveChanger;

    /// @brief Used osg::NodeSet groups
    enum NodeSetGroup {
        /// @brief semi-transparent domes around user-placed TLS models
        NODESET_TLSDOMES,
        /// @brief markers above lanes showing the signal state of the corresponding tlIndex
        NODESET_TLSLINKMARKERS,
        /// @brief auto-generated TLS models
        NODESET_TLSMODELS,
    };

    /**
     * @class Command_TLSChange
     * @brief Updates scene on each tls switch
     */
    class Command_TLSChange : public MSTLLogicControl::OnSwitchAction {
    public:
        /** @brief Constructor
         *
         * @param[in] link   The link to observe
         * @param[in] root   the root of the scene
         * @param[in] green  the green light
         * @param[in] yellow the yellow light
         * @param[in] red    the red light
         */
        Command_TLSChange(const MSLink* const link, osg::Switch* switchNode);

        /// @brief Destructor
        virtual ~Command_TLSChange();

        /** @brief Executes the command
         *
         * Called when an active tls program switches.
         *  If the state of the observed linkchanged, this method removes
         *  the old traffic light and adds a new one.
         */
        void execute();

    private:
        /// @brief The link to observe
        const MSLink* const myLink;

        /// @brief The switch for the traffic light models
        osg::ref_ptr<osg::Switch> mySwitch;

        /// @brief The previous link state
        LinkState myLastState;

    private:
        /// @brief Invalidated copy constructor.
        Command_TLSChange(const Command_TLSChange&) = delete;

        /// @brief Invalidated assignment operator.
        Command_TLSChange& operator=(const Command_TLSChange&) = delete;
    };

    /// @brief struct for OSG movable elements
    struct OSGMovable {
        osg::ref_ptr<osg::PositionAttitudeTransform> pos;
        osg::ref_ptr<osg::ShapeDrawable> geom;
        osg::ref_ptr<osg::Material> mat;
        osg::ref_ptr<osg::Switch> lights;
        bool active;
    };

    /// @brief constructor
    GUIOSGView(FXComposite* p, GUIMainWindow& app,
               GUISUMOViewParent* parent, GUINet& net, FXGLVisual* glVis,
               FXGLCanvas* share);

    /// @brief destructor
    virtual ~GUIOSGView();

    /// @brief Returns the cursor's x/y position within the network
    Position getPositionInformation() const;

    /// @brief recalculate boundaries
    void recalculateBoundaries();

    /// @brief confirm 3D view to viewport editor
    bool is3DView() const;

    /// @brief builds the view toolbars
    virtual void buildViewToolBars(GUIGlChildWindow*);

    /// @brief recenters the view
    void recenterView();

    /** @brief centers to the chosen artifact
     * @param[in] id The id of the artifact to center to
     * @param[in] applyZoom Whether to zoom in
     * @param[in] zoomDist The distance in m to use for the zoom, values < 0 means: use the centeringBoundary
     * @note caller is responsible for calling update
     */
    //void centerTo(GUIGlID id, bool applyZoom, double zoomDist = 20);

    /// @brief update the viewport chooser with the current view values
    void updateViewportValues();

    /// @brief show viewport editor
    void showViewportEditor();

    /// @brief applies the given viewport settings
    void setViewportFromToRot(const Position& lookFrom, const Position& lookAt, double rotation);

    /// @brief copy the viewport to the given view
    void copyViewportTo(GUISUMOAbstractView* view);

    /** @brief Starts vehicle tracking
     * @param[in] id The glID of the vehicle to track
     */
    void startTrack(int id);

    /** @brief Stops vehicle tracking
     */
    void stopTrack();

    /** @brief Returns the id of the tracked vehicle (-1 if none)
     * @return The glID of the vehicle to track
     */
    GUIGlID getTrackedID() const;

    bool setColorScheme(const std::string& name);

    /// @brief handle mouse click in gaming mode
    void onGamingClick(Position pos);

    /// @brief get the current simulation time
    SUMOTime getCurrentTimeStep() const;

    void removeVeh(MSVehicle* veh);
    void removeTransportable(MSTransportable* t);

    /// @brief added some callback to OSG to resize
    void position(int x, int y, int w, int h);
    void resize(int w, int h);

    // callback
    long onConfigure(FXObject*, FXSelector, void*);
    long onKeyPress(FXObject*, FXSelector, void*);
    long onKeyRelease(FXObject*, FXSelector, void*);
    long onLeftBtnPress(FXObject*, FXSelector, void*);
    long onLeftBtnRelease(FXObject*, FXSelector, void*);
    long onMiddleBtnPress(FXObject*, FXSelector, void*);
    long onMiddleBtnRelease(FXObject*, FXSelector, void*);
    long onRightBtnPress(FXObject*, FXSelector, void*);
    long onRightBtnRelease(FXObject*, FXSelector, void*);
    //long onMotion(FXObject*, FXSelector, void*);
    long onMouseMove(FXObject*, FXSelector, void*);
    long onPaint(FXObject*, FXSelector, void*);
    long OnIdle(FXObject* sender, FXSelector sel, void* ptr);

    /// @brief interaction with the simulation
    long onCmdCloseLane(FXObject*, FXSelector, void*);
    long onCmdCloseEdge(FXObject*, FXSelector, void*);
    long onCmdAddRerouter(FXObject*, FXSelector, void*);

    /// @brief highlight edges according to reachability
    long onCmdShowReachability(FXObject*, FXSelector, void*);

    /// @brief reset graphical settings when forced to refresh the view (triggered by ViewSettings)
    long  onVisualizationChange(FXObject*, FXSelector, void*);

    // @brief get the new camera position given a zoom value
    void zoom2Pos(Position& camera, Position& lookAt, double zoom);

    // @brief convert RGBColor 0..255 RGBA values to osg::Vec4 0..1 vector
    static osg::Vec4d toOSGColorVector(RGBColor c, bool useAlpha = false);

    // @brief Overwrite the HUD text
    void updateHUDText(const std::string text);

protected:
    /// @brief Store the normalized OSG window cursor coordinates
    void setWindowCursorPosition(float x, float y);

    void updatePositionInformation() const;

    /// @brief Compute the world coordinate on the ground plane given the normalized cursor position inside the OSG view (range X, Y [-1;1])
    bool getPositionAtCursor(float xNorm, float yNorm, Position& pos) const;

    /// @brief returns the GUIGlObject under the cursor using OSG ray intersecting
    std::vector<GUIGlObject*> getGUIGlObjectsUnderCursor();

    /* @brief Find GUILane which intersects with a ray from the camera to the stored cursor position
     * @return The first found GUILane found or nullptr
     */
    GUILane* getLaneUnderCursor();

    /// @brief implement the current view settings in OSG
    void adoptViewSettings();

private:
    double calculateRotation(const osg::Vec3d& lookFrom, const osg::Vec3d& lookAt, const osg::Vec3d& up);

    /// @brief inform HUD about the current window size to let it reposition
    void updateHUDPosition(int width, int height);

    class FXOSGAdapter : public osgViewer::GraphicsWindow {
    public:
        FXOSGAdapter(GUISUMOAbstractView* parent, FXCursor* cursor);
        void grabFocus();
        void grabFocusIfPointerInWindow() {}
        void useCursor(bool cursorOn);

        bool makeCurrentImplementation();
        bool releaseContext();
        void swapBuffersImplementation();

        // not implemented yet...just use dummy implementation to get working.
        bool valid() const {
            return true;
        }
        bool realizeImplementation() {
            return true;
        }
        bool isRealizedImplementation() const  {
            return true;
        }
        void closeImplementation() {}
        bool releaseContextImplementation() {
            return true;
        }
        void requestWarpPointer(float x, float y) {
            int xRound = std::lround(x);
            int yRound = std::lround(y);
            int xPrev, yPrev;
            unsigned int buttons;
            myParent->getCursorPosition(xPrev, yPrev, buttons);
            if (xRound - xPrev != 0 || yRound - yPrev != 0) {
                myParent->setCursorPosition(xRound, yRound);
                getEventQueue()->mouseWarped(x, y);
            }
        }

    protected:
        ~FXOSGAdapter();
    private:
        GUISUMOAbstractView* const myParent;
        FXCursor* const myOldCursor;
    };

    class PlaneMoverCallback : public osg::Callback {
    public:
        PlaneMoverCallback(osg::Camera* camera) : myCamera(camera) {};
        virtual bool run(osg::Object* object, osg::Object* /* data */) override {
            osg::MatrixTransform* mt = dynamic_cast<osg::MatrixTransform*>(object);
            osg::Vec3d lookFrom, lookAt, up;
            myCamera->getViewMatrixAsLookAt(lookFrom, lookAt, up);
            osg::Vec3d direction = lookAt - lookFrom;
            direction.normalize();
            osg::Vec3d lookAtGround = lookFrom - direction * (lookFrom.z() / direction.z());
            osg::Matrixd translateMatrix;
            translateMatrix.makeTranslate(lookAtGround.x(), lookAtGround.y(), 0.);
            double angle = atan2(direction.y(), direction.x());
            osg::Matrixd rotMatrix = osg::Matrixd::rotate(angle, osg::Z_AXIS);
            mt->setMatrix(rotMatrix * translateMatrix);
            return true;
        }
    protected:
        ~PlaneMoverCallback() {};
    private:
        osg::Camera* myCamera;
    };

    class PickHandler : public osgGA::GUIEventHandler {
    public:
        PickHandler(GUIOSGView* parent) : myParent(parent), myDrag(false) {};
        bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
    protected:
        ~PickHandler() {};
    private:
        GUIOSGView* const myParent;
        bool myDrag;
    };

    class ExcludeFromNearFarComputationCallback : public osg::NodeCallback {
        virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {
            osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
            // Default value
            osg::CullSettings::ComputeNearFarMode oldMode = osg::CullSettings::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES;
            if (cv) {
                oldMode = cv->getComputeNearFarMode();
                cv->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
            }
            traverse(node, nv);
            if (cv) {
                cv->setComputeNearFarMode(oldMode);
            }
        }
    };



protected:
    GUIOSGView() {}

    osg::ref_ptr<FXOSGAdapter> myAdapter;
    osg::ref_ptr<osgViewer::Viewer> myViewer;
    osg::ref_ptr<osg::Group> myRoot;
    osg::ref_ptr<osg::MatrixTransform> myPlane;
    osg::ref_ptr<osg::Camera> myHUD;
    osg::ref_ptr<osg::Geode> myTextNode;
    osg::ref_ptr<osgText::Text> myText;

private:
    GUIVehicle* myTracked;
    osg::ref_ptr<GUIOSGManipulator> myCameraManipulator;
    SUMOTime myLastUpdate;

    float myOSGNormalizedCursorX, myOSGNormalizedCursorY;

    std::map<MSVehicle*, OSGMovable > myVehicles;
    std::map<MSTransportable*, OSGMovable > myPersons;

    osg::ref_ptr<osg::Node> myGreenLight;
    osg::ref_ptr<osg::Node> myYellowLight;
    osg::ref_ptr<osg::Node> myRedLight;
    osg::ref_ptr<osg::Node> myRedYellowLight;
    osg::ref_ptr<osg::Node> myPoleBase;
    osg::ref_ptr<osg::Node> myPlaneTransform;
};

#endif
