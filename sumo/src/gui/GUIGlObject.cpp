#include <string>
#include "GUIGlObject.h"
#include "GUIGlObjectStorage.h"

GUIGlObject::GUIGlObject(GUIGlObjectStorage &idStorage,
                         std::string fullName)
    : myFullName(fullName)
{
    idStorage.registerObject(this);
}


GUIGlObject::~GUIGlObject()
{
}


const std::string &
GUIGlObject::getFullName() const
{
    return myFullName;
}


size_t
GUIGlObject::getGlID() const
{
    return myGlID;
}


void
GUIGlObject::setGlID(size_t id)
{
    myGlID = id;
}


