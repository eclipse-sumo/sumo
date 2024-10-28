#!/bin/bash
cp save2/state2.sumo load/input_state.xml
cp save2/state2.sumo.meso load/input_meso_state.xml

cp save2/state2.sumo load_scaled/input_state.xml
cp save2/state2.sumo.meso load_scaled/input_meso_state.xml

cp save2/state2.sumo load_remove/input_state.xml
cp save2/state2.sumo.meso load_remove/input_meso_state.xml

cp save2/state.sumo load_late_insertion/input_state.xml
cp save2/state.sumo.meso load_late_insertion/input_meso_state.xml

cp save2/state.sumo load_routes_as_additional/input_state.xml
cp save2/state.sumo.meso load_routes_as_additional/input_meso_state.xml

cp save2/state.sumo load_and_save/input_state.xml
cp save2/state.sumo.meso load_and_save/input_meso_state.xml

cp save_actionStepLength/state.sumo load_and_save_actionStepLength/input_state.xml
cp save_actionStepLength/state.sumo.meso load_and_save_actionStepLength/input_meso_state.xml

cp save_actionStepLength2/state.sumo load_and_save_actionStepLength2/input_state.xml
cp save_actionStepLength2/state.sumo.meso load_and_save_actionStepLength2/input_meso_state.xml

sumo -c load_arriving/saveState.sumocfg
sumo -c load_arriving/saveMesoState.sumocfg

sumo -c load_shifted/saveState.sumocfg
sumo -c load_shifted/saveMesoState.sumocfg

sumo -c load_and_save_same/saveState.sumocfg
sumo -c load_and_save_same/saveMesoState.sumocfg

rm load_arriving/tripinfos.xml
rm load_shifted/tripinfos.xml
