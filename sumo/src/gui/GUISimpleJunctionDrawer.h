#ifndef GUISimpleJunctionDrawer_h
#define GUISimpleJunctionDrawer_h

#include <vector>
#include "GUISUMOAbstractView.h"

class GUIJunctionWrapper;

class GUISimpleJunctionDrawer :
    public GUISUMOAbstractView::GUIJunctionDrawer
{
public:
    GUISimpleJunctionDrawer(std::vector<GUIJunctionWrapper*> &junctions);
    ~GUISimpleJunctionDrawer();
    void drawGLJunctions(size_t *which, size_t maxJunctions,
        bool showToolTips, GUISUMOAbstractView::JunctionColoringScheme scheme);
};

#endif
