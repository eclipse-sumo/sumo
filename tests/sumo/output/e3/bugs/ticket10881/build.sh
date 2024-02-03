#!/bin/bash
python $SUMO_HOME/tools/output/generateTLSE3Detectors.py -n ../net.net.xml \
    -f 3600 -o input_additional.add.xml -r detector.xml --junction-ids C \
    --min-pos 7.5 --joined
