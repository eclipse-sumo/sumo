#ifndef GUILaneChanger_h
#define GUILaneChanger_h

#include <microsim/MSEdge.h>
#include <microsim/MSLaneChanger.h>

class GUILaneChanger : public MSLaneChanger {
public:
    public:
    /// Destructor.
    ~GUILaneChanger();

    /// Constructor
    GUILaneChanger( MSEdge::LaneCont* lanes );

protected:
    /** @brief sets the lanes aftr changing was done
        This methods makes the same as the methods from MSLaneChanger but locks
        the lanes first */
    void updateLanes();
};

#endif
