from enum import Enum

#--------------------------------
# NETWORK
#--------------------------------

class lane:
    class inspect:
        speed = 1
        allowButton = 2             # missing
        allow = 3
        disallowButton = 4          # missing
        disallow = 5
        width = 6
        endOffset = 7
        acceleration = 8
        customShape = 9
        opposite = 10               # missing
        changeLeftButton = 11       # missing
        changeLeft = 12             # missing
        changeRightButton = 13      # missing
        changeRight = 14            # missing
        type = 15                   # missing
        stopOffset = 16             # missing
        stopOffsetException = 17    # missing
        parameters = 21
    class inspectSelection:
        speed = 1
        allowButton = 2             # missing
        allow = 3
        disallowButton = 4          # missing
        disallow = 5
        width = 6
        endOffset = 7
        acceleration = 8
        changeLeftButton = 9        # missing
        changeLeft = 10             # missing
        changeRightButton = 11      # missing
        changeRight = 12            # missing
        type = 13                   # missing
        stopOffset = 14             # missing
        stopOffsetException = 15    # missing
        parameters = 19

#--------------------------------
# ADDITIONALS
#--------------------------------

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

# trainStop
class trainStop:
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
        
# containerStop
class containerStop:
    class create:
        idEnable = 5
        id = 6
        name = 6
        friendlyPos = 7
        lines = 8
        containerCapacity = 9
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
        containerCapacity = 8
        parkingLength = 9
        colorDialog = 10        # missing
        color = 11
        parameters = 14
    class inspectSelection():
        name = 1
        friendlyPos = 2
        lines = 3
        containerCapacity = 4
        parkingLength = 5
        colorDialog = 6        # missing
        color = 7
        parameters = 10

# chargingStation
class chargingStation:
    class create:
        idEnable = 5
        id = 6
        name = 6
        friendlyPos = 7
        power = 8
        efficiency = 9
        chargeInTransit = 10
        chargeDelay = 11
        references = 15
        length = 17
    class inspect():
        id = 1
        lane = 2
        startPos = 3
        endPos = 4
        name = 5
        friendlyPos = 6
        power = 7
        efficiency = 8
        chargeInTransit = 9
        chargeDelay = 10
        parameters = 13
    class inspectSelection():
        name = 1
        friendlyPos = 2
        power = 3
        efficiency = 4
        chargeInTrainsit = 5
        chargeDelay = 6
        parameters = 9