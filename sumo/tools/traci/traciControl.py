import socket, time, struct

# Command IDs
CMD_SIMSTEP = 0x01
CMD_STOP = 0x12
CMD_CHANGETARGET = 0x31
CMD_CLOSE = 0x7F
CMD_MOVENODE = 0x80
CMD_INDLOOP = 0xa0
CMD_CHANGETLSTATE = 0xc2

# Data Types
STRINGLIST = 0x0E

# Composed Data Types
POSITION_ROADMAP = 0x04


# Variables of Induction Loop Value Retrieval
VEHICLE_NUMBER = 0x10

# Variables of Traffic Lights Value
TLSTATE = 0x21
RESULTS = {0x00: "OK", 0x01: "Not implemented", 0xFF: "Error"}

class Message:
    string = ""
    queue = []

_socket = socket.socket()
_message = Message()

class Storage:

    def __init__(self, content):
        self._content = content
        self._pos = 0

    def read(self, format):
        oldPos = self._pos
        self._pos += struct.calcsize(format)
        return struct.unpack(format, self._content[oldPos:self._pos])

    def readString(self):
        length = self.read("!i")[0]
        return self.read("!%ss" % length)[0]
    
    def ready(self):
        return self._pos < len(self._content) 


def _recvExact():
    result = ""
    while len(result) < 4:
        result += _socket.recv(4 - len(result))
    length = struct.unpack("!i", result)[0] - 4
    result = ""
    while len(result) < length:
        result += _socket.recv(length - len(result))
    return Storage(result)

def _sendExact():
    length = struct.pack("!i", len(_message.string)+4)
    _socket.send(length)
    _socket.send(_message.string)
    _message.string = ""
    result = _recvExact()
    for command in _message.queue:
        prefix = result.read("!BBB")
        err = result.readString()
        if prefix[2] or err:
            print prefix, RESULTS[prefix[2]], err
        elif prefix[1] != command:
            print "Error! Received answer %s for command %s." % (prefix[1], command)
        elif prefix[1] == CMD_STOP:
            length = result.read("!B")[0] - 1
            result.read("!%sx" % length)
    _message.queue = []
    return result

	
def initTraCI(port):
    for wait in range(10):
        try:
            _socket.connect(("localhost", port))
            break
        except socket.error:
            time.sleep(wait)

			
def simStep(step):
    _message.queue.append(CMD_SIMSTEP)
    _message.string += struct.pack("!BBdB", 1+1+8+1, CMD_SIMSTEP, float(step), POSITION_ROADMAP)
    result = _sendExact()
    updates = []
    while result.ready():
        if result.read("!BB")[1] == CMD_MOVENODE: 
            updates.append((result.read("!idB")[0], result.readString(), result.read("!fB")[0]))
    return updates

def getVehNoOfIndLoop(IndLoopID):
    _message.queue.append(CMD_INDLOOP)
    _message.string += struct.pack("!BBBi", 1+1+1+4+len(IndLoopID), CMD_INDLOOP, VEHICLE_NUMBER, len(IndLoopID)) + IndLoopID

    result = _sendExact()
    result.read("!B")
    result.read("!B")
    result.read("!B")
    result.read("!B")
    result.read("!B")     # Length
    result.read("!B")     # Identifier
    result.read("!B")     # Variable
    result.readString()   # Induction Loop ID
    result.read("!B")     # Return type of the variable
    return result.read("!i")[0]    # Variable value

def setPhase(TLID, state):
    [phase, brake, yellow] = state
    _message.queue.append(CMD_CHANGETLSTATE)
    _message.string += struct.pack("!BBBi", 1+1+1+4+len(TLID)+1+4+4+len(phase)+4+len(brake)+4+len(yellow), CMD_CHANGETLSTATE, TLSTATE, len(TLID)) + TLID
    _message.string += struct.pack("!Bi", STRINGLIST, 3)
    _message.string += struct.pack("!i", len(phase)) + phase
    _message.string += struct.pack("!i", len(brake)) + brake
    _message.string += struct.pack("!i", len(yellow)) + yellow
    _sendExact()
	
def stopObject(edge, objectID, pos=1., duration=10000.):
    _message.queue.append(CMD_STOP)
    _message.string += struct.pack("!BBiBi", 1+1+4+1+4+len(edge)+4+1+4+8, CMD_STOP, objectID, POSITION_ROADMAP, len(edge)) + edge
    _message.string += struct.pack("!fBfd", pos, 0, 1., duration)

def changeTarget(edge, objectID):
    _message.queue.append(CMD_CHANGETARGET)
    _message.string += struct.pack("!BBii", 1+1+4+4+len(edge), CMD_CHANGETARGET, objectID, len(edge)) + edge

def close():
    _message.queue.append(CMD_CLOSE)
    _message.string += struct.pack("!BB", 1+1, CMD_CLOSE)
    _sendExact()
    _socket.close()
