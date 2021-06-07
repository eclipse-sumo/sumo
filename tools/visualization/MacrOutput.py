# @file    MacrOutput.py
# @author  Amirhosein Karbasi
# @date    2021-04-20


from __future__ import absolute_import
from __future__ import print_function
import pandas as pd
import matplotlib.pyplot as plt

import os
import sys

sys.path.append(os.path.dirname(os.path.dirname(os.path.realpath(__file__))))
import sumolib  # noqa
from sumolib.visualization import helpers  # noqa


def main(args=None):

    # ---------- build and read options ----------
    from optparse import OptionParser
    optParser = OptionParser()
    optParser.add_option("-i", "--input", dest="input", metavar="FILE",
                         help="import the output file to use as input")

    # standard plot options
    helpers.addInteractionOptions(optParser)
    helpers.addPlotOptions(optParser)
    # parse
    options, remaining_args = optParser.parse_args(args=args)

    if options.input is None:
        print("Error: at least one xml file must be given")
        sys.exit(1)
        
    #Converting XML to dataframe
    import pandas_read_xml as pdx
    from pandas_read_xml import flatten, fully_flatten, auto_separate_tables

    new= options.input

    df = pdx.read_xml(new, ['meandata'])

    df = pdx.flatten(df)
    df = df.pipe(flatten)
    df = df.pipe(flatten)
    df = df.pipe(flatten)

    df = df.rename({'interval|@begin': 'begin', 'interval|@end': 'end'
                  , 'interval|edge|@sampledSeconds': 'sampledSeconds', 'interval|edge|@density': 'density'
                  , 'interval|edge|@laneDensity': 'laneDensity', 'interval|edge|@speed': 'speed'
                  }, axis=1)  # new method

    import numpy as np
    df['begin'] = df['begin'].astype(float)
    df['end'] = df['end'].astype(float)
    df["sampledSeconds"] = df["sampledSeconds"].astype(float)
    df["density"] = df["density"].astype(float)
    df["laneDensity"] = df["laneDensity"].astype(float)
    df["speed"] = df["speed"].astype(float)
    df=df.replace(np.NaN,0)
    df['begin'] = df['begin'].astype(int)
    
    #calculation time interval
    bft=df.begin.iloc[0]
    eft=df.end.iloc[0]
    time_interval = eft - bft
    time_interval = int(time_interval)

    
    # The end time of the last interval 
    _lastsimulationperiod_=  (df.end.iat[-1]).astype(int)
    type(_lastsimulationperiod_)


    # The begin time of the last interval 
    _blastsimulationperiod_=  df.begin.iat[-1].astype(int)
    type(_blastsimulationperiod_)


    # creating a list of all begin time intervals
    _beginvalues_=list(range(0,_blastsimulationperiod_+time_interval,time_interval))


    # creating a list of all end time intervals
    _beginvalues_=list(range(time_interval,_lastsimulationperiod_+time_interval,time_interval))


    # creating a list of all density values
    _densityvalues_=list(df.density)


    # creating a list of all lane density values
    _ldensityvalues_=list(df.laneDensity)


    # creating a list of all speed values
    _speedvalues_=list(df.speed)


    # creating a list of all sample seconds values
    _sssvalues_=list(df.sampledSeconds)

    #detecting number of segments 
    from collections import Counter
    counter1= Counter(df.begin)
    _seg = counter1[0]


    # calculating total lenght of network
    Length = df.iloc[:,6] / (df.iloc[:,3]-df.iloc[:,2]) / df.iloc[:,9]
    Length=Length.replace(np.NaN,0)
    Length=Length.replace(np.inf,0)
    df['Length'] = Length
    i=0
    j=0
    __net=[]
    while _beginvalues_[i]<(_lastsimulationperiod_):

        _net= sum(df.Length.iloc[j:j+_seg])
        __net.append(_net)
        f_net = max(__net)
        i=i+1
        j=j+_seg

    #calculating meandensity,meanflow,meanspeed (density=density)
    i=0
    j=0
    MD=[]
    MS=[]
    MF=[]
    while _beginvalues_[i]<(_lastsimulationperiod_):

        numofveh = (1/time_interval)*(sum(df.sampledSeconds.iloc[j:j+_seg]))
        speedznumofveh = (1/time_interval)*(sum(df.sampledSeconds.iloc[j:j+_seg]*df.speed.iloc[j:j+_seg]))
        if numofveh > 0:
            meanspeed_=3.6*speedznumofveh/numofveh
        else:
            meanspeed_=0
        meandensity_ = (sum(df.density.iloc[j:j+_seg]*df.Length.iloc[j:j+_seg]))/_net
        meanflow_ = (sum(df.density.iloc[j:j+_seg]*df.Length.iloc[j:j+_seg]*df.speed.iloc[j:j+_seg]*3.6))/_net
        MD.append(meandensity_)
        MS.append(meanspeed_)
        MF.append(meanflow_)
        i=i+1
        j=j+_seg


    #plot
    plt.scatter(MD,MS)
    plt.xlabel("Density (Veh/km)")
    plt.ylabel("Speed (Km/hr)")
    plt.show()
    plt.scatter(MD,MF)
    plt.xlabel("Density (Veh/km)")
    plt.ylabel("Flow (Veh/hr)")
    plt.show()
    plt.scatter(MS,MF)
    plt.xlabel("Speed (Km/hr)")
    plt.ylabel("Flow (Veh/hr)")
    plt.show()


    #calculating meandensity,meanflow,meanspeed (density=laneDensity)
    i=0
    j=0
    lMD=[]
    lMS=[]
    lMF=[]
    while _beginvalues_[i]<=(_lastsimulationperiod_ - time_interval):

        numofveh = (1/time_interval)*(sum(df.sampledSeconds.iloc[j:j+_seg]))
        speedznumofveh = (1/time_interval)*(sum(df.sampledSeconds.iloc[j:j+_seg]*df.speed.iloc[j:j+_seg]))
        if numofveh > 0:
            meanspeed_=3.6*speedznumofveh/numofveh
        else:
            meanspeed_=0
        meandensity_ = (sum(df.laneDensity.iloc[j:j+_seg]*df.Length.iloc[j:j+_seg]))/_net
        meanflow_ = (sum(df.laneDensity.iloc[j:j+_seg]*df.Length.iloc[j:j+_seg]*df.speed.iloc[j:j+_seg]*3.6))/_net
        lMD.append(meandensity_)
        lMS.append(meanspeed_)
        lMF.append(meanflow_)
        i=i+1
        j=j+_seg

    #plot
    plt.scatter(lMD,lMS)
    plt.xlabel("Density (Veh/km)")
    plt.ylabel("Speed (Km/hr)")
    plt.show()
    plt.scatter(lMD,lMF)
    plt.xlabel("Density (Veh/km)")
    plt.ylabel("Flow (Veh/hr)")
    plt.show()
    plt.scatter(lMS,lMF)
    plt.xlabel("Speed (Km/hr)")
    plt.ylabel("Flow (Veh/hr)")
    plt.show()
    


    #Build a csv file 
    Macro_Features = {'Density': MD,
            'Speed': MS,
            'Flow': MF,
            }
    df = pd.DataFrame(Macro_Features, columns= ['Density', 'Speed','Flow'])

    df.to_csv('Macro_density.csv')
    
    #Build a csv file 
    Macro_Features = {'Density': lMD,
            'Speed': lMS,
            'Flow': lMF,
            }
    df = pd.DataFrame(Macro_Features, columns= ['Density', 'Speed','Flow'])

    df.to_csv('Macro_lanedensity.csv')



if __name__ == "__main__":
    sys.exit(main(sys.argv))
