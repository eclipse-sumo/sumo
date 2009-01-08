
import os,zipfile, time
from os.path import join
from os.path import exists
from time import localtime
from time import strftime
from winsound import Beep
from time import sleep

source="E:/Diplom"
destination="D:/DiplomSicherung/"

interval=3 #Hours
format="%Y-%m-%d %H_%M_%S"


def main():
    initBeep();
    beepKa();
    startBackup()


def startBackup():
    while True:
        date=strftime(format,localtime())

        #check if source is available
        if exists(source):
            try:
                #backup
                beepWr();
                zip = zipfile.ZipFile(destination+date+".zip", 'w', zipfile.ZIP_DEFLATED)
                for root, dirs, files in os.walk(source):
                    #print root
                    #print dirs
                    #print files
                    #print "\n"
                    if len(files)>0:
                        for fileName in files:
                            zip.write(join(root,fileName))
                zip.close()
                log (date+'\twrite backup')
            except StandardError, e:
                beepHh();
                log (date+'\t'+str(e))
            #pasue
            beepBt();
            time.sleep(interval*3600)
        else:
            #check every 10 min if source is available
            log(date+'\tfolder not available')
            time.sleep(10*60)


def log(text):
    if exists(destination):
        day=strftime("%Y-%m-%d",localtime())
        outputFile=open(join(destination,day+'_backup.log'),'a')
        outputFile.write(text+'\n')
        outputFile.close()
    else:
        #Beep Error
        beepHh();
        beepHh();
        beepHh();

#get the Beep
def initBeep():
    global beepKa, beepBt, beepHh, beepWr
    # . (Dit) = 100ms
    # - (Dah) = 300ms = 3 Dit
    # Pause pro Symbol = 1 Dit = 100ms
    # Pause pro Buchs. = 1 Dah = 300ms
    # Pause pro Wort   = 7 Dit = 700ms

    ditDuration=100 #in Ms
    freq=520
    def dit(): Beep(freq,ditDuration)
    def dah(): Beep(freq,3*ditDuration)
    def pSymb(): sleep(ditDuration/1000.0)
    def pLett(): sleep(3*ditDuration/1000.0)
    def pWord(): sleep(7*ditDuration/1000.0)

    #Buchstaben
    def a(): dit();pSymb();dah();
    def c(): dah();pSymb();dit();pSymb();dah();pSymb();dit();
    def e(): dit();
    def g(): dah();pSymb();dah();pSymb();dit();
    def h(): dit();pSymb();dit();pSymb();dit();pSymb();dit();
    def i(): dit();pSymb();dit();
    def o(): dah();pSymb();dah();pSymb();dah();
    def r(): dit();pSymb();dah();pSymb();dit();
    def s(): dit();pSymb();dit();pSymb();dit();
    def t(): dah();
    def w(): dit();pSymb();dah();pSymb();dah();

    def beepKa():dah();pSymb();dit();pSymb();dah();pSymb();dit();pSymb();dah();pWord(); #Spruchanfang
    def beepBt():dah();pSymb();dit();pSymb();dit();pSymb();dit();pSymb();dah();pWord(); #Pause
    def beepHh():h();pSymb();h();pWord(); #Fehler
    def beepWr():w();pSymb();r();pWord(); #write


#start the program
main()
