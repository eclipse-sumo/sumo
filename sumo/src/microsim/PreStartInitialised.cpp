#include <microsim/MSNet.h>
#include "PreStartInitialised.h"

PreStartInitialised::PreStartInitialised(MSNet &net) 
{
    net.addPreStartInitialisedItem(this);
}

PreStartInitialised::~PreStartInitialised() 
{
}