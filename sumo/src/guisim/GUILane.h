#ifndef GUILane_h
#define GUILane_h

#include <string>
#include <utility>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <utils/geom/Position2D.h>
#include <utils/qutils/NewQMutex.h>

class MSVehicle;

class GUILane : public MSLane {
private:
//    std::pair<Position2D, Position2D> _position;
    Position2D _begin;
    Position2D _end;
    Position2D _direction;
    double _rotation;
    NewQMutex _lock;
public:
    GUILane( std::string id, double maxSpeed, double length, MSEdge* egde );
    ~GUILane();
    void setPosition(double x1, double y1, double x2, double y2);
//    const std::pair<Position2D, Position2D> &getPos() const;
    const MSLane::VehCont &getVehiclesLocked();
    void unlockVehicles();
    double getLength() const;
    const Position2D &getBegin() const;
    const Position2D &getEnd() const;
    const Position2D &getDirection() const;
    double getRotation() const;
    void moveExceptFirst();
    void moveExceptFirst( 
        MSEdge::LaneCont::const_iterator firstNeighLane,
        MSEdge::LaneCont::const_iterator lastNeighLane );
    /// Emit vehicle with speed 0 into lane if possible.
    bool emit( MSVehicle& newVeh );

    /// Try to emit a vehicle with speed > 0, i.e. from a source with
    /// initial speed values.
    bool isEmissionSuccess( MSVehicle* aVehicle );

    /** Move first vehicle according to the previously calculated
        next speed if respond is true. This may imply that the first vehicle
        leaves this lane. If repond is false, decelerate towards the lane's
        end. Should only be called, if request was set. */
    void moveFirst( bool respond );

};


#endif
