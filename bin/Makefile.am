if WITH_GUI
GUI_APP = sumo-gui
NETEDIT_APP = netedit
endif

CLEANFILES = sumo sumoD sumo-gui sumo-guiD \
dfrouter dfrouterD duarouter duarouterD \
jtrrouter jtrrouterD marouter marouterD \
netconvert netconvertD netedit neteditD \
netgenerate netgenerateD od2trips od2tripsD \
polyconvert polyconvertD activitygen activitygenD \
TraCITestClient TraCITestClientD sumo-unittest sumo-unittestD \
emissionsDrivingCycle emissionsDrivingCycleD emissionsMap emissionsMapD

install: all
all: sumo dfrouter duarouter jtrrouter netconvert netgenerate od2trips polyconvert activitygen emissionsDrivingCycle emissionsMap marouter TraCITestClient $(GUI_APP) $(NETEDIT_APP)
sumo: ../src/sumo
	cp ../src/sumo ./`echo sumo | sed '$(transform)'`
	if [ ! -e sumo -a -e sumoD ]; then ln -s sumoD sumo; fi
dfrouter: ../src/dfrouter/dfrouter
	cp ../src/dfrouter/dfrouter ./`echo dfrouter | sed '$(transform)'`
	if [ ! -e dfrouter -a -e dfrouterD ]; then ln -s dfrouterD dfrouter; fi
duarouter: ../src/duarouter/duarouter
	cp ../src/duarouter/duarouter ./`echo duarouter | sed '$(transform)'`
	if [ ! -e duarouter -a -e duarouterD ]; then ln -s duarouterD duarouter; fi
jtrrouter: ../src/jtrrouter/jtrrouter
	cp ../src/jtrrouter/jtrrouter ./`echo jtrrouter | sed '$(transform)'`
	if [ ! -e jtrrouter -a -e jtrrouterD ]; then ln -s jtrrouterD jtrrouter; fi
netconvert: ../src/netconvert
	cp ../src/netconvert ./`echo netconvert | sed '$(transform)'`
	if [ ! -e netconvert -a -e netconvertD ]; then ln -s netconvertD netconvert; fi
netgenerate: ../src/netgen/netgenerate
	cp ../src/netgen/netgenerate ./`echo netgenerate | sed '$(transform)'`
	if [ ! -e netgenerate -a -e netgenerateD ]; then ln -s netgenerateD netgenerate; fi
od2trips: ../src/od2trips
	cp ../src/od2trips ./`echo od2trips | sed '$(transform)'`
	if [ ! -e od2trips -a -e od2tripsD ]; then ln -s od2tripsD od2trips; fi
polyconvert: ../src/polyconvert/polyconvert
	cp ../src/polyconvert/polyconvert ./`echo polyconvert | sed '$(transform)'`
	if [ ! -e polyconvert -a -e polyconvertD ]; then ln -s polyconvertD polyconvert; fi
activitygen: ../src/activitygen/activitygen
	cp ../src/activitygen/activitygen ./`echo activitygen | sed '$(transform)'`
	if [ ! -e activitygen -a -e activitygenD ]; then ln -s activitygenD activitygen; fi
emissionsDrivingCycle: ../src/tools/emissionsDrivingCycle
	cp ../src/tools/emissionsDrivingCycle ./`echo emissionsDrivingCycle | sed '$(transform)'`
	if [ ! -e emissionsDrivingCycle -a -e emissionsDrivingCycleD ]; then ln -s emissionsDrivingCycleD emissionsDrivingCycle; fi
emissionsMap: ../src/tools/emissionsMap
	cp ../src/tools/emissionsMap ./`echo emissionsMap | sed '$(transform)'`
	if [ ! -e emissionsMap -a -e emissionsMapD ]; then ln -s emissionsMapD emissionsMap; fi
marouter: ../src/marouter/marouter
	cp ../src/marouter/marouter ./`echo marouter | sed '$(transform)'`
	if [ ! -e marouter -a -e marouterD ]; then ln -s marouterD marouter; fi
$(GUI_APP): ../src/$(GUI_APP)
	cp ../src/$(GUI_APP) ./`echo $(GUI_APP) | sed '$(transform)'`
	if [ ! -e $(GUI_APP) -a -e $(GUI_APP)D ]; then ln -s $(GUI_APP)D $(GUI_APP); fi
$(NETEDIT_APP):	../src/netedit/$(NETEDIT_APP)
	cp ../src/netedit/$(NETEDIT_APP) ./`echo $(NETEDIT_APP) | sed '$(transform)'`
	if [ ! -e $(NETEDIT_APP) -a -e $(NETEDIT_APP)D ]; then ln -s $(NETEDIT_APP)D $(NETEDIT_APP); fi
TraCITestClient: ../src/traci_testclient/TraCITestClient
	cp ../src/traci_testclient/TraCITestClient ./`echo TraCITestClient | sed '$(transform)'`
	if [ ! -e TraCITestClient -a -e TraCITestClientD ]; then ln -s TraCITestClientD TraCITestClient; fi
