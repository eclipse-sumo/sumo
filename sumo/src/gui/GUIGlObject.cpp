#include <string>
#include "GUIGlObject.h"


GUIGlObject::GUIGlObject(std::string fullName)
    : myFullName(fullName)
{
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


