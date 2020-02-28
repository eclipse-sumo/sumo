#include "fmi2Functions.h"

/* The function returns a new instance of an FMU. If a null pointer is returned, then instantiation
   failed.*/

fmi2Component 
fmi2Instantiate(fmi2String instanceName, fmi2Type fmuType, fmi2String fmuGUID,
                              fmi2String fmuResourceLocation, const fmi2CallbackFunctions *functions,
                              fmi2Boolean visible, fmi2Boolean loggingOn)
{

   /* For co-simulation, this function call has to perform all actions of a slave which are necessary
   before a simulation run starts */
   functions->logger(functions->componentEnvironment, instanceName, fmi2Error, "error", "Not yet implemented");
   return NULL;
}
