#include <vector>
#include "GUISUMOAbstractView.h"
#include <guisim/GUIJunctionWrapper.h>
#include <utils/glutils/GLHelper.h>
#include "GUISimpleJunctionDrawer.h"

GUISimpleJunctionDrawer::GUISimpleJunctionDrawer(
        std::vector<GUIJunctionWrapper*> &junctions)
    : GUIJunctionDrawer(junctions)
{
}


GUISimpleJunctionDrawer::~GUISimpleJunctionDrawer()
{
}


void
GUISimpleJunctionDrawer::drawGLJunctions(size_t *which, size_t maxJunctions,
                                         bool showToolTips,
                                         GUISUMOAbstractView::JunctionColoringScheme )
{
    glLineWidth(1);
    glColor3f(0, 0, 0);
    // check whether tool-tip information shall be generated
    if(showToolTips) {
        // go through edges
        for(size_t i=0; i<maxJunctions; i++ ) {
            if(which[i]==0) {
                continue;
            }
            size_t pos = 1;
            for(size_t j=0; j<32; j++, pos<<=1) {
                if((which[i]&pos)!=0) {
                    GLHelper::drawFilledPoly(
                        myJunctions[j+(i<<5)]->getShape(), true);
                }
            }
        }
    } else {
        // go through edges
        for(size_t i=0; i<maxJunctions; i++ ) {
            if(which[i]==0) {
                continue;
            }
            size_t pos = 1;
            for(size_t j=0; j<32; j++, pos<<=1) {
                if((which[i]&pos)!=0) {
                    GLHelper::drawFilledPoly(
                        myJunctions[j+(i<<5)]->getShape(), true);
                }
            }
        }
    }
}

