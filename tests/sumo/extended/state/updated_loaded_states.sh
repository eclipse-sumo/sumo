#!/bin/bash
cp save2/state2.sumo load/input_state.xml
cp save2/state2.sumo.meso load/input_meso_state.xml

cp save2/state2.sumo load_scaled/input_state.xml
cp save2/state2.sumo.meso load_scaled/input_meso_state.xml

cp save2/state2.sumo load_remove/input_state.xml
cp save2/state2.sumo.meso load_remove/input_meso_state.xml

cp save2/state.sumo load_late_insertion/input_state.xml
cp save2/state.sumo.meso load_late_insertion/input_meso_state.xml

cp save2/state.sumo load_and_save/input_state.xml
cp save2/state.sumo.meso load_and_save/input_meso_state.xml

cp save_actionStepLength/state.sumo load_and_save_actionStepLength/input_state.xml
cp save_actionStepLength/state.sumo.meso load_and_save_actionStepLength/input_meso_state.xml

cp save_actionStepLength2/state.sumo load_and_save_actionStepLength2/input_state.xml
cp save_actionStepLength2/state.sumo.meso load_and_save_actionStepLength2/input_meso_state.xml

for f in flow parking parking_busStop stopped cyclic_stops rng rail_signal on_rail_signal rail_signal_constraint; do
    cp save_$f/state.sumo load_$f/input_state.xml
    cp save_$f/state.sumo.meso load_$f/input_meso_state.xml
done

sumo -c load_arriving/saveState.sumocfg
sumo -c load_arriving/saveMesoState.sumocfg
sumo -c load_shifted/saveState.sumocfg
sumo -c load_shifted/saveMesoState.sumocfg

rm load_arriving/tripinfos.xml
rm load_shifted/tripinfos.xml
