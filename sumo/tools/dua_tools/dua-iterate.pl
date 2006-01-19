#!/usr/bin/perl
# ***************************************************************************
#                          dua-iterate.pl
# Iterates over dua steps
#                             -------------------
#   project              : SUMO
#   subproject           : dua tools
#   begin                : Fri, 02.11.2005
#   copyright            : (C) 2005 by DLR/IVF http://ivf.dlr.de/
#   author               : Daniel Krajzewicz
#   email                : Daniel.Krajzewicz@dlr.de
# ***************************************************************************

#***************************************************************************
#*                                                                         *
#*   This program is free software; you can redistribute it and/or modify  *
#*   it under the terms of the GNU General Public License as published by  *
#*   the Free Software Foundation; either version 2 of the License, or     *
#*   (at your option) any later version.                                   *
#*                                                                         *
#***************************************************************************
# $Log$
# Revision 1.1  2006/01/19 08:01:06  dkrajzew
# tools rechecked
#
#

sub writeRouteConf($)
{
	my $step = $_[0];
	open(OUTDAT, "> sumo-router.rou.cfg");
	print OUTDAT "<configuration>\n";
	print OUTDAT "	<files>\n";
	print OUTDAT "		<net-file>net.net.xml</net-file>\n";
	if($step==0) {
		print OUTDAT "		<t>trips.trips.xml</t>\n";
	} else {
		$route_name = "routes_".($step-1).".rou.xml.alt";
		print OUTDAT "		<alternatives>".$route_name."</alternatives>\n";
		$dump_name = "dump_".($step-1)."_900.xml";
		print OUTDAT "		<weights>".$dump_name."</weights>\n";
	}
	$routeout_name = "routes_".$step.".rou.xml";
	print OUTDAT "		<output>".$routeout_name."</output>\n";
	print OUTDAT "	</files>\n";
	print OUTDAT "	<process>\n";
	print OUTDAT "		<begin>0</begin>\n";
	print OUTDAT "		<end>86400</end>\n";
	print OUTDAT "	</process>\n";
	print OUTDAT "	<reports>\n";
	print OUTDAT "		<verbose>v</verbose>\n";
	print OUTDAT "		<continue-on-unbuild>v</continue-on-unbuild>\n";
	print OUTDAT "	</reports>\n";
	print OUTDAT "</configuration>\n";
	close OUTDAT;
}

sub writeSumoConf($)
{
	my $step = $_[0];
	open(OUTDAT, "> sumo.sumo.cfg");
	print OUTDAT "<configuration>\n";
	print OUTDAT "	<files>\n";
	print OUTDAT "		<net-file>net.net.xml</net-file>\n";
	$routeout_name = "routes_".$step.".rou.xml";
	print OUTDAT "		<route-files>".$routeout_name."</route-files>\n";
	print OUTDAT "		<dump-intervals>150;300;900</dump-intervals>\n";
	$dump_base = "dump_".$step."";
	print OUTDAT "		<dump-basename>".$dump_base."</dump-basename>\n";
	$trip_name = "tripinfo_".$step.".rou.xml";
	print OUTDAT "          <tripinfo>".$trip_name."</tripinfo>\n";
	$emissions_name = "emissions_".$step.".rou.xml";
	print OUTDAT "          <emissions>".$emissions_name."</emissions>\n";
	print OUTDAT "	</files>\n";
	print OUTDAT "	<simulation>\n";
	print OUTDAT "          <route-steps>200</route-steps>\n";
	print OUTDAT "		<begin>0</begin>\n";
	print OUTDAT "		<end>86400</end>\n";
	print OUTDAT "	</simulation>\n";
	print OUTDAT "	<reports>\n";
	print OUTDAT "		<suppress-warnings>v</suppress-warnings>\n";
	print OUTDAT "	</reports>\n";
	print OUTDAT "</configuration>\n";
	close OUTDAT;
}

# -------------------------------------


# check the settings
if(!defined($ARGV[1])) {
	print "Syntax-Error!\n";
	print "Syntax: dua-iterate.pl <SUMO_PATH> [<BEG_STEP_NUMBER(INT)>] <END_STEP_NUMBER(INT)>\n";
	die;
}

# set steps
$step=0;
$end = 10;
if(defined($ARGV[2])) {
	$step = $ARGV[1];
	$end = $ARGV[2];
} else {
	$end = $ARGV[1];
}

#
for(; $step<$end+1; $step++) {
	writeRouteConf($step);
	if(system("$ARGV[0]/duarouter -c sumo-router.rou.cfg  --suppress-warnings --gBeta=0.9 --gA=0.5")!=0) {
		die "Error in Routing!!!\n";
	}
	writeSumoConf($step);
	if(system("$ARGV[0]/sumo -c sumo.sumo.cfg -v")!=0) {
		die "Error in Simulation!!!\n";
	}
}




