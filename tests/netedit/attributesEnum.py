from enum import Enum

# busStop
class busStop:
    class create:
        idEnable = 5
        id = 6
        name = 6
        friendlyPos = 7
        lines = 8
        personCapacity = 9
        parkingLength = 10
        colorDialog = 11
        color = 12
        references = 16
        length = 18
    class inspect():
        id = 1
        lane = 2
        startPos = 3
        endPos = 4
        name = 5
        friendlyPos = 6
        lines = 7
        personCapacity = 8
        parkingLength = 9
        colorDialog = 10        # missing
        color = 11
        parameters = 14
    class inspectSelection():
        name = 1
        friendlyPos = 2
        lines = 3
        personCapacity = 4
        parkingLength = 5
        colorDialog = 6        # missing
        color = 7
        parameters = 10