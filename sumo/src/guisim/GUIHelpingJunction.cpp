#include <vector>
#include <microsim/MSJunction.h>
#include "GUIHelpingJunction.h"



void
GUIHelpingJunction::fill(std::vector<GUIJunctionWrapper*> &list,
                         GUIGlObjectStorage &idStorage)
{
    size_t size = MSJunction::dictSize();
    list.reserve(size);
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        GUIJunctionWrapper *wrapper =
            (*i).second->buildJunctionWrapper(idStorage);
        list.push_back(wrapper);
    }
}

