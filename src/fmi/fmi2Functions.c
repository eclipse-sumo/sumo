#include "fmi2Functions.h"


/* ***********************************************************************************************
   * CREATION AND DESTRUCTION OF AN FMU
   ***********************************************************************************************/

/* The function returns a new instance of an FMU. If a null pointer is returned, then instantiation
   failed.*/
fmi2Component 
fmi2Instantiate(fmi2String instanceName, fmi2Type fmuType, fmi2String fmuGUID,
                              fmi2String fmuResourceLocation, const fmi2CallbackFunctions *functions,
                              fmi2Boolean visible, fmi2Boolean loggingOn)
{
   /* For co-simulation, this function call has to perform all actions of a slave which are necessary
   before a simulation run starts */
   if (loggingOn) 
      functions->logger(functions->componentEnvironment, instanceName, fmi2Error, "error", "Not yet implemented");
   
   return NULL;
}

/* Disposes the given instance, unloads the loaded model, and frees all the allocated memory
and other resources that have been allocated by the functions of the FMU interface. */
void 
fmi2FreeInstance(fmi2Component c) {
   if (!c) return;
   
   /* Frees the model instance */
   // ModelInstance *comp = (ModelInstance *)c;
   // c->freeMemory(c->componentEnvironment, 
}
