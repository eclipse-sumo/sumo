---
title: Adding a pedestrian model
---

# Adding a pedestrian model

A pedestrian model controls movement during a person's walking stage. See the [pedestrian update loop](../Implementation_Notes/Simulation_Loop.md#pedestrian_lifecycle_and_update_loop) for the sequence of calls and [pedestrian models](../../Simulation/Pedestrians.md#pedestrian_models) for the available models and their options.

## Implement movement and state

1. Derive the model from [`MSPModel`]({{Source}}src/microsim/transportables/MSPModel.h). For an event-based implementation, start with [`MSPModel_NonInteracting`]({{Source}}src/microsim/transportables/MSPModel_NonInteracting.cpp). For a model that interacts with vehicles, see [`MSPModel_Interacting`]({{Source}}src/microsim/transportables/MSPModel_Interacting.h) and its use by [`MSPModel_Striping`]({{Source}}src/microsim/transportables/MSPModel_Striping.cpp).
2. Implement `add` to register a pedestrian at the start of a walk and return an `MSTransportableStateAdapter`. The walking stage uses this adapter to read the pedestrian's position, lane, direction, speed, and waiting time. Implement `remove` and `clearState` to remove pedestrians from the model and stop pending updates that refer to them. Implement `getActiveNumber` and indicate whether the model uses internal junction lanes with `usingInternalLanes`.
3. Schedule movement updates using [`MSNet::getBeginOfTimestepEvents`]({{Source}}src/microsim/MSNet.h) or follow the existing model that uses a suitable update schedule. Call [`MSStageWalking::moveToNextEdge`]({{Source}}src/microsim/transportables/MSStageWalking.cpp) when the pedestrian reaches the next edge or finishes walking. Its return value signals whether the walk has finished.
4. If vehicles must respond to pedestrians in the model, implement or reuse `hasPedestrians`, `blockedAtDist`, and `nextBlocking`. For simulation state save and load, implement the adapter's `saveState` and the model's `loadState` as a pair, including any timing or state needed to resume movement.

## Build and select the model

1. Add the model's source files to [`src/microsim/transportables/CMakeLists.txt`]({{Source}}src/microsim/transportables/CMakeLists.txt) for the `microsim_transportables` target.
2. Add the model name and constructor call to the person branch of [`MSTransportableControl::MSTransportableControl`]({{Source}}src/microsim/transportables/MSTransportableControl.cpp). This branch selects the implementation for `--pedestrian.model`.
3. Update the option description and register any model-specific options in [`MSFrame::fillOptions`]({{Source}}src/microsim/MSFrame.cpp). For a model with an optional library, use the same build condition for the new sources, include, and constructor branch.
4. Rebuild SUMO using the [platform instructions](../index.md#build_instructions). Select the model with `--pedestrian.model` followed by the registered name.

## Test the model

Add [TextTest cases](../Tests.md#adding_a_test_to_an_existing_application) under [`tests/sumo/pedestrian_model`]({{Source}}tests/sumo/pedestrian_model). Register the new suite in its parent `testsuite.sumo` and put `--pedestrian.model` followed by the model name in its `options.sumo`. Start with a small walk across multiple edges and check that the person enters and completes the walking stage. Add focused cases for any supported behavior such as vehicle interactions at crossings, subsecond updates, or saving and reloading state.

To exercise the selection and movement workflow with an existing model, run the following commands from the source tree with the SUMO binaries on your PATH:

```bash
python tools/extractTest.py -x -d -o pedestrian-smoke tests/sumo/pedestrian_model/nonInteracting/3_arm_pathing
sumo -c pedestrian-smoke/test.sumocfg --pedestrian.model nonInteracting --person-fcd-output pedestrian-smoke/person-fcd.xml --person-device.fcd.probability 1
```

Check the completed walks in `pedestrian-smoke/tripinfos.xml` and their positions in `pedestrian-smoke/person-fcd.xml`. To try your model on the same input, replace `nonInteracting` in the SUMO command with your model's registered name.
