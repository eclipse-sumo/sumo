if WITH_GUI
GUI_APPS = sumo-gui
endif

if TRACI
TRACI_TESTCLIENT_APPS = traci-testclient
endif

all: sumo dfrouter duarouter jtrrouter netconvert netgen od2trips polyconvert $(GUI_APPS) $(TRACI_TESTCLIENT_APPS)
sumo: ../src/sumo
	cp ../src/sumo .
dfrouter: ../src/dfrouter/dfrouter
	cp ../src/dfrouter/dfrouter .
duarouter: ../src/duarouter/duarouter
	cp ../src/duarouter/duarouter .
jtrrouter: ../src/jtrrouter/jtrrouter
	cp ../src/jtrrouter/jtrrouter .
netconvert: ../src/netconvert
	cp ../src/netconvert .
netgen: ../src/netgen/netgen
	cp ../src/netgen/netgen .
od2trips: ../src/od2trips/od2trips
	cp ../src/od2trips/od2trips .
polyconvert: ../src/polyconvert/polyconvert
	cp ../src/polyconvert/polyconvert .
$(GUI_APPS): ../src/$(GUI_APPS)
	cp ../src/$(GUI_APPS) .
$(TRACI_TESTCLIENT_APPS): ../src/$(TRACI_TESTCLIENT_APPS)
	cp ../src/$(TRACI_TESTCLIENT_APPS) .
