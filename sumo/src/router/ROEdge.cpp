#ifdef MSVC
#include <limits>
#else
#define DBL_MAX 10000000000.0 // !!!
#endif
#include <algorithm>
#include "ROLane.h"
#include "ROEdge.h"

using namespace std;

// !!! wie wärs mit einer effort-Tabelle oder Funktion über die Zeit?

ROEdge::ROEdge(const std::string &id)
    : _id(id), _effort(0)
{
}


ROEdge::~ROEdge()
{
    for(std::vector<ROLane*>::iterator i=_lanes.begin(); i!=_lanes.end(); i++) {
        delete *i;
    }
}


void ROEdge::setEffort(double effort)
{
    _effort = effort;
}


void ROEdge::addLane(ROLane *lane)
{
    _lanes.push_back(lane);
    double length = lane->getLength();
    _effort = length > _effort ? length : _effort;
}
/*
void
ROEdge::computeWeight() {
    _effort = FLT_MAX;
    for(std::vector<ROLane*>::iterator i=_lanes.begin(); i!=_lanes.end(); i++) {
        double tmpWeight = (*i)->getWeight();
        _effort = _effort < tmpWeight ? _effort : tmpWeight;
    }
}
*/

void ROEdge::setLane(long currentTime, const std::string &scheme,
                     const std::string &id, float value)
{
}

void
ROEdge::myInit() {
}

void
ROEdge::addSucceeder(ROEdge *s) {
    _succeeding.push_back(s);
}

float
ROEdge::getMyEffort() const {
    return _effort;
}

size_t
ROEdge::getNoFollowing()
{
    return _succeeding.size();
}


ROEdge *
ROEdge::getFollower(size_t pos)
{
    return _succeeding[pos];
}

bool
ROEdge::isConnectedTo(ROEdge *e) {
    return find(_succeeding.begin(), _succeeding.end(), e)!=_succeeding.end();
}



double 
ROEdge::getCost(long time) const
{
    return _lanes[0]->getLength();
}


long 
ROEdge::getDuration(long time) const
{
    return long(_lanes[0]->getLength() / 20.9); // !!!
}


void ROEdge::init() {
    _explored = false;
    _dist = DBL_MAX;
    _prevKnot = 0;
    _inFrontList= false;
    myInit();
}

void ROEdge::initRootDistance() {
    _dist = 0;
    _inFrontList = true;
}

float ROEdge::getEffort() const {
    return _dist;
}

float ROEdge::getNextEffort() const {
    return _dist + getMyEffort();
}

void ROEdge::setEffort(float dist) {
    _dist = dist;
}

bool ROEdge::isInFrontList() const {
    return _inFrontList;
}

bool ROEdge::isExplored() const {
    return _explored;
}

void ROEdge::setExplored(bool value) {
    _explored = value;
}

ROEdge *ROEdge::getPrevKnot() const {
    return _prevKnot;
}

void ROEdge::setPrevKnot(ROEdge *prev) {
    _prevKnot = prev;
}

std::string ROEdge::getID() const {
    return _id;
}
