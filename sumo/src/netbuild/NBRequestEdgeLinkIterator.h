#ifndef NBRequestEdgeLinkIterator_h
#define NBRequestEdgeLinkIterator_h

#include <bitset>
#include <vector>
#include "NBRequest.h"
#include "NBContHelper.h"

class NBRequestEdgeLinkIterator {
private:
    const NBRequest * const _request;
    EdgeCont _fromEdges;
    EdgeCont _toEdges;
    size_t _linkNumber;
    size_t _validLinks;
    size_t _position;
    std::bitset<64> _validNonLeft;
    std::bitset<64> _valid;
    std::vector<size_t> _positions;
    std::vector<bool> _isLeftMover;
    std::vector<bool> _isTurnaround;
    std::vector<int> _fromLanes;
    bool _joinLaneLinks;
    size_t _outerValidLinks;
public:
    NBRequestEdgeLinkIterator(const NBRequest * const request,
        bool joinLanes, bool removeTurnArounds, 
        NBRequest::LinkRemovalType removalType);
    ~NBRequestEdgeLinkIterator();
    size_t getLinkNumber() const;
    NBEdge *getFromEdge() const;
    NBEdge *getToEdge() const;
    size_t getNoValidLinks() const;
    size_t getNumberOfAssignedLinks(size_t pos) const;
    bool pp();
    void resetNonLeftMovers(std::bitset<64> &driveMask,
        std::bitset<64> &brakeMask) const;
    bool forbids(const NBRequestEdgeLinkIterator &other) const;
    bool testBrakeMask(int set, size_t pos) const;
private:
    bool isLeftMover(const NBRequest * const request,
        NBEdge *from, NBEdge *to) const;
    void init(const NBRequest * const request, bool joinLanes,
        bool removeTurnArounds, NBRequest::LinkRemovalType removalType);
    void joinLaneLinksFunc(const EdgeCont * const incoming, 
        bool joinLaneLinks);
    void setValidNonLeft(bool removeTurnArounds, 
        NBRequest::LinkRemovalType removalType);
    void computeValidLinks();
    bool valid(size_t pos, bool removeTurnArounds, 
        NBRequest::LinkRemovalType removalType);
    bool internJoinLaneForbids(NBEdge *fromEdge, NBEdge *toEdge) const;
};

#endif

