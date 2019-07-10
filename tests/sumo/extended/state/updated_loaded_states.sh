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

#cp save2/state.sumo load_and_save_including_actionStepLength/input_state.xml
cp save2/state.sumo.meso load_and_save_including_actionStepLength/input_meso_state.xml

#cp save2/state.sumo load_and_save_including_actionStepLength2/input_state.xml
cp save2/state.sumo.meso load_and_save_including_actionStepLength2/input_meso_state.xml

cp save_flow/state.sumo load_flow/input_state.xml
cp save_flow/state.sumo.meso load_flow/input_meso_state.xml

cp save_parking/state.sumo load_parking/input_state.xml
cp save_parking/state.sumo.meso load_parking/input_meso_state.xml

cp save_stopped/state.sumo load_stopped/input_state.xml
cp save_stopped/state.sumo.meso load_stopped/input_meso_state.xml

sumo -c load_arriving/saveState.sumocfg
sumo -c load_arriving/saveMesoState.sumocfg
sumo -c load_shifted/saveState.sumocfg
sumo -c load_shifted/saveMesoState.sumocfg

rm load_arriving/tripinfos.xml
rm load_shifted/tripinfos.xml
