/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <algorithm>
#include <utils/geom/Position2DVector.h>
#include <utils/options/OptionsSubSys.h>
#include <utils/options/OptionsCont.h>
#include <utils/geom/GeomHelper.h>
#include <utils/common/StdDefs.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/ToString.h>
#include "NBNode.h"
#include "NBNodeShapeComputer.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG

using namespace std;

NBNodeShapeComputer::NBNodeShapeComputer(const NBNode &node,
                                         std::ofstream * const out)
    : myNode(node), myOut(out)
{
}

NBNodeShapeComputer::~NBNodeShapeComputer()
{
}


Position2DVector
NBNodeShapeComputer::compute()
{
    Position2DVector ret;
    // compute the shape of the junction
    //  only junction have a shape, otherwise it's somekind
    //  of another connection between streets
/*    if( (_incomingEdges->size()<1||_outgoingEdges->size()<1)
        ||
        (_incomingEdges->size()<2&&_outgoingEdges->size()<2) ) {
        return;
    }*/
    // check whether this is a real junction between different edges
    //  or only an edge's split/join
/*    bool isRealJunction = false;
    for(EdgeVector::const_iterator i=myNode._allEdges.begin(); i!=myNode._allEdges.end()&&!isRealJunction; i++) {
        Line2D incLine;
        if(myNode.hasIncoming(*i)) {
            const Position2DVector &geom = (*i)->getGeometry();
            incLine = Line2D(geom.at(geom.size()-2), geom.at(geom.size()-1));
        } else {
            const Position2DVector &geom = (*i)->getGeometry();
            incLine = Line2D(geom.at(1), geom.at(0));
        }
        NBEdge *e1 = (*i);
        SUMOReal angle1 = incLine.atan2DegreeAngle();
        for(EdgeVector::const_iterator i2=i+1; i2!=myNode._allEdges.end()&&!isRealJunction; i2++) {
            Line2D outLine;
            NBEdge *e2 = (*i2);
            if(myNode.hasIncoming(*i2)) {
                const Position2DVector &geom = (*i2)->getGeometry();
                outLine = Line2D(geom.at(geom.size()-2), geom.at(geom.size()-1));
            } else {
                const Position2DVector &geom = (*i2)->getGeometry();
                outLine = Line2D(geom.at(1), geom.at(0));
            }
            SUMOReal angle2 = outLine.atan2DegreeAngle();
            //
            if(abs(angle1-angle2)>35&&abs(angle1-angle2)<145) {
                isRealJunction = true;
            }
        }
    }*/
    /*
    int bla;
    {
        const EdgeVector &ev = myNode.getIncomingEdges();
        for(int i=0; i<ev.size(); ++i) {
            if(ev[i]->getID()=="15039938") {
                bla = 0;
            }
        }
    }
    */

    if(isSimpleContinuation(myNode)) {
        ret = computeContinuationNodeShape();
    } else {
        ret = computeRealNodeShape();
    }
    // add the geometry of internal lanes
    if(OptionsSubSys::getOptions().getBool("add-internal-links")) {
        addInternalGeometry();
    }
    if(ret.size()>3) {
        try {
            ret = ret.convexHull();
            if(ret.size()>0) {
                ret.closePolygon();
            }
        } catch (ProcessError&) {
        }
    }
    if(ret.size()<4) {
        ret = computeNodeShapeByCrosses();
        /*
        SUMOReal maxWidth = myNode.getMaxEdgeWidth();
        Position2D p = myNode.getPosition();
        //
        Position2D p1 = p;
        p1.add(maxWidth, maxWidth);
        ret.push_back(p1);
        //
        p1 = p;
        p1.add(maxWidth, -maxWidth);
        ret.push_back(p1);
        //
        p1 = p;
        p1.add(-maxWidth, -maxWidth);
        ret.push_back(p1);
        //
        p1 = p;
        p1.add(-maxWidth, maxWidth);
        ret.push_back(p1);

        ret.closePolygon();
        */
    }
    {
        if(myOut!=0) {
            for(int i=0; i<(int) ret.size(); ++i) {
                (*myOut) << "   <poi id=\"end_" << myNode.getID() << "_"
                    << toString(i) << "\" type=\"nodeshape.end\" color=\"1,0,1\""
                    << " x=\"" << ret.at(i).x() << "\" y=\"" << ret.at(i).y() << "\"/>"
                    << endl;
            }
        }
    }
    return ret;
}


void
NBNodeShapeComputer::addInternalGeometry()
{
    /*
    for(EdgeVector::const_iterator i=myNode._incomingEdges->begin(); i!=myNode._incomingEdges->end(); i++) {
        size_t noLanesEdge = (*i)->getNoLanes();
        for(size_t j=0; j<noLanesEdge; j++) {
            const EdgeLaneVector *elv = (*i)->getEdgeLanesFromLane(j);
            for(EdgeLaneVector::const_iterator k=elv->begin(); k!=elv->end(); k++) {
                if((*k).edge==0) {
                    continue;
                }
                Position2DVector shape =
                    computeInternalLaneShape(*i, j, (*k).edge, (*k).lane);
                if(shape.length()==0) {
                    continue;
                }
                Position2DVector l(shape);
                l.move2side(1.5);
                Position2DVector r(shape);
                l.move2side(-1.5);
                myPoly.push_back(shape);
            }
        }
    }
    */
}


Position2DVector
NBNodeShapeComputer::computeContinuationNodeShape()
{
    EdgeVector::const_iterator i;
    std::map<NBEdge*, SUMOReal> myIncPos;
    std::map<NBEdge*, SUMOReal> myOutPos;
    for(i=myNode._allEdges.begin(); i!=myNode._allEdges.end(); i++) {
        if(myNode.hasIncoming(*i)) {
            Position2DVector own_bound =
                (*i)->getCCWBoundaryLine(myNode, 1.5);
            EdgeVector::const_iterator ri = i;
            NBContHelper::nextCCW(&(myNode._allEdges), ri);
            Position2DVector opp_bound =
                (*ri)->getCWBoundaryLine(myNode, 1.5);
            myIncPos[*i] = myNode.getOffset(own_bound, opp_bound);
        } else {
            Position2DVector own_bound =
                (*i)->getCWBoundaryLine(myNode, 1.5);
            EdgeVector::const_iterator li = i;
            NBContHelper::nextCW(&(myNode._allEdges), li);
            Position2DVector opp_bound =
                (*li)->getCCWBoundaryLine(myNode, 1.5);
            myOutPos[*i] = myNode.getOffset(own_bound, opp_bound);
        }
    }
    //
    Position2DVector ret;
    if(myIncPos.size()==2) {
        for(std::map<NBEdge*, SUMOReal>::iterator j=myIncPos.begin(); j!=myIncPos.end(); j++) {
            NBEdge *inc = (*j).first;
            SUMOReal v1 = (*j).second;
            NBEdge *out =
                *(find_if(
                    myNode.getOutgoingEdges().begin(),
                    myNode.getOutgoingEdges().end(),
                    NBContHelper::opposite_finder(inc, &myNode)));
            SUMOReal v2 = myOutPos[out];
            addCCWPoint(ret, inc, MAX2(v1, v2), 1.5);
            addCWPoint(ret, out, MAX2(v1, v2), 1.5);
        }
        if(myOut!=0) {
            for(int i=0; i<(int) ret.size(); ++i) {
                (*myOut) << "   <poi id=\"cont1_" << myNode.getID() << "_" <<
                    toString<int>(i) << "\" type=\"nodeshape.cont1\" color=\"1,0,0\""
                    << " x=\"" << ret.at(i).x() << "\" y=\"" << ret.at(i).y() << "\"/>"
                    << endl;
            }
        }
    } else {
        if(myIncPos.size()>0) {
            addCCWPoint(ret,
                myIncPos.begin()->first, myIncPos.begin()->second, 1.5);
            addCWPoint(ret,
                myIncPos.begin()->first, myIncPos.begin()->second, 1.5);
        }
        if(myOutPos.size()>0) {
            addCCWPoint(ret,
                myOutPos.begin()->first, myOutPos.begin()->second, 1.5);
            addCWPoint(ret,
                myOutPos.begin()->first, myOutPos.begin()->second, 1.5);
        }
        if(myOut!=0) {
            for(int i=0; i<(int) ret.size(); ++i) {
                (*myOut) << "   <poi id=\"cont2_" << myNode.getID() << "_" <<
                    toString(i) << "\" type=\"nodeshape.cont2\" color=\"0.5,0,0\""
                    << " x=\"" << ret.at(i).x() << "\" y=\"" << ret.at(i).y() << "\"/>"
                    << endl;
            }
        }
    }
    return ret;
}


Position2DVector
NBNodeShapeComputer::computeRealNodeShape()
{
    Position2DVector ret;
//    std::vector<SUMOReal> edgeOffsets;
    EdgeCrossDefVector edgeOffsets;
    EdgeVector::const_iterator i;
    for(i=myNode._allEdges.begin(); i!=myNode._allEdges.end(); i++) {
        edgeOffsets.push_back(getEdgeNeighborCrossings(i));
    }
    EdgeCrossDefVector::iterator j;
    for(i=myNode._allEdges.begin(), j=edgeOffsets.begin(); i!=myNode._allEdges.end(); i++, j++) {
        NeighborCrossDesc used = getNeighbor2Use(j);
        // do not process outgoing which have opposite incoming for themselves
        EdgeVector::const_iterator li = i;
        NBContHelper::nextCW(&(myNode._allEdges), li);
        NBEdge *current = *i;
        if(myNode.hasIncoming(current)&&current->isTurningDirectionAt(&myNode, *li)) {
            EdgeCrossDefVector::iterator j2 = j+1;
            if(j2==edgeOffsets.end()) {
                j2 = edgeOffsets.begin();
            }
            NeighborCrossDesc used2 = getNeighbor2Use(j2);
            used.myCrossingPosition =
                MAX2(used.myCrossingPosition, used2.myCrossingPosition);
            // ok, process both directions
            addCCWPoint(ret, current, used.myCrossingPosition, 1.5);
            addCWPoint(ret, (*li), used.myCrossingPosition, 1.5);
        } else if(myNode.hasOutgoing(current)) {
            EdgeVector::const_iterator ri = i;
            NBContHelper::nextCCW(&(myNode._allEdges), ri);
            // skip outgoing that have an incoming
            if((*ri)->isTurningDirectionAt(&myNode, current)) {
                continue;
            }
            addCCWPoint(ret, current, used.myCrossingPosition, 1.5);
            addCWPoint(ret, current, used.myCrossingPosition, 1.5);
        } else {
            // process this edge only
            addCCWPoint(ret, current, used.myCrossingPosition, 1.5);
            addCWPoint(ret, current, used.myCrossingPosition, 1.5);
        }
    }
    {
        if(myOut!=0) {
            for(int i=0; i<(int) ret.size(); ++i) {
                (*myOut) << "   <poi id=\"real1_" << myNode.getID() << "_" <<
                    toString(i) << "\" type=\"nodeshape.real1\" color=\"0,1,0\""
                    << " x=\"" << ret.at(i).x() << "\" y=\"" << ret.at(i).y() << "\"/>"
                    << endl;
            }
        }
    }
    return ret;
}

Position2DVector
rotateAround(const Position2DVector &what, const Position2D &at, SUMOReal rot)
{
    Position2DVector rret;
    Position2DVector ret = what;
    ret.resetBy(-at.x(), -at.y());
    {
    SUMOReal x = ret.at(0).x() * cos(rot) + ret.at(0).y() * sin(rot);
    SUMOReal y = ret.at(0).y() * cos(rot) - ret.at(0).x() * sin(rot);
    rret.push_back(Position2D(x, y));
    }
    {
    SUMOReal x = ret.at(1).x() * cos(rot) + ret.at(1).y() * sin(rot);
    SUMOReal y = ret.at(1).y() * cos(rot) - ret.at(1).x() * sin(rot);
    rret.push_back(Position2D(x, y));
    }
    rret.resetBy(at.x(), at.y());
    return rret;
}

Position2DVector
NBNodeShapeComputer::computeNodeShapeByCrosses()
{
    Position2DVector ret;
    EdgeVector::const_iterator i;
    for(i=myNode._allEdges.begin(); i!=myNode._allEdges.end(); i++) {
        // compute crossing with normal
        {
            Position2DVector edgebound = (*i)->getCCWBoundaryLine(myNode, 1.5);
            Position2DVector cross;
            cross.push_back(edgebound.at(0));
            cross.push_back(edgebound.at(1));
            cross = rotateAround(cross, myNode.getPosition(), (SUMOReal) (90/180.*3.1415926535897932384626433832795));
            if(cross.intersects(edgebound)) {
                ret.push_back(cross.intersectsAtPoint(edgebound));
            } else {
                Position2DVector cross2 = cross;
                cross2.extrapolate(500);
                edgebound.extrapolate(500);
                if(cross2.intersects(edgebound)) {
                   ret.push_back(cross2.intersectsAtPoint(edgebound));
                }
            }
        }
        {
            Position2DVector edgebound = (*i)->getCWBoundaryLine(myNode, 1.5);
            Position2DVector cross;
            cross.push_back(edgebound.at(0));
            cross.push_back(edgebound.at(1));
            cross = rotateAround(cross, myNode.getPosition(), (SUMOReal) (90/180.*3.1415926535897932384626433832795));
            if(cross.intersects(edgebound)) {
                ret.push_back(cross.intersectsAtPoint(edgebound));
            } else {
                Position2DVector cross2 = cross;
                cross2.extrapolate(500);
                edgebound.extrapolate(500);
                if(cross2.intersects(edgebound)) {
                   ret.push_back(cross2.intersectsAtPoint(edgebound));
                }
            }
        }
    }
    {
        if(myOut!=0) {
            for(int i=0; i<(int) ret.size(); ++i) {
                (*myOut) << "   <poi id=\"cross1_" << myNode.getID() << "_" <<
                    toString(i) << "\" type=\"nodeshape.cross1\" color=\"0,0,1\""
                    << " x=\"" << ret.at(i).x() << "\" y=\"" << ret.at(i).y() << "\"/>"
                    << endl;
            }
        }
    }
    return ret;
}


NBNodeShapeComputer::NeighborCrossDesc
NBNodeShapeComputer::getNeighbor2Use(const EdgeCrossDefVector::iterator &j)
{
    NeighborCrossDesc used;
    if((*j).first.myAmValid==false&&(*j).second.myAmValid==false) {
        used.myCrossingPosition = 4;
    } else {
        if((*j).first.myCrossingPosition>(*j).second.myCrossingPosition) {
            used = (*j).first;
        } else {
            used = (*j).second;
        }
    }
    return used;
}


NBNodeShapeComputer::EdgeCrossDef
NBNodeShapeComputer::getEdgeNeighborCrossings(
        const EdgeVector::const_iterator &i)
{
    NBEdge *current = *i;
    // the clockwise and the counter clockwise border;
    Position2DVector cb, ccb;
    EdgeVector::const_iterator mri = i;
    EdgeVector::const_iterator mli = i;
    // the left and the right crossing line
    // the crossing edges
    EdgeVector::const_iterator ri = i;
    EdgeVector::const_iterator li = i;
    // clockwise border
    cb = current->getCWBoundaryLine(myNode, 2.5);
    // counterclockwise border
    ccb = current->getCCWBoundaryLine(myNode, 2.5);
    Position2DVector cl, ccl;
    if(myNode.hasIncoming(current)) {
        // is an incoming edge
        //  the counter clockwise edge is surely not the opposite direction
        NBContHelper::nextCCW(&(myNode._allEdges), ri);
        //  check the clockwise edge
        NBContHelper::nextCW(&(myNode._allEdges), li);
        while(current->isTurningDirectionAt(&myNode, *li)/*||(current->isAlmostSameDirectionAt(&myNode, *li)&&myNode._allEdges.size()>2)*/) {
            mli = li;
            cb = (*li)->getCWBoundaryLine(myNode, 2.5);
            NBContHelper::nextCW(&(myNode._allEdges), li);
        }
        ccl = (*ri)->getCWBoundaryLine(myNode, 2.5);
        cl = (*li)->getCCWBoundaryLine(myNode, 2.5);
    } else {
        NBContHelper::nextCCW(&(myNode._allEdges), ri);
        while((*ri)->isTurningDirectionAt(&myNode, current)/*||((*ri)->isAlmostSameDirectionAt(&myNode, current)&&myNode._allEdges.size()>2)*/) {
            mri = ri;
            ccb = (*ri)->getCCWBoundaryLine(myNode, 2.5);
            NBContHelper::nextCCW(&(myNode._allEdges), ri);
        }
        NBContHelper::nextCW(&(myNode._allEdges), li);
        ccl = (*ri)->getCWBoundaryLine(myNode, 2.5);
        cl = (*li)->getCCWBoundaryLine(myNode, 2.5);
    }
    return EdgeCrossDef(
        buildCrossingDescription(mri, ri, ccb, ccl),
        buildCrossingDescription(mli, li, cb, cl));
}


NBNodeShapeComputer::NeighborCrossDesc
NBNodeShapeComputer::buildCrossingDescription(const EdgeVector::const_iterator &i,
                                              const EdgeVector::const_iterator &oi,
                                              const Position2DVector &own_bound,
                                              const Position2DVector &opp_bound)
                                              const
{
    NeighborCrossDesc ret;
    ret.myAmValid = true;
    if(oi==i) {
        ret.myCrossingPosition = 4;
        ret.myCrossingAngle = 360;
    } else {
        ret.myCrossingPosition = myNode.getOffset(own_bound, opp_bound);
        SUMOReal a1 = own_bound.lineAt(0).atan2DegreeAngle();
        SUMOReal a2 = opp_bound.lineAt(0).atan2DegreeAngle();
        ret.myCrossingAngle = myNode.getCCWAngleDiff(a1, a2);
        if(ret.myCrossingAngle>180) {
            ret.myCrossingAngle = (SUMOReal) 360.0 - ret.myCrossingAngle;
        }
        if(ret.myCrossingAngle>150||ret.myCrossingAngle<20) {
            ret.myAmValid = false;
        }
    }
    return ret;
}


void
NBNodeShapeComputer::addCCWPoint(Position2DVector &poly,
                                 NBEdge *e, SUMOReal offset,
                                 SUMOReal width)
{
    Position2DVector l = e->getCCWBoundaryLine(myNode, width);
    SUMOReal len = l.length();
    if(len>=offset) {
        poly.push_back(l.positionAtLengthPosition(offset));
    } else {
        poly.push_back(l.positionAtLengthPosition(len));
    }
}

void
NBNodeShapeComputer::addCWPoint(Position2DVector &poly,
                                NBEdge *e, SUMOReal offset,
                                SUMOReal width)
{
    Position2DVector l = e->getCWBoundaryLine(myNode, width);
    SUMOReal len = l.length();
    if(len>=offset) {
        poly.push_back(l.positionAtLengthPosition(offset));
    } else {
        poly.push_back(l.positionAtLengthPosition(len));
    }
}

Position2DVector
NBNodeShapeComputer::computeJoinSplitNodeShape()
{
    Position2DVector ret;
    std::vector<SUMOReal> edgeOffsets;
    EdgeVector::const_iterator i;
    for(i=myNode._allEdges.begin(); i!=myNode._allEdges.end(); i++) {
        Position2DVector o1 = (*i)->getCCWBoundaryLine(myNode, 1.5);
        Position2DVector o2 = (*i)->getCWBoundaryLine(myNode, 1.5);
        Position2DVector geom = (*i)->getGeometry();
        geom.extrapolate(1000);
        // build normal
        Line2D incLine;
        if(myNode.hasIncoming(*i)) {
            incLine = Line2D(geom.at(geom.size()-2), geom.at(geom.size()-1));
        } else {
            incLine = Line2D(geom.at(1), geom.at(0));
        }
        SUMOReal xcenter = (incLine.p1().x() + incLine.p2().x()) / (SUMOReal) 2.0;
        SUMOReal ycenter = (incLine.p1().y() + incLine.p2().y()) / (SUMOReal) 2.0;
        incLine.sub(xcenter, ycenter);
        Line2D extrapolated(
            GeomHelper::extrapolate_first(incLine.p1(), incLine.p2(), 1000),
            GeomHelper::extrapolate_second(incLine.p1(), incLine.p2(), 1000));
        Line2D normal(
            Position2D(extrapolated.p1().y(), extrapolated.p1().x()),
            Position2D(extrapolated.p2().y(), extrapolated.p2().x()));
        normal.add(myNode.getPosition());
        // get cross position
        SUMOReal pos;
        if(myNode.hasIncoming(*i)) {
            pos = 0;//!!!(*i)->getGeometry().length();
        } else {
            pos = 0;
        }
        if(!geom.intersects(normal.p1(), normal.p2())) {
//            continue;//!!! muss immer gelten, tut's nicht
            ret.push_back(geom.positionAtLengthPosition(pos));
        }

        if(o1.intersects(normal.p1(), normal.p2())) {
            ret.push_back(o1.intersectsAtPoint(normal.p1(), normal.p2()));
        } else {
            int bla = 0;
        }
        if(o2.intersects(normal.p1(), normal.p2())) {
            ret.push_back(o2.intersectsAtPoint(normal.p1(), normal.p2()));
        } else {
            int bla = 0;
        }
//        if(geom.intersects(normal.p1(), normal.p2())) {
        if(!o1.intersects(normal.p1(), normal.p2())
            &&
            !o2.intersects(normal.p1(), normal.p2())
            &&
            geom.intersects(normal.p1(), normal.p2())
            ) {
            assert(geom.intersects(normal.p1(), normal.p2()));
            DoubleVector posses = geom.intersectsAtLengths(normal);
            if(posses.size()==0) {
                int bla = 0;
                posses = geom.intersectsAtLengths(normal);
            }
            if(myNode.hasIncoming(*i)) {
                pos = DoubleVectorHelper::minValue(posses);
            } else {
                pos = DoubleVectorHelper::maxValue(posses);
            }
            // add geometry point
            ret.push_back(
                (*i)->getCCWBoundaryLine(myNode, 1.5).positionAtLengthPosition(pos-1000));
            ret.push_back(
                (*i)->getCWBoundaryLine(myNode, 1.5).positionAtLengthPosition(pos-1000));
        }
//        myPoly.push_back(geom.positionAtLengthPosition(pos));
    }
    return ret;
}


bool
NBNodeShapeComputer::isSimpleContinuation(const NBNode &n) const
{
    // one in, one out->continuation
    const EdgeVector incoming = n.getIncomingEdges();
    const EdgeVector outgoing = n.getOutgoingEdges();
    if(incoming.size()==1&&outgoing.size()==1) {
        // both must have the same number of lanes
        return
            (*(incoming.begin()))->getNoLanes()
            ==
            (*(outgoing.begin()))->getNoLanes();
    }
    // two in and two out and both in reverse direction
    if(incoming.size()==2&&outgoing.size()==2) {
        for(EdgeVector::const_iterator i=incoming.begin(); i!=incoming.end(); i++) {
            NBEdge *in = *i;
            EdgeVector::const_iterator opposite =
                find_if(outgoing.begin(), outgoing.end(),
                NBContHelper::opposite_finder(in, &myNode));
            // must have an opposite edge
            if(opposite==outgoing.end()) {
                return false;
            }
            // both must have the same number of lanes
            NBContHelper::nextCW(&outgoing, opposite);
            if(in->getNoLanes()!=(*opposite)->getNoLanes()) {
                return false;
            }
        }
        return true;
    }
    // nope
    return false;
}


//SUMOReal getOffset(Position2DVector on, Position2DVector cross) const;
