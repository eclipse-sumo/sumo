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

cp save_flow/state.sumo load_flow/input_state.xml
cp save_flow/state.sumo.meso load_flow/input_meso_state.xml

cp save_parking/state.sumo load_parking/input_state.xml
cp save_parking/state.sumo.meso load_parking/input_meso_state.xml

cp save_parking_busStop/state.sumo load_parking_busStop/input_state.xml
cp save_parking_busStop/state.sumo.meso load_parking_busStop/input_meso_state.xml

cp save_parking_busstop/state.sumo load_parking/input_state.xml
cp save_parking_busstop/state.sumo.meso load_parking/input_meso_state.xml

cp save_stopped/state.sumo load_stopped/input_state.xml
cp save_stopped/state.sumo.meso load_stopped/input_meso_state.xml

cp save_cyclic_stops/state.sumo load_cyclic_stops/input_state.xml
cp save_cyclic_stops/state.sumo.meso load_cyclic_stops/input_meso_state.xml

cp save_rng/state.sumo load_rng/input_state.xml
cp save_rng/state.sumo.meso load_rng/input_meso_state.xml

cp save_rail_signal/state.sumo load_rail_signal/input_state.xml
cp save_rail_signal/state.sumo.meso load_rail_signal/input_meso_state.xml

cp save_on_rail_signal/state.sumo load_on_rail_signal/input_state.xml
cp save_on_rail_signal/state.sumo.meso load_on_rail_signal/input_meso_state.xml

sumo -c load_arriving/saveState.sumocfg
sumo -c load_arriving/saveMesoState.sumocfg
sumo -c load_shifted/saveState.sumocfg
sumo -c load_shifted/saveMesoState.sumocfg

rm load_arriving/tripinfos.xml
rm load_shifted/tripinfos.xml
