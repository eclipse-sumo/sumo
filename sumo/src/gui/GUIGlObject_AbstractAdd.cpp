#include "GUIGlObject_AbstractAdd.h"
#include <cassert>
#include <iostream>

using namespace std;

std::map<std::string, GUIGlObject_AbstractAdd*> GUIGlObject_AbstractAdd::myObjects;
std::vector<GUIGlObject_AbstractAdd*> GUIGlObject_AbstractAdd::myObjectList;

GUIGlObject_AbstractAdd::GUIGlObject_AbstractAdd(GUIGlObjectStorage &idStorage,
                                               std::string fullName,
                                               GUIGlObjectType type)
    : GUIGlObject(idStorage, fullName), myGlType(type)
{
//!!!    assert(myObjects.find(fullName)==myObjects.end());
    myObjects[fullName] = this;
    myObjectList.push_back(this);
}


GUIGlObject_AbstractAdd::GUIGlObject_AbstractAdd(GUIGlObjectStorage &idStorage,
                                               std::string fullName,
                                               size_t glID,
                                               GUIGlObjectType type)
    : GUIGlObject(idStorage, fullName, glID), myGlType(type)
{
//!!!    assert(myObjects.find(fullName)==myObjects.end());
    myObjects[fullName] = this;
    myObjectList.push_back(this);
}


GUIGlObject_AbstractAdd::~GUIGlObject_AbstractAdd()
{
}



GUIGlObjectType
GUIGlObject_AbstractAdd::getType() const
{
    return myGlType;
}


void
GUIGlObject_AbstractAdd::clearDictionary()
{
    std::map<std::string, GUIGlObject_AbstractAdd*>::iterator i;
    for(i=myObjects.begin(); i!=myObjects.end(); i++) {
//!!!        delete (*i).second;
    }
    myObjects.clear();
    myObjectList.clear();
}


GUIGlObject_AbstractAdd *
GUIGlObject_AbstractAdd::get(const std::string &name)
{
    std::map<std::string, GUIGlObject_AbstractAdd*>::iterator i =
        myObjects.find(name);
    if(i==myObjects.end()) {
        return 0;
    }
    return (*i).second;
}


const std::vector<GUIGlObject_AbstractAdd*> &
GUIGlObject_AbstractAdd::getObjectList()
{
    return myObjectList;
}