#!/usr/bin/perl

sub writeRouteConf($)
{
	my $step = $_[0];
	open(OUTDAT, "> due2s.rou.cfg");
	print OUTDAT "<configuration>\n";
	print OUTDAT "	<files>\n";
	print OUTDAT "		<net-files>due2s.net.xml</net-files>\n";
	print OUTDAT "		<weights></weights>\n";
	if($step==0) {
		print OUTDAT "		<trip-defs>due2s.trips.xml</trip-defs>\n";
	} else {
		print OUTDAT "		<alternatives>due2s.rou.xml.alt</alternatives>\n";
		print OUTDAT "		<weights>aggregated/due2s_300.xml</weights>\n";
	}
	print OUTDAT "		<output>due2s_new.rou.xml</output>\n";
	print OUTDAT "	</files>\n";
	print OUTDAT "	<process>\n";
	print OUTDAT "		<begin>0</begin>\n";
	print OUTDAT "		<end></end>\n";
	print OUTDAT "	</process>\n";
	print OUTDAT "</configuration>\n";
	close OUTDAT;
}

sub writeSumoConf($)
{
	my $step = $_[0];
	open(OUTDAT, "> due2s.sumo.cfg");
	print OUTDAT "<configuration>\n";
	print OUTDAT "	<files>\n";
	print OUTDAT "		<net-files>due2s.net.xml</net-files>\n";
	print OUTDAT "		<route-files>due2s.rou.xml</route-files>\n";
	print OUTDAT "		<dump-intervals>300</dump-intervals>\n";
	print OUTDAT "		<dump-basename>aggregated/due2s</dump-basename>\n";
	print OUTDAT "	</files>\n";
	print OUTDAT "	<simulation>\n";
	print OUTDAT "		<begin>0</begin>\n";
	print OUTDAT "		<end></end>\n";
	print OUTDAT "	</simulation>\n";
	print OUTDAT "</configuration>\n";
	close OUTDAT;
}

# -------------------------------------

if(!defined($ARGV[1])) {
	print "Syntax-Error!\n";
	print "Syntax: iterate.pl <SUMO_PATH> [<BEG_STEP_NUMBER(INT)>] <END_STEP_NUMBER(INT)>\n";
	die;
}
$step=0;
$end = 10;
if(defined($ARGV[2])) {
	$step = $ARGV[1];
	$end = $ARGV[2];
}
for(; $step<$end+1; $step++) {
	writeRouteConf($step);
	system("$ARGV[0]/router.exe -c due2s.rou.cfg --gBeta=0.9 --gA=0.5");
	system("copy due2s_new.rou.xml due2s.rou.xml");
	system("copy due2s_new.rou.xml.alt due2s.rou.xml.alt");
	system("erase due2s_new.rou.xml");
	system("erase due2s_new.rou.xml.alt");
	writeSumoConf($step);
	system("$ARGV[0]/sumo.exe -c due2s.sumo.cfg");
}




