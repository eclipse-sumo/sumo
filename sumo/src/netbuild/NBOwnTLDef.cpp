
#include <vector>
#include <cassert>
#include "NBTrafficLightDefinition.h"
#include "NBTrafficLightLogicVector.h"
#include "NBRequestEdgeLinkIterator.h"
#include "NBLinkCliqueContainer.h"
#include "NBNode.h"
#include "NBOwnTLDef.h"


NBOwnTLDef::NBOwnTLDef(const std::string &id,
                       const std::set<NBNode*> &junctions)
    : NBTrafficLightDefinition(id, junctions)
{
}


NBOwnTLDef::NBOwnTLDef(const std::string &id, NBNode *junction)
    : NBTrafficLightDefinition(id, junction)
{
}


NBOwnTLDef::NBOwnTLDef(const std::string &id,
                       std::string type,
                       NBNode *junction)
    : NBTrafficLightDefinition(id, type, junction)
{
}


NBOwnTLDef::NBOwnTLDef(const std::string &id)
    : NBTrafficLightDefinition(id)
{
}


NBOwnTLDef::~NBOwnTLDef()
{
}


NBTrafficLightLogicVector *
NBOwnTLDef::myCompute(size_t breakingTime, std::string type, bool buildAll)
{
    bool appendSmallestOnly = true;
    bool skipLarger = true;

    bool joinLaneLinks = false;
    bool removeTurnArounds = true;
    LinkRemovalType removal = LRT_REMOVE_WHEN_NOT_OWN;
    NBTrafficLightLogicVector *logics1 =
        computeTrafficLightLogics(getID(), type,
            joinLaneLinks, removeTurnArounds, removal,
            appendSmallestOnly, skipLarger, breakingTime);

    if(buildAll) {
        joinLaneLinks = false;
        removeTurnArounds = true;
        removal = LRT_NO_REMOVAL;
        NBTrafficLightLogicVector *logics2 =
            computeTrafficLightLogics(getID(), type,
                joinLaneLinks, removeTurnArounds, removal,
                appendSmallestOnly, skipLarger, breakingTime);

        joinLaneLinks = false;
        removeTurnArounds = true;
        removal = LRT_REMOVE_ALL_LEFT;
        NBTrafficLightLogicVector *logics3 =
            computeTrafficLightLogics(getID(), type,
                joinLaneLinks, removeTurnArounds, removal,
                appendSmallestOnly, skipLarger, breakingTime);

        // join build logics
        logics1->add(*logics2);
        logics1->add(*logics3);
        delete logics2;
        delete logics3;
    }
    return logics1;
}



NBTrafficLightLogicVector *
NBOwnTLDef::computeTrafficLightLogics(const std::string &key,
                                      std::string type,
                                      bool joinLaneLinks,
                                      bool removeTurnArounds,
                                      LinkRemovalType removal,
                                      bool appendSmallestOnly,
                                      bool skipLarger,
                                      size_t breakingTime) const
{
    // compute the matrix of possible links x links
    //  (links allowing each other the parallel execution)
    NBLinkPossibilityMatrix *v = getPossibilityMatrix(joinLaneLinks,
        removeTurnArounds, removal);
    // get the number of regarded links
    NBRequestEdgeLinkIterator cei1(this,
        joinLaneLinks, removeTurnArounds, removal);
    size_t maxStromAnz = cei1.getNoValidLinks();

#ifdef TL_DEBUG
    if(maxStromAnz>=10) {
        DEBUG_OUT << _junction->getID() << ":" << maxStromAnz << endl;
    }
#endif

    // compute the link cliquen
    NBLinkCliqueContainer cliquen(v, maxStromAnz);
    // compute the phases
    NBTrafficLightPhases *phases = cliquen.computePhases(v,
        maxStromAnz, appendSmallestOnly, skipLarger);
    // compute the possible logics
    NBTrafficLightLogicVector *logics =
        phases->computeLogics(key, type, getSizes().second, cei1,
        _links, breakingTime);
    // clean everything
    delete v;
    delete phases;
    return logics;
}


std::vector<std::bitset<64> > *
NBOwnTLDef::getPossibilityMatrix(bool joinLaneLinks,
                                 bool removeTurnArounds,
                                 LinkRemovalType removalType) const
{
    size_t noEdges = _incoming.size();
    // go through all links
    NBRequestEdgeLinkIterator cei1(this, joinLaneLinks, removeTurnArounds,
        removalType);
    std::vector<std::bitset<64> > *ret =
        new std::vector<std::bitset<64> >(cei1.getNoValidLinks(),
        std::bitset<64>());
    do {
        assert(ret!=0 && cei1.getLinkNumber()<ret->size());
        (*ret)[cei1.getLinkNumber()].set(cei1.getLinkNumber(), 1);
        NBRequestEdgeLinkIterator cei2(cei1);
        if(cei2.pp()) {
            do {
                if(cei1.forbids(cei2)) {
                    assert(ret!=0 && cei1.getLinkNumber()<ret->size());
                    assert(ret!=0 && cei2.getLinkNumber()<ret->size());
                    (*ret)[cei1.getLinkNumber()].set(cei2.getLinkNumber(), 0);
                    (*ret)[cei2.getLinkNumber()].set(cei1.getLinkNumber(), 0);
                } else {
                    (*ret)[cei1.getLinkNumber()].set(cei2.getLinkNumber(), 1);
                    (*ret)[cei2.getLinkNumber()].set(cei1.getLinkNumber(), 1);
                }
            } while(cei2.pp());
        }
    } while(cei1.pp());
    return ret;
}


void
NBOwnTLDef::collectNodes()
{
}


void
NBOwnTLDef::collectLinks()
{
    // build the list of links which are controled by the traffic light
    for(EdgeVector::iterator i=_incoming.begin(); i!=_incoming.end(); i++) {
        NBEdge *incoming = *i;
        size_t noLanes = incoming->getNoLanes();
        for(size_t j=0; j<noLanes; j++) {
            const EdgeLaneVector *connected = incoming->getEdgeLanesFromLane(j);
            for(EdgeLaneVector::const_iterator k=connected->begin(); k!=connected->end(); k++) {
                const EdgeLane &el = *k;
                if(el.edge!=0) {
                    _links.push_back(
                            NBConnection(incoming, j, el.edge, el.lane));
                }
            }
        }
    }
}


void
NBOwnTLDef::setParticipantsInformation()
{
    // assign participating nodes to the request
    collectNodes();
    // collect the infomration about participating edges and links
    collectEdges();
    collectLinks();
}

void
NBOwnTLDef::setTLControllingInformation() const
{
    // set the information about the link's positions within the tl into the
    //  edges the links are starting at, respectively
    size_t pos = 0;
    for(NBConnectionVector::const_iterator j=_links.begin(); j!=_links.end(); j++) {
        const NBConnection &conn = *j;
        NBEdge *edge = conn.getFrom();
        edge->setControllingTLInformation(
            conn.getFromLane(), conn.getTo(), conn.getToLane(),
            getID(), pos++);
    }
}


void
NBOwnTLDef::remapRemoved(NBEdge *removed, const EdgeVector &incoming,
                         const EdgeVector &outgoing)
{
}


void
NBOwnTLDef::replaceRemoved(NBEdge *removed, int removedLane,
                           NBEdge *by, int byLane)
{
}


