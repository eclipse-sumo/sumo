---
title: traci
---

# areal.py

# constants.py

This script contains TraCI constant definitions from
<SUMO_HOME\>/src/traci-server/TraCIConstants.h.

# domain.py

Provides the base `Domain` class shared by all TraCI entity modules, including `subscribe`, `getSubscriptionResults`, `getParameter`, `getParameterWithKey`, `subscribeParameterWithKey`, and `setParameter` (see [Object Variable Subscription](../TraCI/Object_Variable_Subscription.md)).

## Subscribing to Generic Parameters

`getSubscriptionResults(objID)` returns a dictionary keyed by variable ID (`int`). Consequently, `tc.VAR_PARAMETER` (`0x7e`) and `tc.VAR_PARAMETER_WITH_KEY` (`0x3e`) can each hold one parameter per object subscription. Pair `tc.VAR_PARAMETER` with `subscribeParameterWithKey` (`tc.VAR_PARAMETER_WITH_KEY`) to receive two generic parameters per step:

```python
import traci
import traci.constants as tc

traci.vehicle.subscribe(vehID, [tc.VAR_PARAMETER], parameters={tc.VAR_PARAMETER: "device.battery.actualBatteryCapacity"})
traci.vehicle.subscribeParameterWithKey(vehID, "device.battery.energyCharged")
# step results: res[tc.VAR_PARAMETER] -> value1; res[tc.VAR_PARAMETER_WITH_KEY] -> ("device.battery.energyCharged", value2)
```

Query additional generic parameters per step with `traci.vehicle.getParameter(vehID, key)`.

# edge.py

# gui.py

# introductionloop.py

# junction.py

# lane.py

# multientryexit.py

# poi.py

# polygon.py

# rebuildConstants.py

# route.py

# simulation.py

# trafficlights.py

# vehicle.py

# vehicletype.py
