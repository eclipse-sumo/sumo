if WITH_GUI
GUI_APPS = sumo-gui
endif

if TRACI
TRACI_TESTCLIENT_APPS = TraCITestClient
endif

DISTCLEANFILES = sumo sumoD dfrouter dfrouterD duarouter duarouterD jtrrouter jtrrouterD netconvert netconvertD netgen netgenD od2trips od2tripsD polyconvert polyconvertD activitygen activitygenD sumo-unittest sumo-unittestD sumo-gui sumo-guiD TraCITestClient TraCITestClientD meso mesoD meso-gui meso-guiD

all: sumo dfrouter duarouter jtrrouter netconvert netgen od2trips polyconvert activitygen $(GUI_APPS) $(TRACI_TESTCLIENT_APPS)
sumo: ../src/sumo
	cp ../src/sumo ./`echo sumo | sed '$(transform)'`
	if [ ! -e sumo -a -e meso ]; then ln -s meso sumo; fi
	if [ ! -e sumoD -a -e mesoD ]; then ln -s mesoD sumoD; fi
dfrouter: ../src/dfrouter/dfrouter
	cp ../src/dfrouter/dfrouter ./`echo dfrouter | sed '$(transform)'`
duarouter: ../src/duarouter/duarouter
	cp ../src/duarouter/duarouter ./`echo duarouter | sed '$(transform)'`
jtrrouter: ../src/jtrrouter/jtrrouter
	cp ../src/jtrrouter/jtrrouter ./`echo jtrrouter | sed '$(transform)'`
netconvert: ../src/netconvert
	cp ../src/netconvert ./`echo netconvert | sed '$(transform)'`
netgen: ../src/netgen/netgen
	cp ../src/netgen/netgen ./`echo netgen | sed '$(transform)'`
od2trips: ../src/od2trips/od2trips
	cp ../src/od2trips/od2trips ./`echo od2trips | sed '$(transform)'`
polyconvert: ../src/polyconvert/polyconvert
	cp ../src/polyconvert/polyconvert ./`echo polyconvert | sed '$(transform)'`
activitygen: ../src/activitygen/activitygen
	cp ../src/activitygen/activitygen ./`echo activitygen | sed '$(transform)'`
$(GUI_APPS): ../src/$(GUI_APPS)
	cp ../src/$(GUI_APPS) ./`echo $(GUI_APPS) | sed '$(transform)'`
	if [ ! -e sumo-gui -a -e meso-gui ]; then ln -s meso-gui sumo-gui; fi
	if [ ! -e sumo-guiD -a -e meso-guiD ]; then ln -s meso-guiD sumo-guiD; fi
$(TRACI_TESTCLIENT_APPS): ../src/traci_testclient/$(TRACI_TESTCLIENT_APPS)
	cp ../src/traci_testclient/$(TRACI_TESTCLIENT_APPS) ./`echo $(TRACI_TESTCLIENT_APPS) | sed '$(transform)'`
