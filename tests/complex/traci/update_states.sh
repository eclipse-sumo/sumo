#!/bin/bash
sumo -c simulation/loadState/busStop/save_state.sumocfg
sumo -c simulation/loadState/detectors/save_state.sumocfg
sumo -c simulation/loadState/railSignal/save_state.sumocfg
sumo -c simulation/loadState/repeat/save_state.sumocfg
sumo -c simulation/loadState/rerouting/save_state.sumocfg
sumo -c bugs/ticket3505/save_state.sumocfg
sumo -c bugs/ticket3505_test_with_begin_ge_savetime/save_state.sumocfg
sumo -c bugs/ticket3505_with_load_offset/save_state.sumocfg

