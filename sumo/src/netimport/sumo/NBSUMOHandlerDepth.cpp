#include <string>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include "NBSUMOHandlerDepth.h"


NBSUMOHandlerDepth::NBSUMOHandlerDepth(LoadFilter what, bool warn, bool verbose)
    : SUMOSAXHandler(warn, verbose),
    _loading(what)
{
}


NBSUMOHandlerDepth::~NBSUMOHandlerDepth()
{
}


void
NBSUMOHandlerDepth::myStartElement(int element, const std::string &name,
                                   const Attributes &attrs)
{
}


void
NBSUMOHandlerDepth::myCharacters(int element, const std::string &name,
                                 const std::string &chars)
{
}

void
NBSUMOHandlerDepth::myEndElement(int element, const std::string &name)
{
}


