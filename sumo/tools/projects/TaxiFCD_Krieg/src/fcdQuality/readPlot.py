
from pylab import * 
from os import walk
from os.path import join

#index = int(sys.argv[2])

#0: Frequenz; ->belegt
#1: Ausstattungsgrad; ->belegt
#2: ausgewaehlterFhzg; ->nein
#3: Proben; ->?was ist das
#4: erkannter Kanten; ->abs Wert rel Besser
#5: nichtErkannterKanten; ->abs Wert rel Besser
#6: edgeSmaples(immer gleich);> ->?was ist das 
#7: FhzgSamples; ->nein
#8: GeschwEdge; ->nein
#9: GeschwFhzg; ->nein
#10: absDiffGeschw; ->rel besser
#11: relDiffGeschw; ->ja
#12: %erkannter Kanten; ->darstellung 13 besser doch 12
#13: %nicht erkannterKanten ->ja
index=11

#absSwitch decide if the Value should be transform into an absolute Value
absSwitch=True
xt = []
yt = []
filePath="D:/Krieg/Projekte/Diplom/Daten/fcdQualitaet/readPlotData/outDir/"


def main(): 
    print "start program"
    plotData()
    print "end"

def fetchData(): 
    valueDict = {}
    firstFile=True
    for root, dirs, files in walk(filePath):
        for fileName in files:       
            inputFile=open(join(root,fileName),'r')                
            for line in inputFile:
                line = line.strip()
                words = line.split(";")
                
                period = float(words[0])
                quota = float(words[1])
                value = float(words[index])        
                if absSwitch:
                    value = abs(value)
                valueDict.setdefault(period,{}).setdefault(quota,[]).append(value)            
               
                #get the used periods and quotas
                if firstFile and period not in xt:
                    xt.append(period)
                if firstFile and len(xt)==1:            
                    yt.append(quota)
            inputFile.close()
            firstFile=False  
          
    #create the array for the plot
    marr = []
    for period in xt:
        marr.append([])
        for quota in yt:
            #calc avg of the Value list            
            valueDict[period][quota]=sum(valueDict[period][quota])/len(valueDict[period][quota])             
            #add avg value
            marr[-1].append(valueDict[period][quota])    
    return marr

        
def plotData(): 
    marr=fetchData()       
    textsize=18
    contourf(marr, 30)#levels=arange(mmin-mmin*.1, mmax+mmax*.1, (mmax-mmin)/10.))     
    #set fontsize for the colorbar:
    cb = colorbar() # grab the Colorbar instance
    for t in cb.ax.get_yticklabels():     
         t.set_fontsize(textsize)
     
    yticks([0,1,2,3,4,5,6,7,8,9,10], xt, size=textsize)
    ylabel("Periode [s]", size=textsize)
    xticks([0,1,2,3,4,5,6,7,8,9,10], yt, size=textsize)
    xlabel("Ausstattung [%]", size=textsize)
    title("Abweichung der Geschwindigkeit zwischen FCD und des simulierten Verkehrs", size=textsize)
    #title("Relative Anzahl erfasster Kanten", size=textsize)
    figtext(0.7865,0.92,'[%]', size=textsize)
    show()
    
    
#start the program
main()
