#ifndef GUIEdge_h
#define GUIEdge_h

#include <vector>
#include <string>
#include <utils/geom/Position2D.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>

class MSJunction;
class GUILane;

class GUIEdge : public MSEdge {
public:
    enum EdgeBasicFunction {
        EDGEFUNCTION_UNKNOWN = -1,
        EDGEFUNCTION_NORMAL = 0,
        EDGEFUNCTION_SOURCE = 1,
        EDGEFUNCTION_SINK = 2
    };
public:
    GUIEdge(std::string id);
    ~GUIEdge();
    void initJunctions(MSJunction *from, MSJunction *to, 
        EdgeBasicFunction function);
    static std::vector<std::string> getNames();
	double toXPos() const;
	double fromXPos() const;
	double toYPos() const;
	double fromYPos() const;
    std::string getID() const;
    EdgeBasicFunction getPurpose() const;
    GUILane &getLane(size_t laneNo);
    //void moveExceptFirstSingle();

private:
    std::pair<double, double> getLaneOffsets(double x1, double y1, 
        double x2, double y2, double prev, double wanted) const;

    MSJunction *_from, *_to;
    EdgeBasicFunction _function;
};

#endif
