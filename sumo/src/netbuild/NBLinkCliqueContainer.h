#ifndef NBLinkCliqueContainer_h
#define NBLinkCliqueContainer_h

#include <vector>
#include <bitset>
#include "NBLinkPossibilityMatrix.h"
#include "NBTrafficLightPhases.h"

class NBLinkCliqueContainer {
private:
    typedef std::vector<std::bitset<64> > LinkCliqueContainer;
    LinkCliqueContainer _cliques;
    typedef std::vector<std::bitset<64> > FurtherPossibleContainer;
    FurtherPossibleContainer _further;
public:
    NBLinkCliqueContainer(NBLinkPossibilityMatrix *v, 
        size_t maxStromAnz);
    ~NBLinkCliqueContainer();
    NBTrafficLightPhases *computePhases(NBLinkPossibilityMatrix *v,
        size_t noLinks, bool appendSmallestOnly, bool skipLarger) const;
    bool test(size_t itemIndex, size_t linkIndex) const;
private:
    void buildCliques(NBLinkPossibilityMatrix *v, size_t maxStromAnz);
    void buildFurther();
    bool furtherResolutionPossible(std::bitset<64> vorhanden,
        std::bitset<64> needed, size_t next) const;
};

#endif

