#ifndef __REMOTECONSTANTS_H_
#define __REMOTECONSTANTS_H_

// CONSTANTS
// holds codes used for transmission protocol
// @author: Thimor Bohn <bohn@itm.uni-luebeck.de>



// ****************************************
// COMMANDS
// ****************************************

// command: simulation step
#define CMD_SIMSTEP 0x01
// command: close sumo
#define CMD_CLOSE   0xFF
// result type: no result


// ****************************************
// RESULT TYPES
// ****************************************

// result type: none
#define RTYPE_NONE  0x00
// result type: absolute position (x/y)
#define RTYPE_ABS   0x01
// result type: relative position (road/road pos)
#define RTYPE_REL   0x02
// result type: error
#define RTYPE_ERR   0xFF

#endif

