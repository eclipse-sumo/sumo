#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2021-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    macrOutput.py
# @author  Amirhosein Karbasi
# @date    2021-04-20


from __future__ import absolute_import
from __future__ import print_function
import sys
from collections import Counter

import numpy as np
import pandas as pd
import pandas_read_xml as pdx
import matplotlib.pyplot as plt


def main(args):

    if args is None or len(args) < 2:
        print("Error: An xml file must be given as input")
        sys.exit(1)

    df = pdx.read_xml(sys.argv[1], ['meandata'])

    df = pdx.flatten(df)
    df = df.pipe(pdx.flatten)
    df = df.pipe(pdx.flatten)
    df = df.pipe(pdx.flatten)

    df = df.rename({'interval|@begin': 'begin', 'interval|@end': 'end',
                    'interval|edge|@sampledSeconds': 'sampledSeconds', 'interval|edge|@density': 'density',
                    'interval|edge|@laneDensity': 'laneDensity', 'interval|edge|@speed': 'speed'}, axis=1)

    df['begin'] = df['begin'].astype(float)
    df['end'] = df['end'].astype(float)
    df["sampledSeconds"] = df["sampledSeconds"].astype(float)
    df["density"] = df["density"].astype(float)
    df["laneDensity"] = df["laneDensity"].astype(float)
    df["speed"] = df["speed"].astype(float)
    df = df.replace(np.NaN, 0)
    df['begin'] = df['begin'].astype(int)

    # calculation time interval
    bft = df.begin.iloc[0]
    eft = df.end.iloc[0]
    time_interval = eft - bft
    time_interval = int(time_interval)

    # The end time of the last interval
    _lastsimulationperiod_ = (df.end.iat[-1]).astype(int)

    # creating a list of all end time intervals
    _beginvalues_ = list(range(time_interval, _lastsimulationperiod_+time_interval, time_interval))

    # detecting number of segments
    counter1 = Counter(df.begin)
    _seg = counter1[0]

    # calculating total length of network
    length = df['sampledSeconds'] / (df['end']-df['begin']) / df['density']
    df['Length'] = length.replace(np.NaN, 0).replace(np.inf, 0)
    i = 0
    j = 0
    __net = []
    while _beginvalues_[i] < (_lastsimulationperiod_):

        _net = sum(df.Length.iloc[j:j+_seg])
        __net.append(_net)
        i = i+1
        j = j+_seg

    # calculating meandensity,meanflow,meanspeed (density=density)
    i = 0
    j = 0
    MD = []
    MS = []
    MF = []
    while _beginvalues_[i] < (_lastsimulationperiod_):

        numofveh = (1/time_interval)*(sum(df.sampledSeconds.iloc[j:j+_seg]))
        speedznumofveh = (1/time_interval)*(sum(df.sampledSeconds.iloc[j:j+_seg]*df.speed.iloc[j:j+_seg]))
        if numofveh > 0:
            meanspeed_ = 3.6*speedznumofveh/numofveh
        else:
            meanspeed_ = 0
        meandensity_ = (sum(df.density.iloc[j:j+_seg]*df.Length.iloc[j:j+_seg]))/_net
        meanflow_ = (sum(df.density.iloc[j:j+_seg]*df.Length.iloc[j:j+_seg]*df.speed.iloc[j:j+_seg]*3.6))/_net
        MD.append(meandensity_)
        MS.append(meanspeed_)
        MF.append(meanflow_)
        i = i+1
        j = j+_seg

    # plot
    plt.scatter(MD, MS)
    plt.xlabel("Density (Veh/km)")
    plt.ylabel("Speed (Km/hr)")
    plt.show()
    plt.scatter(MD, MF)
    plt.xlabel("Density (Veh/km)")
    plt.ylabel("Flow (Veh/hr)")
    plt.show()
    plt.scatter(MS, MF)
    plt.xlabel("Speed (Km/hr)")
    plt.ylabel("Flow (Veh/hr)")
    plt.show()

    # calculating meandensity,meanflow,meanspeed (density=laneDensity)
    i = 0
    j = 0
    lMD = []
    lMS = []
    lMF = []
    while _beginvalues_[i] <= (_lastsimulationperiod_ - time_interval):

        numofveh = (1/time_interval)*(sum(df.sampledSeconds.iloc[j:j+_seg]))
        speedznumofveh = (1/time_interval)*(sum(df.sampledSeconds.iloc[j:j+_seg]*df.speed.iloc[j:j+_seg]))
        if numofveh > 0:
            meanspeed_ = 3.6*speedznumofveh/numofveh
        else:
            meanspeed_ = 0
        meandensity_ = (sum(df.laneDensity.iloc[j:j+_seg]*df.Length.iloc[j:j+_seg]))/_net
        meanflow_ = (sum(df.laneDensity.iloc[j:j+_seg]*df.Length.iloc[j:j+_seg]*df.speed.iloc[j:j+_seg]*3.6))/_net
        lMD.append(meandensity_)
        lMS.append(meanspeed_)
        lMF.append(meanflow_)
        i = i+1
        j = j+_seg

    # plot
    plt.scatter(lMD, lMS)
    plt.xlabel("Density (Veh/km)")
    plt.ylabel("Speed (Km/hr)")
    plt.show()
    plt.scatter(lMD, lMF)
    plt.xlabel("Density (Veh/km)")
    plt.ylabel("Flow (Veh/hr)")
    plt.show()
    plt.scatter(lMS, lMF)
    plt.xlabel("Speed (Km/hr)")
    plt.ylabel("Flow (Veh/hr)")
    plt.show()

    # Build a csv file
    Macro_Features = {'Density': MD,
                      'Speed': MS,
                      'Flow': MF,
                      }
    df = pd.DataFrame(Macro_Features, columns=['Density', 'Speed', 'Flow'])

    df.to_csv('Macro_density.csv')

    # Build a csv file
    Macro_Features = {'Density': lMD,
                      'Speed': lMS,
                      'Flow': lMF,
                      }
    df = pd.DataFrame(Macro_Features, columns=['Density', 'Speed', 'Flow'])

    df.to_csv('Macro_lanedensity.csv')


if __name__ == "__main__":
    sys.exit(main(sys.argv))
